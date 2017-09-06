//
// Created by 13342 on 2017/9/5.
//

#include <Log.h>
#include <head.h>
#include <libswresample/swresample.h>
#include <libavutil/opt.h>
#include "opensl_es.h"
SLObjectItf sl_object;
SLEngineItf sl_engine;
SLObjectItf sl_outmix;
SLObjectItf sl_player;
int initOpenSlEngine() {
    slCreateEngine(&sl_object, 0, NULL, 0, NULL, NULL);
    (*sl_object)->Realize(sl_object,0);
    (*sl_object)->GetInterface(sl_object,SL_IID_ENGINE,&sl_engine);
    if (!sl_engine){
        return -1;
    }
    (*sl_engine)->CreateOutputMix(sl_engine,&sl_outmix,0,0,0);
    (*sl_outmix)->Realize(sl_outmix,0);

    return 0;
}

int destoryOpenSl() {
    (*sl_outmix)->Destroy(&sl_outmix);
    (*sl_object)->Destroy(&sl_object);
    return 0;
}

int create_sles_player(int numChannels,int samplesPerSecint,int bitsPerSample) {
    const SLInterfaceID ids[3] = {SL_IID_BUFFERQUEUE, SL_IID_EFFECTSEND,
            /*SL_IID_MUTESOLO,*/ SL_IID_VOLUME};
    const SLboolean req[3] = {SL_BOOLEAN_TRUE, SL_BOOLEAN_TRUE,
            /*SL_BOOLEAN_TRUE,*/ SL_BOOLEAN_TRUE};
    SLDataLocator_AndroidSimpleBufferQueue bufferQueue = {SL_DATALOCATOR_ANDROIDSIMPLEBUFFERQUEUE, 2};
    SLDataFormat_PCM format_pcm = {SL_DATAFORMAT_PCM,
                                   numChannels,//声道数
                                   samplesPerSecint * 1000,//采样率
                                   bitsPerSample,//采样大小
                                   bitsPerSample,//容器大小
                                   (numChannels == 2) ? SL_SPEAKER_FRONT_LEFT | SL_SPEAKER_FRONT_RIGHT : SL_SPEAKER_FRONT_CENTER,
                                   SL_BYTEORDER_LITTLEENDIAN};

    SLDataLocator_OutputMix outputMix = {SL_DATALOCATOR_OUTPUTMIX,sl_outmix};
    SLDataSource src = {&bufferQueue,&format_pcm};
    SLDataSink sink = {&outputMix,0};
    (*sl_engine)->CreateAudioPlayer(sl_engine, &sl_player, &src, &sink,
                                   3, ids, req);
    //实例化，注册到opensl引擎
    (*sl_player)->Realize(sl_player, SL_BOOLEAN_FALSE);

    return 0;
}

int initOpenslPlayer(FFmpeg_Object *object) {

    OpenSLES* openSLES = malloc(sizeof(OpenSLES));
    openSLES->sl_buffer_size = 8196;
    openSLES->sl_buffer = malloc(sizeof(uint8_t) * openSLES->sl_buffer_size);
    object->openSLES = openSLES;
    AVCodecContext* codecContext = object->pAudioCodecCtx;
    create_sles_player(codecContext->channels,codecContext->sample_rate,SL_PCMSAMPLEFORMAT_FIXED_16);
    //初始化播放器
    LOGD("获得播放接口");
    (*sl_player)->GetInterface(sl_player, SL_IID_PLAY, &openSLES->sl_player_itf); /*获得播放接口， 存至_aud_ply*/
    LOGD("获得缓冲区接口");
    (*sl_player)->GetInterface(sl_player, SL_IID_BUFFERQUEUE,
                               &openSLES->sl_queue_itf); /*获得缓冲区接口，存至 _aud_buf */
    LOGD("获得音效接口");
    (*sl_player)->GetInterface(sl_player, SL_IID_EFFECTSEND,
                               &openSLES->sl_effect_itf); /*获得音效接口，存至 _aud_bufefx */
    LOGD("获取音量接口");
    (*sl_player)->GetInterface(sl_player, SL_IID_VOLUME, &openSLES->sl_volume_itf); /*获得音量接口，存至 _aud_vol */
    LOGD("初始化播放器4");
    (*openSLES->sl_player_itf)->RegisterCallback(openSLES->sl_queue_itf, sles_callback, object);
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

    object->pAudioQueue = calloc(sizeof(Queue),1);
    return 0;
}

void sles_callback(SLBufferQueueItf bq, void *context) {
    LOGE("sles_callback");
    FFmpeg_Object* object = context;
    object->get_audio_data(context);
}

int play(OpenSLES *openSLES) {
    if(openSLES == 0){
        return  -1;
    }
    (*openSLES->sl_player_itf)->SetPlayState(openSLES->sl_player_itf, SL_PLAYSTATE_PLAYING);
    return 0;
}

int post_audio_data(FFmpeg_Object* object) {
    LOGD("post Audio Data");
    OpenSLES* openSLES = object->openSLES;
    AVFrame* aFrame = object->pAudioFrame;
    int data_size = av_samples_get_buffer_size(
            aFrame->linesize, object->pAudioCodecCtx->channels,
            aFrame->nb_samples, object->pAudioCodecCtx->sample_fmt, 1);
    // 这里内存再分配可能存在问题
    if (data_size > openSLES->sl_buffer_size) {
        openSLES->sl_buffer_size = data_size;
        openSLES->sl_buffer = (uint8_t *) realloc(openSLES->sl_buffer,
                                                  sizeof(uint8_t) * openSLES->sl_buffer_size);
    }
    // 音频格式转换
    swr_convert(openSLES->pSwrCtx, &openSLES->sl_buffer, aFrame->nb_samples,
                (uint8_t const **) (aFrame->extended_data),
                aFrame->nb_samples);
    if (NULL != openSLES->sl_buffer && 0 != openSLES->sl_buffer_size)
        (*object->openSLES->sl_queue_itf)->Enqueue(object->openSLES->sl_queue_itf,openSLES->sl_buffer,openSLES->sl_buffer_size);
    return 0;
}
