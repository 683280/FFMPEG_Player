//
// Created by carljay on 17-9-6.
//

#ifndef FFMPEG_PROJECT_QUEUE_H
#define FFMPEG_PROJECT_QUEUE_H

#include "head.h"
int post_packet(Queue* queue,AVPacket* packet);
AVPacket* get_packet(Queue* queue);
#endif //FFMPEG_PROJECT_QUEUE_H
