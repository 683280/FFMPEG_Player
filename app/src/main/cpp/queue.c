//
// Created by carljay on 17-9-6.
//

#include <head.h>
#include <queue.h>
#include <Log.h>

int post_packet(Queue* queue,AVPacket *packet) {
    while (queue->curr == queue->max){
//        LOGE("post_packet = max");
    }
    QueueNode* node = calloc(sizeof(QueueNode),1);
    node->packet = packet;
    queue->curr++;
    if (queue->first == 0){
        queue->first = node;
        queue->last = node;
        return 0;
    }
    queue->last->next = node;
    queue->last = node;
    return 0;
}

AVPacket *get_packet(Queue* queue) {
    while (queue->first == 0){

//        LOGE("get_packet = 0 max = %d",queue->max);
    }
    queue->curr--;
    QueueNode* node = queue->first;
    queue->first = node->next;
    return node->packet;
}
