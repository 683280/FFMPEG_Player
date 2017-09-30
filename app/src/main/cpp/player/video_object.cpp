//
// Created by 13342 on 2017/9/27.
//

#include <pthread.h>
#include "video_object.h"


VideoObject::VideoObject(Queue *queue) {
    this->queue = queue;
}
int VideoObject::post_frame(AVFrame *rsc) {
    ANativeWindow_lock(pWindow, &windowBuffer, 0);
    uint8_t *dst_data = (uint8_t *)windowBuffer.bits;
    int dst_size = windowBuffer.stride * 4;

    uint8_t *rsc_data = rsc->data[0];
    int rsc_size = rsc->linesize[0];

    for (int i = 0; i < height; i++) {
        memcpy(dst_data + i * dst_size, rsc_data + i * rsc_size, rsc_size * 4);
    }
    ANativeWindow_unlockAndPost(pWindow);
    return 0;
}

int VideoObject::set_surface(ANativeWindow *surface) {
    if(surface){
        pWindow = surface;
       return 0;
    }
    return -1;
}

int VideoObject::start_play() {
    pthread_t p;
    pthread_create(&p,NULL,_start,this);
    pthread_detach(p);
    return 0;
}

void *VideoObject::_start(void *videoObject) {
    VideoObject* videoObject1 = (VideoObject *) videoObject;
    while(1){
        AVFrame* frame = (AVFrame *) get_packet(videoObject1->queue);
        videoObject1->post_frame(frame);
    }
    return NULL;
}
