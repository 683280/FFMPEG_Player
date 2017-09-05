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
typedef struct {
    ANativeWindow* pWindow;
    int height,width;//Surface宽高
    ANativeWindow_Buffer windowBuffer;
    struct SwsContext* pVideoSwsCtx;
    AVFrame* pSwsFrame;//转换之后的一帧
}SurfaceNative;

typedef struct {
    char* path;
    int video_stream_idx,audio_stream_idx;
    AVFormatContext* pFormatCtx;
    AVFrame* pVideoFrame,*pAudioFrame;
    AVCodecContext* pVideoCodecCtx,*pAudioCodecCtx;
    SurfaceNative*pSurfaceNative;
}FFmpeg_Object;

typedef struct {

}OpenSLES;
#endif //FFMPEG_PLAYER_HEAD_H
