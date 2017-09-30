//
// Created by carljay on 17-9-3.
//

#ifndef FFMPEG_PROJECT_FFMPEG_OBJECT_H
#define FFMPEG_PROJECT_FFMPEG_OBJECT_H
#define JNI_PARAMETER JNIEnv *env,jobject *obj
#include <jni.h>
#include <libavformat/avformat.h>
#include <libavutil/avutil.h>
#include "surface_native.h"
#include "head.h"
#include <codec.h>

//FFmpeg_Object* getFFmpegObject(JNIEnv*env,jobject jobject1);
int findFFmpegStream();
int setSurface(JNI_PARAMETER,jobject* surface,int width,int height);
int initFFmpegObject(FFmpeg_Object* object);
int setPath(JNI_PARAMETER,jstring path);
int start(JNI_PARAMETER);
int seek(JNI_PARAMETER,int i);
int pause(JNI_PARAMETER);
int stop(JNI_PARAMETER);
int load(JNI_PARAMETER);
void t_load(FFmpeg_Object* object);
void t_decodec_video(FFmpeg_Object* object);
void t_decodec_audio(FFmpeg_Object* object);
#endif //FFMPEG_PROJECT_FFMPEG_OBJECT_H
