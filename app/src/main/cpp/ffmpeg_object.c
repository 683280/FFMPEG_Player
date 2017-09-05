//
// Created by carljay on 17-9-3.
//

#include <ffmpeg_object.h>
#include <libavutil/file.h>
#include <Log.h>
#include <libavutil/imgutils.h>
#include <head.h>
#include <libswscale/swscale.h>

#define ffmpeg_object FFmpeg_Object* object = getFFmpegObject(env,obj);
FFmpeg_Object *getFFmpegObject(JNIEnv *env, jobject jobject1) {
    jclass* jclass1 = (*env)->GetObjectClass(env,jobject1);
    jfieldID id = (*env)->GetFieldID(env,jclass1,"jni_object","J");
    jlong instance = (*env)->GetLongField(env,jobject1,id);

    if(instance == NULL){
        instance = calloc(1,sizeof(FFmpeg_Object));
        (*env)->SetLongField(env,jobject1,id,instance);
        initFFmpegObject((FFmpeg_Object*)instance);
    }
    return (FFmpeg_Object*)instance;
}
int initFFmpegObject(FFmpeg_Object* object){
    object->pFormatCtx = avformat_alloc_context();
    object->audio_stream_idx = -1;
    object->video_stream_idx = -1;

    return 0;
}
static int open_codec_context(int *stream_idx,
                              AVCodecContext **dec_ctx, AVFormatContext *fmt_ctx, enum AVMediaType type)
{
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
            LOGE( "Failed to find %s codec\n",
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
            LOGE( "Failed to copy %s codec parameters to decoder context\n",
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

static int decode_packet(FFmpeg_Object *object, AVPacket *pPacket) {
    int ret = 0;
    int decoded = pPacket->size;
    if (pPacket->stream_index == object->video_stream_idx) {
        /* decode video frame */
        if ((ret = avcodec_send_packet(object->pVideoCodecCtx, pPacket)) != 0) {
            LOGE("avcodec_send_packet error = %d", ret);
        }
        do {
            ret = avcodec_receive_frame(object->pVideoCodecCtx, object->pVideoFrame);
            if (ret < 0) {
                av_frame_unref(object->pVideoFrame);
                return ret;
            }
//            AVERROR(EAGAIN)
            postFrame(object);
            av_frame_unref(object->pVideoFrame);
            av_packet_unref(pPacket);
        }while(1);

    }else if (pPacket->stream_index == object->audio_stream_idx) {

    }
    return decoded;
}

int start(JNIEnv *env,jobject *obj) {
    ffmpeg_object
    int result;
    if((result = load(env,obj)) < 0){
        return result;
    }
    return 0;
}

int setPath(JNIEnv *env,jobject *obj,jstring path) {
    ffmpeg_object
    const char* c = (*env)->GetStringUTFChars(env,path,0);
    object->path = c;
    return 0;
}

int pause(JNIEnv *env,jobject *obj) {
    return 0;
}

int stop(JNIEnv *env,jobject *obj) {
    return 0;
}

int load(JNIEnv *env,jobject *obj) {
    ffmpeg_object
    if(object->path == NULL){
        return -1;
    }
    if(avformat_open_input(&object->pFormatCtx,object->path,NULL,NULL)){
        return -1;
    }
    if(avformat_find_stream_info(object->pFormatCtx,NULL) < 0){
        return -2;
    }
    if(open_codec_context(&object->video_stream_idx,&object->pVideoCodecCtx,object->pFormatCtx,AVMEDIA_TYPE_VIDEO) == 0){
//        object->pSurfaceNative->width = object->pVideoCodecCtx->width;
//        object->pSurfaceNative->height = object->pVideoCodecCtx->height;
        initSurfaceNative(object);
        object->pVideoFrame = av_frame_alloc();
//        LOGD("没有视频数据");
    }
    if(open_codec_context(&object->audio_stream_idx,&object->pAudioCodecCtx,object->pFormatCtx,AVMEDIA_TYPE_AUDIO) != 0){
        LOGD("没有音频数据");
        object->pAudioFrame = av_frame_alloc();
    }
    if (object->audio_stream_idx == -1 && object->video_stream_idx == -1){
        return -4;
    }
    LOGD("width = %d",object->pVideoCodecCtx->width);
    LOGD("height = %d",object->pVideoCodecCtx->height);
    AVPacket pkt;
    av_init_packet(&pkt);
    while (av_read_frame(object->pFormatCtx, &pkt) >= 0) {
        int ret = decode_packet(object, &pkt);
        if(ret == 0){
            continue;
        }else if (ret == AVERROR(EAGAIN)){//数据太少不能解出一帧，重新读取
            continue;
        } else if(ret == AVERROR(EINVAL)){//不支持数据，错误，退出
            av_packet_unref(&pkt);
            return -1;
        }
    }
    av_free(&pkt);
    LOGE("play end");
    return 0;
}

int seek(JNIEnv *env, jobject *obj, int i) {
    ffmpeg_object
    return 0;
}

int setSurface(JNIEnv *env, jobject *obj, jobject* surface, int width, int height) {
    ffmpeg_object
    if(object->pSurfaceNative == NULL){
        object->pSurfaceNative = malloc(sizeof(SurfaceNative));
    }
    object->pSurfaceNative->width = width;
    object->pSurfaceNative->height = height;
    object->pSurfaceNative->pWindow = ANativeWindow_fromSurface(env, surface);
    if( object->pSurfaceNative->pWindow == NULL){
        return -1;
    }
    return 0;
}

