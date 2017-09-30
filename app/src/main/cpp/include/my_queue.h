//
// Created by carljay on 17-9-6.
//

#ifndef FFMPEG_PROJECT_QUEUE_H
#define FFMPEG_PROJECT_QUEUE_H

typedef struct QueueNode_{
    struct QueueNode_* next;
    void* packet;
}QueueNode;
typedef struct {
    QueueNode* first,*last;
    int max;
    int curr;

}Queue;
int post_packet(Queue* queue,void* packet);
void* get_packet(Queue* queue);
#endif //FFMPEG_PROJECT_QUEUE_H
