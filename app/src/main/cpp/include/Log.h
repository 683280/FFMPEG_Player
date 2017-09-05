//
// Created by 13342 on 2017/9/4.
//

#ifndef FFMPEG_PLAYER_LOG_H
#define FFMPEG_PLAYER_LOG_H

#include <android/log.h>
#define LOGV(...) __android_log_print(ANDROID_LOG_VERBOSE, "ProjectName", __VA_ARGS__)
#define LOGD(...) __android_log_print(ANDROID_LOG_DEBUG , "ProjectName", __VA_ARGS__)
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO  , "ProjectName", __VA_ARGS__)
#define LOGW(...) __android_log_print(ANDROID_LOG_WARN  , "ProjectName", __VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR  , "ProjectName", __VA_ARGS__)
#endif //FFMPEG_PLAYER_LOG_H
