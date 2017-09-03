//
// Created by carljay on 17-9-3.
//

#ifndef FFMPEG_PROJECT_FFMPEG_OBJECT_H
#define FFMPEG_PROJECT_FFMPEG_OBJECT_H

#include <jni.h>
#include <libavformat/avformat.h>
#include <libavutil/avutil.h>
typedef struct {
    char* path;
    int video_stream_idx,audio_stream_idx;
    AVFormatContext* pFormatCtx;
    AVCodecContext* pVideoCodecCtx,*pAudioCodecCtx;
}FFmpeg_Object;
FFmpeg_Object* getFFmpegObject(JNIEnv*env,jobject jobject1);
int initFFmpegObject(FFmpeg_Object* object);
int setPath(JNIEnv *env,jobject *obj,jstring path);
int start(JNIEnv *env,jobject *obj,int i);
int pause(JNIEnv *env,jobject *obj);
int stop(JNIEnv *env,jobject *obj);
int load(JNIEnv *env,jobject *obj);
#endif //FFMPEG_PROJECT_FFMPEG_OBJECT_H
