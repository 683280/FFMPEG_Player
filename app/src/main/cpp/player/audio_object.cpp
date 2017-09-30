//
// Created by 13342 on 2017/9/27.
//

#include <SLES/OpenSLES.h>
#include <audio_object.h>
#include <pthread.h>
#include <Log.h>

SLObjectItf sl_object;
SLEngineItf sl_engine;
SLObjectItf sl_outmix;
SLObjectItf sl_player;
SLObjectItf sl_record;
int create_opensl_engine() {
    slCreateEngine(&sl_object, 0, NULL, 0, NULL, NULL);
    (*sl_object)->Realize(sl_object,0);
    (*sl_object)->GetInterface(sl_object,SL_IID_ENGINE,&sl_engine);
    if (!sl_engine){
        return -1;
    }
    const SLInterfaceID ids[] = {SL_IID_VOLUME};
    const SLboolean req[] = {SL_BOOLEAN_FALSE};
    (*sl_engine)->CreateOutputMix(sl_engine,&sl_outmix,0,0,0);
    (*sl_outmix)->Realize(sl_outmix,0);
    return 0;
}

int destory_opensl_engine() {
    (*sl_outmix)->Destroy(sl_outmix);
    (*sl_object)->Destroy(sl_object);
    return 0;
}

AudioObject::AudioObject() {
    play_buffer_size = 8196;
    play_buffer = (uint8_t *)malloc(sizeof(uint8_t) * play_buffer_size);
}
AudioObject::~AudioObject() {
    free(play_buffer);
}
int AudioObject::create_opensl_player() {
    AVCodecContext* audioCtx = play_codec_ctx;
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
    init_opensl_player();
    return 0;
}

int AudioObject::init_opensl_player() {
    //初始化播放器
    LOGD("获得播放接口");
    (*sl_player)->GetInterface(sl_player, SL_IID_PLAY, &sl_player_itf); /*获得播放接口， 存至sl_player_itf*/
    LOGD("获得缓冲区接口");
    (*sl_player)->GetInterface(sl_player, SL_IID_BUFFERQUEUE,
                               &sl_player_queue_itf); /*获得缓冲区接口，存至 _aud_buf */
    LOGD("获得音效接口");
    (*sl_player)->GetInterface(sl_player, SL_IID_EFFECTSEND,
                               &sl_effect_itf); /*获得音效接口，存至 _aud_bufefx */
    LOGD("获取音量接口");
    (*sl_player)->GetInterface(sl_player, SL_IID_VOLUME, &sl_volume_itf); /*获得音量接口，存至 _aud_vol */
    LOGD("初始化播放器4");
    (*sl_player_queue_itf)->RegisterCallback(sl_player_queue_itf, play_callback, this);

    //init ffmpeg audio swr
    SwrContext * swr= swr_alloc();
    av_opt_set_int(swr, "in_channel_layout", play_codec_ctx->channel_layout, 0);
    av_opt_set_int(swr, "out_channel_layout", play_codec_ctx->channel_layout, 0);
    av_opt_set_int(swr, "in_sample_rate", play_codec_ctx->sample_rate, 0);
    av_opt_set_int(swr, "out_sample_rate", play_codec_ctx->sample_rate, 0);
    av_opt_set_sample_fmt(swr, "in_sample_fmt", play_codec_ctx->sample_fmt, 0);
    av_opt_set_sample_fmt(swr, "out_sample_fmt", AV_SAMPLE_FMT_S16, 0);
    swr_init(swr);
    play_swr = swr;
    return 0;
}
int AudioObject::get_pcm(void** buffer, int* data_size) {
    int size;
    AVFrame* frame = (AVFrame *) get_packet(play_queue);
    if (play_codec_ctx->sample_fmt == AV_SAMPLE_FMT_S16P) {
        size = av_samples_get_buffer_size(frame->linesize, play_codec_ctx->channels,play_codec_ctx->frame_size,play_codec_ctx->sample_fmt, 1);
    }else {
        av_samples_get_buffer_size(&size, play_codec_ctx->channels,play_codec_ctx->frame_size,play_codec_ctx->sample_fmt, 1);
    }
    // 这里内存再分配可能存在问题
    if (size > play_buffer_size) {
        play_buffer_size = size;
        play_buffer = (uint8_t *) realloc(play_buffer,
                                                  sizeof(uint8_t) * play_buffer_size);
    }
    // 音频格式转换
    swr_convert(play_swr, &play_buffer,size,
                (uint8_t const **) (frame->extended_data),
                frame->nb_samples);
    *buffer = play_buffer;
    *data_size = size;
//    av_frame_free(&frame);
    return 0;
}

int AudioObject::save_pcm(void *buffer, unsigned int data_size) {
    AVFrame* frame;
//    swr_convert_frame(record_swr,frame,)
    return 0;
}

int AudioObject::get_record_pcm(SLBufferQueueItf bq) {
    (*bq)->Enqueue(bq, record_buffer, record_buffer_size);
    save_pcm(record_buffer,record_buffer_size);
    return 0;
}

int AudioObject::start_play() {
    (*sl_player_itf)->SetPlayState(sl_player_itf, SL_PLAYSTATE_PLAYING);
    (*sl_player_queue_itf)->Enqueue(sl_player_queue_itf, play_buffer, play_buffer_size);
    return 0;
}

void AudioObject::set_play_codec_ctx(AVCodecContext *play_codec_ctx) {
    this->play_codec_ctx = play_codec_ctx;
    create_opensl_player();
}

void AudioObject::set_record_codec_ctx(AVCodecContext *record_codec_ctx) {
    this->record_codec_ctx = record_codec_ctx;
}

void play_callback(SLAndroidSimpleBufferQueueItf bq, void *context) {
    AudioObject* audioObject = (AudioObject *) context;
    void* buffer;
    int data_size;
    audioObject->get_pcm(&buffer,&data_size);
    if (NULL != buffer && 0 != data_size)
        (*bq)->Enqueue(bq,buffer,data_size);
}
void record_callback(SLBufferQueueItf bq, void *context){
    AudioObject* audioObject = (AudioObject *) context;
    audioObject->get_record_pcm(bq);
}







