//
// Created by 13342 on 2017/9/27.
//

#include <Log.h>
#include <pthread.h>
#include "ffmpeg_object.h"

FFmpegObject::FFmpegObject() {
    pFormatCtx = avformat_alloc_context();
    if(!pFormatCtx){
        LOGE("avformat_alloc_context error");
    }
}

FFmpegObject::~FFmpegObject() {
    avformat_free_context(pFormatCtx);
}

int FFmpegObject::load(char *path) {
    int ret = avformat_open_input(&pFormatCtx, path, NULL, NULL);
    if (ret != 0) {
        LOGE("avformat_open_input error ret = %d", ret);
        LOGE("%s",get_ffmpeg_error(ret));
        return ret;
    }
    ret = avformat_find_stream_info(pFormatCtx, NULL);
    if (ret != 0) {
        LOGE("avformat_find_stream_info error ret = %d", ret);
        return ret;
    }
    if ((ret = open_codec_context(&video_stream_idx, AVMEDIA_TYPE_VIDEO, &pVideoCodecCtx)) == 0) {
        width = pVideoCodecCtx->width;
        height = pVideoCodecCtx->height;
    }
    if ((ret = open_codec_context(&audio_stream_idx, AVMEDIA_TYPE_AUDIO, &pAudioCodecCtx)) == 0) {

    }
    if (video_stream_idx == -1 && audio_stream_idx == -1) {
        LOGE("error : not find audio or video");
        return -1;
    }
    pthread_t p_load;
    pthread_create(&p_load,0,t_load,this);
    return ret;
}

void *FFmpegObject::t_load(void *context) {
    FFmpegObject* _this = (FFmpegObject *) context;
    AVPacket pkt;
    av_init_packet(&pkt);
    int ret;
    while ((ret = av_read_frame(_this->pFormatCtx, &pkt)) >= 0) {
        AVPacket *p = av_packet_clone(&pkt);
        if (pkt.stream_index == _this->audio_stream_idx) {
            post_packet(_this->audio_queue, p);
        } else if (pkt.stream_index == _this->video_stream_idx) {
//            post_packet(_this->video_queue, p);
        }
    }
    return NULL;
}
int FFmpegObject::open_codec_context(int *stream_idx, AVMediaType type, AVCodecContext **dec_ctx) {
    int ret, stream_index;
    AVStream *st;
    AVCodec *dec = NULL;
    ret = av_find_best_stream(pFormatCtx, type, -1, -1, NULL, 0);
    if (ret < 0) {
        LOGE("Could not find %s stream in input file \n",
             av_get_media_type_string(type));
        return ret;
    } else {
        stream_index = ret;
        st = pFormatCtx->streams[stream_index];
        /* find decoder for the stream */
        dec = avcodec_find_decoder(st->codecpar->codec_id);
        if (!dec) {
            LOGE("Failed to find %s codec\n",
                 av_get_media_type_string(type));
            return AVERROR(EINVAL);
        }
        /* Allocate a codec context for the decoder */
        *dec_ctx = avcodec_alloc_context3(dec);
        if (!*dec_ctx) {
            LOGE("Failed to allocate the %s codec context\n",
                 av_get_media_type_string(type));
            return AVERROR(ENOMEM);
        }
        /* Copy codec parameters from input stream to output codec context */
        if ((ret = avcodec_parameters_to_context(*dec_ctx, st->codecpar)) < 0) {
            LOGE("Failed to copy %s codec parameters to decoder context\n",
                 av_get_media_type_string(type));
            return ret;
        }
        /* Init the decoders, with or without reference counting */
        if ((ret = avcodec_open2(*dec_ctx, dec, NULL)) < 0) {
            fprintf(stderr, "Failed to open %s codec\n",
                    av_get_media_type_string(type));
            return ret;
        }
        *stream_idx = stream_index;
    }
    return 0;
}

int FFmpegObject::decode(Queue *v_pkt_queue,Queue* v_frame_queue,Queue *a_pkt_queue,Queue* a_frame_queue) {
    this->v_frame_queue = v_frame_queue;
    this->v_pkt_queue = v_pkt_queue;
    this->a_frame_queue = a_frame_queue;
    this->a_pkt_queue = a_pkt_queue;

    pthread_t v_decode,a_decode;

    pthread_create(&v_decode,0,t_video_decode,this);
    pthread_create(&a_decode,0,t_audio_decode,this);
    return 0;
}

void *FFmpegObject::t_audio_decode(void *context) {
    FFmpegObject* _this = (FFmpegObject *) context;
    if(_this->pAudioCodecCtx) {
        _this->_decode(_this->pAudioCodecCtx, _this->a_pkt_queue, _this->a_frame_queue);
    }
    return NULL;
}
void *FFmpegObject::t_video_decode(void *context) {
    FFmpegObject* _this = (FFmpegObject *) context;
    if(_this->pVideoCodecCtx){
        _this->_decode(_this->pVideoCodecCtx,_this->v_pkt_queue,_this->v_frame_queue);
    }
    return NULL;
}

int FFmpegObject::_decode(AVCodecContext *codecCtx, Queue *pkt_queue, Queue *frame_queue) {
    AVFrame *frame = av_frame_alloc();
    AVPacket *pkt;
    int ret = 0;
    while (1) {
        while (1) {
            ret = avcodec_receive_frame(codecCtx, frame);
            if (ret == AVERROR(EAGAIN)) {//数据太少不能解出一帧，重新读取
                break;
            } else if (ret == AVERROR(EINVAL)) {//不支持数据，错误，退出
                return NULL;
            }
            post_packet(frame_queue,frame);
            av_packet_unref(pkt);
            frame = av_frame_alloc();
        }
        pkt = (AVPacket *) get_packet(pkt_queue);
        if ((ret = avcodec_send_packet(codecCtx, pkt)) != 0) {
            LOGE("avcodec_send_packet error = %d", ret);
            return NULL;
        }
    }
    return 0;
}

bool FFmpegObject::has_video() {
    if(pVideoCodecCtx != NULL){
        return true;
    }
    return false;
}

bool FFmpegObject::has_audio() {
    if(pAudioCodecCtx != NULL){
        return true;
    }
    return false;
}

char *get_ffmpeg_error(int error) {
    switch (error){
        case AVERROR_BSF_NOT_FOUND:
            return "AVERROR_BSF_NOT_FOUND";
        case AVERROR_BUG:
            return "AVERROR_BUG";
        case AVERROR_BUFFER_TOO_SMALL:
            return "AVERROR_BUFFER_TOO_SMALL";
        case AVERROR_DECODER_NOT_FOUND:
            return "AVERROR_DECODER_NOT_FOUND";
        case AVERROR_DEMUXER_NOT_FOUND:
            return "AVERROR_DEMUXER_NOT_FOUND";
        case AVERROR_ENCODER_NOT_FOUND:
            return "AVERROR_ENCODER_NOT_FOUND";
        case AVERROR_EOF:
            return "AVERROR_EOF";
        case AVERROR_EXIT:
            return "AVERROR_EXIT";
        case AVERROR_EXTERNAL:
            return "AVERROR_EXTERNAL";
        case AVERROR_FILTER_NOT_FOUND:
            return "AVERROR_FILTER_NOT_FOUND";
        case AVERROR_INVALIDDATA:
            return "AVERROR_INVALIDDATA";
        case AVERROR_MUXER_NOT_FOUND:
            return "AVERROR_MUXER_NOT_FOUND";
        case AVERROR_OPTION_NOT_FOUND:
            return "AVERROR_OPTION_NOT_FOUND";
        case AVERROR_PATCHWELCOME:
            return "AVERROR_PATCHWELCOME";
        case AVERROR_PROTOCOL_NOT_FOUND:
            return "AVERROR_PROTOCOL_NOT_FOUND";
        case AVERROR_STREAM_NOT_FOUND:
            return "AVERROR_STREAM_NOT_FOUND";
        case AVERROR_BUG2:
            return "AVERROR_BUG2";
        case AVERROR_UNKNOWN:
            return "AVERROR_UNKNOWN";
        case AVERROR_EXPERIMENTAL:
            return "AVERROR_EXPERIMENTAL";
        case AVERROR_INPUT_CHANGED:
            return "AVERROR_INPUT_CHANGED";
        case AVERROR_OUTPUT_CHANGED:
            return "AVERROR_OUTPUT_CHANGED";
        case AVERROR_HTTP_BAD_REQUEST:
            return "AVERROR_HTTP_BAD_REQUEST";
        case AVERROR_HTTP_UNAUTHORIZED:
            return "AVERROR_HTTP_UNAUTHORIZED";
        case AVERROR_HTTP_FORBIDDEN:
            return "AVERROR_HTTP_FORBIDDEN";
        case AVERROR_HTTP_NOT_FOUND:
            return "AVERROR_HTTP_NOT_FOUND";
        case AVERROR_HTTP_OTHER_4XX:
            return "AVERROR_HTTP_NOT_FOUND";
        case AVERROR_HTTP_SERVER_ERROR:
            return "AVERROR_HTTP_SERVER_ERROR";
    }
    return "not find error";
}
