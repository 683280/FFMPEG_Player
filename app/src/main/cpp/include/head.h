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
typedef struct QueueNode_{
    struct QueueNode_* next;
    AVPacket* packet;
}QueueNode;
typedef struct {
    QueueNode* first,*last;
    int max;
    int curr;

}Queue;
typedef struct {
    ANativeWindow* pWindow;
    int height,width;//Surface宽高
    ANativeWindow_Buffer windowBuffer;
    struct SwsContext* pVideoSwsCtx;
    AVFrame* pSwsFrame;//转换之后的一帧
}SurfaceNative;
typedef struct {
    SwrContext* pSwrCtx;
    SLPlayItf sl_player_itf;
    SLAndroidSimpleBufferQueueItf sl_queue_itf;
    SLAndroidEffectItf sl_effect_itf;
    SLVolumeItf sl_volume_itf;
    void* sl_buffer;
    int sl_buffer_size;

}OpenSLES;
typedef struct {
    char* path;
    int video_stream_idx,audio_stream_idx,state;
    AVFormatContext* pFormatCtx;
    AVFrame* pVideoFrame,*pAudioFrame;
    AVCodecContext* pVideoCodecCtx,*pAudioCodecCtx;
    SurfaceNative*pSurfaceNative;
    OpenSLES* openSLES;
    Queue* pAudioQueue,*pVideoQueue;
    void(*get_audio_data)(void* context);
}FFmpeg_Object;

#endif //FFMPEG_PLAYER_HEAD_H
