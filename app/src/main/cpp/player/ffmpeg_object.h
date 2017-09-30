//
// Created by 13342 on 2017/9/27.
//

#ifndef FFMPEG_PLAYER_FFMPEG_OBJECT_H
#define FFMPEG_PLAYER_FFMPEG_OBJECT_H


extern "C"{
#include <libavformat/avformat.h>
#include <my_queue.h>
}
char* get_ffmpeg_error(int error);
class FFmpegObject {
private:
    AVFormatContext* pFormatCtx;

    int video_stream_idx = -1,audio_stream_idx = -1;
    Queue *a_pkt_queue,*a_frame_queue,*v_pkt_queue,*v_frame_queue;
public:
    AVCodecContext *pVideoCodecCtx,*pAudioCodecCtx;
    int width,height;
    Queue* audio_queue,*video_queue;
public:
    FFmpegObject();
    ~FFmpegObject();
    int load(char* path);
    int decode(Queue *v_pkt_queue,Queue* v_frame_queue,Queue *a_pkt_queue,Queue* a_frame_queue);
    bool has_video();
    bool has_audio();
private:
    int open_codec_context(int* stream_idx,AVMediaType type,AVCodecContext** dec_ctx);
    int _decode(AVCodecContext *,Queue *pkt_queue,Queue* frame_queue);
    static void* t_load(void* context);
    static void* t_audio_decode(void* context);
    static void* t_video_decode(void* context);
};


#endif //FFMPEG_PLAYER_FFMPEG_OBJECT_H
