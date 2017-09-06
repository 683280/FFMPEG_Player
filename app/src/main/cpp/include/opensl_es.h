//
// Created by 13342 on 2017/9/5.
//

#ifndef FFMPEG_PLAYER_OPENSL_ES_H
#define FFMPEG_PLAYER_OPENSL_ES_H

#include "head.h"
int initOpenSlEngine();
int destoryOpenSl();

int create_sles_player(int numChannels,int samplesPerSecint,int bitsPerSample);
int initOpenslPlayer(FFmpeg_Object* object);
void sles_callback(SLBufferQueueItf bq, void *context);
int post_audio_data(FFmpeg_Object* object);
int play(OpenSLES* openSLES);
int pause();
int stop();
#endif //FFMPEG_PLAYER_OPENSL_ES_H
