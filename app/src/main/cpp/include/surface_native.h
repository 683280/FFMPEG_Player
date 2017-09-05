//
// Created by 13342 on 2017/9/4.
//

#ifndef FFMPEG_PLAYER_SURFACENATIVE_H
#define FFMPEG_PLAYER_SURFACENATIVE_H

#include <android/native_window_jni.h>
#include <android/native_window.h>
#include "ffmpeg_object.h"
#include "head.h"
int init_video_sws(FFmpeg_Object* object);
int postFrame(FFmpeg_Object* object);
int initSurfaceNative(FFmpeg_Object* object);
#endif //FFMPEG_PLAYER_SURFACENATIVE_H
