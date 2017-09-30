//
// Created by 13342 on 2017/9/27.
//

#ifndef FFMPEG_PLAYER_MEDIA_PLAYER_H
#define FFMPEG_PLAYER_MEDIA_PLAYER_H

#include "video_object.h"
#include "../player/ffmpeg_object.h"
#include "audio_object.h"
#define READED 0
#define ENT 4
enum LOAD_STATE{
    LOADING,
    LOADED,
};
enum PLAY_STATE{
    PLARING,
    PAUSE,
    END,
};
class MediaPlayer{
public:

private:
    VideoObject* video_player;
    AudioObject* audio_object;
    FFmpegObject* ffmpeg_object;

    Queue* video_queue,*audio_queue;
    Queue* video_pkt_queue,*audio_pkt_queue;
    char* _path;
    LOAD_STATE load_state;
public:
    MediaPlayer();
    ~MediaPlayer();
    int set_path(char* path);
    int set_surface(void* window,int width,int height);
    int load();
    int start();
    int pause();
    int stop();
    int seek(int position);


};
#endif //FFMPEG_PLAYER_MEDIA_PLAYER_H
