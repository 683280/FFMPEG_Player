//
// Created by 13342 on 2017/9/5.
//

#include <Log.h>
#include <head.h>
#include <libswresample/swresample.h>
#include <libavutil/opt.h>
#include "opensl_native.h"

int create_opensl_player(FFmpeg_Object* object) {
    AVCodecContext* audioCtx = object->pAudioCodecCtx;
    const SLInterfaceID ids[3] = {SL_IID_BUFFERQUEUE, SL_IID_EFFECTSEND,
            /*SL_IID_MUTESOLO,*/ SL_IID_VOLUME};
    const SLboolean req[3] = {SL_BOOLEAN_TRUE, SL_BOOLEAN_TRUE,
            /*SL_BOOLEAN_TRUE,*/ SL_BOOLEAN_TRUE};
    SLDataLocator_AndroidSimpleBufferQueue bufferQueue = {SL_DATALOCATOR_ANDROIDSIMPLEBUFFERQUEUE, 2};
    int sample_rate = audioCtx->sample_rate * 1000;
    SLDataFormat_PCM format_pcm = {SL_DATAFORMAT_PCM,
                                   audioCtx->channels,//声道数
                                   sample_rate,//采样率
                                   SL_PCMSAMPLEFORMAT_FIXED_16,//采样大小
                                   SL_PCMSAMPLEFORMAT_FIXED_16,//容器大小
                                   (audioCtx->channels == 2) ? SL_SPEAKER_FRONT_LEFT | SL_SPEAKER_FRONT_RIGHT : SL_SPEAKER_FRONT_CENTER,
                                   SL_BYTEORDER_LITTLEENDIAN};
    SLDataLocator_OutputMix outputMix = {SL_DATALOCATOR_OUTPUTMIX,sl_outmix};
    SLDataSource src = {&bufferQueue,&format_pcm};
    SLDataSink sink = {&outputMix,0};
    (*sl_engine)->CreateAudioPlayer(sl_engine, &sl_player, &src, &sink,
                                   3, ids, req);
    //实例化，注册到opensl引擎
    (*sl_player)->Realize(sl_player, SL_BOOLEAN_FALSE);
    //初始化
    init_opensl_player(object);
    return 0;
}

int init_opensl_player(FFmpeg_Object *object) {
    OpenSLES* openSLES = object->pOpenSLES;
    //初始化播放器
    LOGD("获得播放接口");
    (*sl_player)->GetInterface(sl_player, SL_IID_PLAY, &openSLES->sl_player_itf); /*获得播放接口， 存至sl_player_itf*/
    LOGD("获得缓冲区接口");
    (*sl_player)->GetInterface(sl_player, SL_IID_BUFFERQUEUE,
                               &openSLES->sl_player_queue_itf); /*获得缓冲区接口，存至 _aud_buf */
    LOGD("获得音效接口");
    (*sl_player)->GetInterface(sl_player, SL_IID_EFFECTSEND,
                               &openSLES->sl_effect_itf); /*获得音效接口，存至 _aud_bufefx */
    LOGD("获取音量接口");
    (*sl_player)->GetInterface(sl_player, SL_IID_VOLUME, &openSLES->sl_volume_itf); /*获得音量接口，存至 _aud_vol */
    LOGD("初始化播放器4");
    (*openSLES->sl_player_queue_itf)->RegisterCallback(openSLES->sl_player_queue_itf, player_callback, object);

    //init ffmpeg audio swr
    AVCodecContext* aCodecCtx = object->pAudioCodecCtx;
    SwrContext * swr= swr_alloc();
    av_opt_set_int(swr, "in_channel_layout", aCodecCtx->channel_layout, 0);
    av_opt_set_int(swr, "out_channel_layout", aCodecCtx->channel_layout, 0);
    av_opt_set_int(swr, "in_sample_rate", aCodecCtx->sample_rate, 0);
    av_opt_set_int(swr, "out_sample_rate", aCodecCtx->sample_rate, 0);
    av_opt_set_sample_fmt(swr, "in_sample_fmt", aCodecCtx->sample_fmt, 0);
    av_opt_set_sample_fmt(swr, "out_sample_fmt", AV_SAMPLE_FMT_S16, 0);
    swr_init(swr);
    openSLES->pSwrCtx = swr;
    return 0;
}

void player_callback(SLBufferQueueItf bq, void *context) {
    FFmpeg_Object* object = context;
    OpenSLES* openSLES = object->pOpenSLES;
    openSLES->get_pcm(context);
    if (NULL != openSLES->sl_buffer && 0 != openSLES->data_size)
        (*bq)->Enqueue(bq,openSLES->sl_buffer,openSLES->data_size);
}


int play(OpenSLES *openSLES) {
    if(openSLES == 0){
        return  -1;
    }
    (*openSLES->sl_player_itf)->SetPlayState(openSLES->sl_player_itf, SL_PLAYSTATE_PLAYING);
    (*openSLES->sl_player_queue_itf)->Enqueue(openSLES->sl_player_queue_itf, openSLES->sl_buffer, openSLES->sl_buffer_size);
    return 0;
}

int create_opensl_record(FFmpeg_Object* object) {
    // 参数
    int samplerate;
    SLDataLocator_IODevice deviceInputLocator = { SL_DATALOCATOR_IODEVICE, SL_IODEVICE_AUDIOINPUT, SL_DEFAULTDEVICEID_AUDIOINPUT, NULL };
    SLDataSource inputSource = { &deviceInputLocator, NULL };

    SLDataLocator_AndroidSimpleBufferQueue inputLocator = { SL_DATALOCATOR_ANDROIDSIMPLEBUFFERQUEUE, 1 };
    SLDataFormat_PCM inputFormat = { SL_DATAFORMAT_PCM, 2, samplerate * 1000, SL_PCMSAMPLEFORMAT_FIXED_16, SL_PCMSAMPLEFORMAT_FIXED_16, SL_SPEAKER_FRONT_LEFT | SL_SPEAKER_FRONT_RIGHT, SL_BYTEORDER_LITTLEENDIAN };

    SLDataSink inputSink = { &inputLocator, &inputFormat };

    const SLInterfaceID inputInterfaces[2] = { SL_IID_ANDROIDSIMPLEBUFFERQUEUE, SL_IID_ANDROIDCONFIGURATION };

    const SLboolean requireds[2] = { SL_BOOLEAN_TRUE, SL_BOOLEAN_FALSE };
    // 创建AudioRecorder
    (*sl_engine)->CreateAudioRecorder(sl_engine, &sl_record, &inputSource, &inputSink, 2, inputInterfaces, requireds);
    init_opensl_record(object->pOpenSLES);
    return 0;
}

int init_opensl_record(OpenSLES* openSLES) {

    SLresult result;

    // 初始化AudioRecorder
    result = (*sl_record)->Realize(sl_record, SL_BOOLEAN_FALSE);

    // 获取音频输入的BufferQueue接口
    result = (*sl_record)->GetInterface(sl_record, SL_IID_ANDROIDSIMPLEBUFFERQUEUE, &openSLES->sl_record_queue_itf);

    // 获取录制器接口
    SLRecordItf audioRecorderInterface;
    (*sl_record)->GetInterface(sl_record, SL_IID_RECORD, &audioRecorderInterface);
    //录制回调
    result = (*openSLES->sl_record_queue_itf)->RegisterCallback(openSLES->sl_record_queue_itf, record_callback, NULL);
    return result;
}

void record_callback(SLBufferQueueItf bq, void *context) {

}