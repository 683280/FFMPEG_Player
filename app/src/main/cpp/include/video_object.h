//
// Created by 13342 on 2017/9/27.
//

#ifndef FFMPEG_PLAYER_VIDEO_OBJECT_H
#define FFMPEG_PLAYER_VIDEO_OBJECT_H

#include <android/native_window.h>
#include <libavutil/frame.h>
extern "C"{
#include <my_queue.h>
}


class VideoObject{
private:
    ANativeWindow_Buffer windowBuffer;
    ANativeWindow* pWindow;
    int height,width;//Surface宽高
    Queue* queue;
private:
    int post_frame(AVFrame* frame);
    static void * _start(void *videoObject);
public:
    VideoObject(Queue* queue);
    int start_play();
    int set_surface(ANativeWindow* surface);

};
#endif //FFMPEG_PLAYER_VIDEO_OBJECT_H
