//
// Created by 13342 on 2017/9/4.
//

#ifndef FFMPEG_PLAYER_HEAD_H
#define FFMPEG_PLAYER_HEAD_H
#include <libswscale/swscale.h>
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <android/native_window.h>
#include <SLES/OpenSLES.h>
#include <SLES/OpenSLES_Android.h>
#include <libswresample/swresample.h>
#define OBJECT_STATE_UNINIT 0
#define OBJECT_STATE_INIT 1
#define OBJECT_STATE_PLAYING 2
#define OBJECT_STATE_RECORDING 3
#define OBJECT_STATE_END 4
//#define OBJECT_STATE_ 2

typedef struct {
    ANativeWindow* pWindow;
    int height,width;//Surface宽高
    ANativeWindow_Buffer windowBuffer;
    struct SwsContext* pVideoSwsCtx;
    AVFrame* pSwsFrame;//转换之后的一帧
    Queue* pQueue;
}SurfaceNative;
typedef struct {

    SwrContext* pSwrCtx;
    //opensl player结构体
    SLPlayItf sl_player_itf;
    SLAndroidSimpleBufferQueueItf sl_player_queue_itf;
    SLAndroidEffectItf sl_effect_itf;
    SLVolumeItf sl_volume_itf;
    uint8_t * sl_buffer;
    int sl_buffer_size;
    //opensl record结构体
    SLRecordItf sl_record_itf;
    SLAndroidSimpleBufferQueueItf sl_record_queue_itf;
    //
    int data_size;
    int(*get_pcm)(void* context);
    int(*save_pcm)(void* context);
}OpenSLES;
typedef struct {
    char* path;
    int video_stream_idx,audio_stream_idx,state;
    AVFormatContext* pFormatCtx;
    AVFrame* pVideoFrame,*pAudioFrame;
    AVCodecContext* pVideoCodecCtx,*pAudioCodecCtx;
    SurfaceNative*pSurfaceNative;
    OpenSLES* pOpenSLES;
    Queue* pAudioQueue,*pVideoQueue;
    void(*get_audio_data)(void* context);
}FFmpeg_Object;

#endif //FFMPEG_PLAYER_HEAD_H
