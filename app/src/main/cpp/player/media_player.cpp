//
// Created by 13342 on 2017/9/27.
//

#include <media_player.h>
#include <head.h>
#include <Log.h>

MediaPlayer::MediaPlayer() {
    ffmpeg_object = new FFmpegObject();

    video_queue = new Queue();
    audio_queue = new Queue();
    video_pkt_queue = new Queue();
    audio_pkt_queue = new Queue();

    video_queue->max = 10;
    audio_queue->max = 10;

    ffmpeg_object->video_queue = video_queue;
    ffmpeg_object->audio_queue = audio_queue;

    video_player = new VideoObject(video_queue);
    audio_object = new AudioObject();
    audio_object->play_queue = audio_queue;
}
MediaPlayer::~MediaPlayer() {
    free(ffmpeg_object);
    free(video_player);
    free(audio_object);
    free(video_queue);
    free(audio_queue);
    free(video_pkt_queue);
    free(audio_pkt_queue);
}

int MediaPlayer::start() {
    while(load_state != LOADING){
        
    }
    if(ffmpeg_object->has_audio()){
        audio_object->set_play_codec_ctx(ffmpeg_object->pAudioCodecCtx);
        audio_object->start_play();
    }
    if(ffmpeg_object->has_video()){
//        video_player->start_play();
    }
    return 0;
}

int MediaPlayer::set_path(char *path) {
    _path = path;
    return 0;
}

int MediaPlayer::load() {
    int ret;
    if((ret = ffmpeg_object->load(_path)) < 0){
        LOGD("load error ret = %d",ret);
        return ret;
    }
    load_state = LOADING;
    LOGE("path = %s",_path);
    ffmpeg_object->decode(video_pkt_queue,video_queue,audio_pkt_queue,audio_queue);
    return 0;
}

int MediaPlayer::set_surface(void *window, int width, int height) {
    if(width == height == 0){
        return -1;
    }
    return video_player->set_surface((ANativeWindow*)window);
}

int MediaPlayer::pause() {
    return 0;
}

int MediaPlayer::stop() {
    return 0;
}

int MediaPlayer::seek(int position) {
    return 0;
}

