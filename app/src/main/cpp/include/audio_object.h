//
// Created by 13342 on 2017/9/27.
//

#ifndef FFMPEG_PLAYER_AUDIO_OBJECT_H
#define FFMPEG_PLAYER_AUDIO_OBJECT_H


#include <SLES/OpenSLES_Android.h>
#include <stdint.h>

extern "C"{
#include <libswresample/swresample.h>
#include <libavutil/avutil.h>
#include <libavcodec/avcodec.h>
#include <libavutil/opt.h>
#include <my_queue.h>
}

//创建opensl引擎，必须
int create_opensl_engine();
//销毁opensl引擎，必须
int destory_opensl_engine();
void play_callback(SLAndroidSimpleBufferQueueItf bq, void *context);
void record_callback(SLBufferQueueItf bq, void *context);
class AudioObject {
private:
    //opensl
    SLPlayItf sl_player_itf;
    SLAndroidSimpleBufferQueueItf sl_player_queue_itf;
    SLAndroidEffectItf sl_effect_itf;
    SLVolumeItf sl_volume_itf;
    uint8_t * play_buffer,*record_buffer;
    int play_buffer_size,record_buffer_size;
    //ffmpeg
    SwrContext *play_swr,*record_swr;
    AVCodecContext *play_codec_ctx,*record_codec_ctx;


private:
    int init_opensl_player();
    int create_opensl_player();
public:
    Queue* record_queue;
    Queue* play_queue;
public:
    AudioObject();
    ~AudioObject();
    int start_record();
    int start_play();
    int get_pcm(void** buffer,int* data_size);
    int save_pcm(void* buffer,unsigned int data_size);
    int get_record_pcm(SLBufferQueueItf bq);
    void set_play_codec_ctx(AVCodecContext *play_codec_ctx);
    void set_record_codec_ctx(AVCodecContext *record_codec_ctx);
};


#endif //FFMPEG_PLAYER_AUDIO_OBJECT_H
