//
// Created by carljay on 17-9-3.
//

#include <ffmpeg_object.h>
#include <libavutil/file.h>

#define ffmpeg_object FFmpeg_Object* object = getFFmpegObject(env,obj);
FFmpeg_Object *getFFmpegObject(JNIEnv *env, jobject jobject1) {
    jclass* jclass1 = (*env)->GetObjectClass(env,jobject1);
    jfieldID id = (*env)->GetFieldID(env,jclass1,"jni_object","J");
    jlong instance = (*env)->GetLongField(env,jobject1,id);

    if(instance == NULL){
        instance = malloc(sizeof(FFmpeg_Object));
        (*env)->SetLongField(env,jobject1,id,instance);
        initFFmpegObject(instance);
    }
    return (FFmpeg_Object*)instance;
}
int initFFmpegObject(FFmpeg_Object* object){
    object->pFormatCtx = avformat_alloc_context();
}
struct buffer_data {
    uint8_t *ptr;
    size_t size; ///< size left in the buffer
};
//读回调函数opaque=调用者传递的参数，buf=目的地址（要把数据保存到哪里）
//buf_size=目的地址的长度，返回值表示读取多少字节的数据到目的地址
static int read_packet(void *opaque, uint8_t *buf, int buf_size)
{
    struct buffer_data *bd = (struct buffer_data *)opaque;
    buf_size = FFMIN(buf_size, bd->size);

    /* copy internal buffer data to buf */
    memcpy(buf, bd->ptr, buf_size);
    bd->ptr += buf_size;
    bd->size -= buf_size;

    return buf_size;
}
int start(JNIEnv *env,jobject *obj,int i) {
    ffmpeg_object
    load(env,obj);
    return 1 + i;
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
    object->video_stream_idx = av_find_best_stream(object->pFormatCtx, AVMEDIA_TYPE_VIDEO, -1, -1, NULL, 0);
    object->audio_stream_idx = av_find_best_stream(object->pFormatCtx, AVMEDIA_TYPE_AUDIO, -1, -1, NULL, 0);
    if(object->video_stream_idx < 0 && object->audio_stream_idx < 0){
        return -3;
    }
    auto a = avcodec_find_decoder(object->pFormatCtx->streams[object->video_stream_idx]->codecpar->codec_id);
    object->pVideoCodecCtx = avcodec_alloc_context3(a);
    return 0;
}

