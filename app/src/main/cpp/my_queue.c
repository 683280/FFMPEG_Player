//
// Created by carljay on 17-9-6.
//

#include <my_queue.h>
#include <Log.h>
#include <malloc.h>

int post_packet(Queue* queue,void *packet) {
    while (queue->curr == queue->max){
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

void *get_packet(Queue* queue) {
    while (queue->first == 0){
    }
    void * pkt;
    QueueNode* node = queue->first;
    queue->first = node->next;
    pkt = node->packet;
    queue->curr--;
    free(node);
    return pkt;
}
