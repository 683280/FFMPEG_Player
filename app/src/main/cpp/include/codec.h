//
// Created by 13342 on 2017/9/11.
//

#ifndef FFMPEG_PLAYER_DECODEC_H
#define FFMPEG_PLAYER_DECODEC_H

#include "head.h"
int decode(AVCodecContext *codecCtx, Queue *queue, AVFrame **f);
#endif //FFMPEG_PLAYER_DECODEC_H
