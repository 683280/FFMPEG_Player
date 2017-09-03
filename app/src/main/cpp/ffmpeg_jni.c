//
// Created by carljay on 17-9-3.
//

#include <jni.h>
#include "ffmpeg_object.h"
#define JNIREG_CLASS "com/carljay/ffmpeg_project/ffmpeg/FFmpegObject"//指定要注册的类

static JNINativeMethod gMethods[] = {
        { "_start", "(I)Ljava/lang/Int;", (void*) start},
//        { "_setPath", "(S)Ljava/lang/Int;", (void*) setPath},
};

static int registerNativeMethods(JNIEnv* env, const char* className,
                                 JNINativeMethod* gMethods, int numMethods)
{
    jclass clazz;
    clazz = (*env)->FindClass(env, className);
    if (clazz == NULL) {
        return JNI_FALSE;
    }
    if ((*env)->RegisterNatives(env, clazz, gMethods, numMethods) < 0) {
        return JNI_FALSE;
    }

    return JNI_TRUE;
}
int registerNatives(JNIEnv* env){
    if (!registerNativeMethods(env, JNIREG_CLASS, gMethods,
                               sizeof(gMethods) / sizeof(gMethods[0])))
        return JNI_FALSE;

    return JNI_TRUE;
}
JNIEXPORT jint JNI_OnLoad(JavaVM* vm, void* reserved){
    JNIEnv* env = NULL;
    jint result = -1;

    if ((*vm)->GetEnv(vm, (void**) &env, JNI_VERSION_1_4) != JNI_OK) {
        return -1;
    }

    if (!registerNatives(env)) {//注册
        return -1;
    }
    /* success -- return valid version number */
    result = JNI_VERSION_1_4;

    return result;
}
JNIEXPORT void JNI_OnUnload(JavaVM* vm, void* reserved){

}