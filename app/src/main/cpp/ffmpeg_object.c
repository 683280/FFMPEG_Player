//
// Created by carljay on 17-9-3.
//

#include <ffmpeg_object.h>
#include <libavutil/file.h>
#include <Log.h>
#include <libavutil/imgutils.h>
#include <head.h>
#include <libswscale/swscale.h>
#include <opensl_native.h>
#include <my_queue.h>
#include <pthread.h>

#define ffmpeg_object FFmpeg_Object* object = getFFmpegObject(env,obj);

FFmpeg_Object *getFFmpegObject(JNIEnv *env, jobject jobject1) {
    jclass *jclass1 = (*env)->GetObjectClass(env, jobject1);
    jfieldID id = (*env)->GetFieldID(env, jclass1, "jni_object", "J");
    jlong instance = (*env)->GetLongField(env, jobject1, id);

    if (instance == NULL) {
        instance = calloc(1, sizeof(FFmpeg_Object));
        (*env)->SetLongField(env, jobject1, id, instance);
        initFFmpegObject((FFmpeg_Object *) instance);
    }
    return (FFmpeg_Object *) instance;
}

int initFFmpegObject(FFmpeg_Object *object) {
    object->pFormatCtx = avformat_alloc_context();
    object->audio_stream_idx = -1;
    object->video_stream_idx = -1;

    return 0;
}

static int open_codec_context(int *stream_idx,
                              AVCodecContext **dec_ctx, AVFormatContext *fmt_ctx,
                              enum AVMediaType type) {
    int ret, stream_index;
    AVStream *st;
    AVCodec *dec = NULL;
    ret = av_find_best_stream(fmt_ctx, type, -1, -1, NULL, 0);
    if (ret < 0) {
        LOGE("Could not find %s stream in input file \n",
             av_get_media_type_string(type));
        return ret;
    } else {
        stream_index = ret;
        st = fmt_ctx->streams[stream_index];
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

void get_audio_data(FFmpeg_Object *object) {
    decode(object->pAudioCodecCtx, object->pAudioQueue, &object->pAudioFrame);
    OpenSLES* openSLES = object->pOpenSLES;
    AVCodecContext* pCodecCtx = object->pAudioCodecCtx;
    AVFrame* aFrame = object->pAudioFrame;

    if (pCodecCtx->sample_fmt == AV_SAMPLE_FMT_S16P) {
        openSLES->data_size = av_samples_get_buffer_size(aFrame->linesize, pCodecCtx->channels,pCodecCtx->frame_size,pCodecCtx->sample_fmt, 1);
    }else {
        av_samples_get_buffer_size(&openSLES->data_size, pCodecCtx->channels,pCodecCtx->frame_size,pCodecCtx->sample_fmt, 1);
    }
    // 这里内存再分配可能存在问题
    if (openSLES->data_size > openSLES->sl_buffer_size) {
        openSLES->sl_buffer_size = openSLES->data_size;
        openSLES->sl_buffer = (uint8_t *) realloc(openSLES->sl_buffer,
                                                  sizeof(uint8_t) * openSLES->sl_buffer_size);
    }
    // 音频格式转换
    swr_convert(openSLES->pSwrCtx, &openSLES->sl_buffer,openSLES->data_size,
                (uint8_t const **) (aFrame->extended_data),
                aFrame->nb_samples);
}

int start(JNIEnv *env, jobject *obj) {
    ffmpeg_object
    while (object->state == OBJECT_STATE_UNINIT) {

    }
    //播放声言
    play(object->pOpenSLES);
    //解码视频
    pthread_t video;
    pthread_create(&video, 0, t_decodec_video, object);
//    pthread_detach(video);
    //播放视频
//    play_video(object);
    return 0;
}

int setPath(JNIEnv *env, jobject *obj, jstring path) {
    ffmpeg_object
    const char *c = (*env)->GetStringUTFChars(env, path, 0);
    object->path = c;
    return 0;
}

int pause(JNIEnv *env, jobject *obj) {
    return 0;
}

int stop(JNIEnv *env, jobject *obj) {
    return 0;
}

int load(JNI_PARAMETER) {
    ffmpeg_object
    pthread_t t;
    pthread_create(&t, 0, t_load, object);
    pthread_detach(t);

    return 0;
}

int seek(JNIEnv *env, jobject *obj, int i) {
    ffmpeg_object
    return 0;
}

int setSurface(JNIEnv *env, jobject *obj, jobject *surface, int width, int height) {
    ffmpeg_object
    if (object->pSurfaceNative == NULL) {
        object->pSurfaceNative = malloc(sizeof(SurfaceNative));
    }
    object->pSurfaceNative->width = width;
    object->pSurfaceNative->height = height;
    object->pSurfaceNative->pWindow = ANativeWindow_fromSurface(env, surface);
    if (object->pSurfaceNative->pWindow == NULL) {
        return -1;
    }
    return 0;
}

void t_load(FFmpeg_Object *object) {
    int ret;
    if (object->path == NULL) {
//        return -1;
    }
    if ((ret = avformat_open_input(&object->pFormatCtx, object->path, NULL, NULL)) != 0) {
        LOGE("avformat_open_input error  = %d",ret);
        return ;
    }
    if ((ret = avformat_find_stream_info(object->pFormatCtx, NULL)) < 0) {
        LOGE("avformat_find_stream_info error = %d",ret);
        return;
    }
    if (open_codec_context(&object->video_stream_idx, &object->pVideoCodecCtx, object->pFormatCtx,
                           AVMEDIA_TYPE_VIDEO) == 0) {
        initSurfaceNative(object);
        object->pVideoQueue->max = 1000;
        object->pVideoFrame = av_frame_alloc();
    }
    if (open_codec_context(&object->audio_stream_idx, &object->pAudioCodecCtx, object->pFormatCtx,
                           AVMEDIA_TYPE_AUDIO) == 0) {
        object->pAudioFrame = av_frame_alloc();
        create_opensl(object);
        create_opensl_player(object);
        object->pAudioQueue->max = 1000;
        object->pOpenSLES->get_pcm = get_audio_data;
    }
    if (object->audio_stream_idx == -1 && object->video_stream_idx == -1) {
//        return -4;
        LOGE("not find audio and video stream error = %d",ret);
        return ;
    }
    object->state = OBJECT_STATE_INIT;
    LOGD("width = %d", object->pVideoCodecCtx->width);
    LOGD("height = %d", object->pVideoCodecCtx->height);
    AVPacket pkt;
    av_init_packet(&pkt);
    while ((ret = av_read_frame(object->pFormatCtx, &pkt)) >= 0) {
        AVPacket *p = av_packet_clone(&pkt);
        if (pkt.stream_index == object->audio_stream_idx) {
            post_packet(object->pAudioQueue, p);
        } else if (pkt.stream_index == object->video_stream_idx) {
            post_packet(object->pVideoQueue, p);
        }
    }
    object->state = OBJECT_STATE_END;
    LOGE("play end %d",ret);
}

void t_decodec_video(FFmpeg_Object *object) {
    while (1) {
        decode(object->pVideoCodecCtx, object->pVideoQueue, &object->pVideoFrame);
        postFrame(object);
    }
}

void t_decodec_audio(FFmpeg_Object *object) {
    decode(object->pAudioCodecCtx, object->pAudioQueue, &object->pAudioFrame);
}

