//
// Created by carljay on 17-9-3.
//

#include <jni.h>
#include <media_player.h>
#include <media_record.h>
#include <android/native_window_jni.h>

#define MEDDIA_PLAYER_CLASS "com/carljay/ffmpeg_project/MediaPlayer"//指定要注册的类
#define MEDDIA_RECORD_CLASS "com/carljay/ffmpeg_project/MediaRecord"//指定要注册的类
#define GET_PLAYER auto object = getMediaPlayer(env,jobject1);
MediaPlayer *getMediaPlayer(JNIEnv *env, jobject jobject1) {
    jclass jclass1 = env->GetObjectClass(jobject1);
    jfieldID id = env->GetFieldID(jclass1, "jni_object", "J");
    jlong instance = env->GetLongField(jobject1, id);

    if (instance == -1) {
        auto player = new MediaPlayer();
        instance = (jlong) player;
        env->SetLongField(jobject1, id, instance);
    }
    return (MediaPlayer *) instance;
}
MediaRecord *getMediaRecord(JNIEnv *env, jobject jobject1) {
    jclass jclass1 = env->GetObjectClass(jobject1);
    jfieldID id = env->GetFieldID(jclass1, "jni_object", "J");
    jlong instance = env->GetLongField(jobject1, id);

    if (instance == NULL) {
        auto player = new MediaRecord();
        instance = (jlong) &player;
        env->SetLongField(jobject1, id, instance);
    }
    return (MediaRecord *) instance;
}
static JNINativeMethod record_methods[] = {
//        { "_start", "()I", (void*) start},
//        { "_setPath", "(Ljava/lang/String;)I", (void*) setPath},
//        { "_setSurface", "(Ljava/lang/Object;II)I", (void*) setSurface},
//        { "_load", "()I", (void*) load},
};
int player_start(JNIEnv *env, jobject jobject1){
    GET_PLAYER
    return object->start();
}
int player_load(JNIEnv *env, jobject jobject1){
    GET_PLAYER
    return object->load();
}
int player_pause(JNIEnv *env, jobject jobject1){
    GET_PLAYER
    return object->pause();
}
int player_stop(JNIEnv *env, jobject jobject1){
    GET_PLAYER
    return object->stop();
}
int player_set_surface(JNIEnv *env, jobject jobject1, jobject surface,jint width,jint height){
    GET_PLAYER
    auto window = ANativeWindow_fromSurface(env, surface);
    return object->set_surface(window,width,height);
}
int player_set_path(JNIEnv *env, jobject jobject1, jstring path){
    GET_PLAYER
    char* _path = (char *) env->GetStringUTFChars(path, 0);
    return object->set_path(_path);
}
int player_seek(JNIEnv *env, jobject jobject1,jint position){
    GET_PLAYER
    return object->seek(position);
}
static JNINativeMethod player_methods[] = {
        { "_start", "()I", (void*) player_start},
        { "_load", "()I", (void*) player_load},
        { "_pause", "()I", (void*) player_pause},
        { "_stop", "()I", (void*) player_stop},
        { "_setSurface", "(Ljava/lang/Object;II)I", (void*) player_set_surface},
        { "_setPath", "(Ljava/lang/String;)I", (void*) player_set_path},
        { "_seek", "(I)I", (void*) player_seek},
};


int init_ffmpeg(){
    av_register_all();
    avformat_network_init();
    return 0;
}
static int registerNativeMethods(JNIEnv* env, const char* className,
                                 JNINativeMethod* gMethods, int numMethods)
{
    jclass clazz;
    clazz = env->FindClass(className);
    if (clazz == NULL) {
        return JNI_FALSE;
    }
    if (env->RegisterNatives(clazz, gMethods, numMethods) < 0) {
        return JNI_FALSE;
    }

    return JNI_TRUE;
}
int registerNatives(JNIEnv* env){
    if (!registerNativeMethods(env, MEDDIA_PLAYER_CLASS, player_methods,
                               sizeof(player_methods) / sizeof(player_methods[0])))
        return JNI_FALSE;
    if (!registerNativeMethods(env, MEDDIA_RECORD_CLASS, record_methods,
                               sizeof(record_methods) / sizeof(record_methods[0])))
        return JNI_FALSE;

    return JNI_TRUE;
}
JNIEXPORT jint JNI_OnLoad(JavaVM* vm, void* reserved){
    JNIEnv* env = NULL;
    jint result = -1;

    if (vm->GetEnv((void**) &env, JNI_VERSION_1_4) != JNI_OK) {
        return -1;
    }

    if (!registerNatives(env)) {//注册
        return -1;
    }
    /* success -- return valid version number */
    result = JNI_VERSION_1_6;
    init_ffmpeg();
    create_opensl_engine();
    return result;
}
JNIEXPORT void JNI_OnUnload(JavaVM* vm, void* reserved){
    destory_opensl_engine();
}