//
// Created by 13342 on 2017/9/5.
//

#ifndef FFMPEG_PLAYER_OPENSL_ES_H
#define FFMPEG_PLAYER_OPENSL_ES_H

#include "head.h"
//创建opensl引擎，必须
int create_opensl_engine();
//销毁opensl引擎，必须
int destory_opensl_engine();
//创建opensl对象(自己写的struct)
int create_opensl(FFmpeg_Object* object);
//销毁opensl对象(自己写的struct
int destory_opensl(FFmpeg_Object* object);
//创建opensl播放对象
int create_opensl_player(FFmpeg_Object* objectle);
//初始化opensl播放对象
int init_opensl_player(FFmpeg_Object* object);
//创建opensl录制对象
int create_opensl_record(FFmpeg_Object* object);
//初始化opensl录制对象
int init_opensl_record(OpenSLES* openSLES);
//播放对象的回调函数
void player_callback(SLBufferQueueItf bq, void *context);
//录制对象的回调函数
void record_callback(SLBufferQueueItf bq, void *context);
//开始播放
int play(OpenSLES* openSLES);
//开始录制
int record(OpenSLES* openSLES);

#endif //FFMPEG_PLAYER_OPENSL_ES_H
