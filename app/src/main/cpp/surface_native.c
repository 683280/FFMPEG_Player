//
// Created by 13342 on 2017/9/4.
//


#include <surface_native.h>
#include <libavutil/imgutils.h>
#include <head.h>

int init_video_sws(FFmpeg_Object* object){
    int width = object->pSurfaceNative->width;
    int height = object->pSurfaceNative->height;
    object->pSurfaceNative->pSwsFrame = av_frame_alloc();
    object->pSurfaceNative->pVideoSwsCtx = sws_getContext(object->pVideoCodecCtx->width,object->pVideoCodecCtx->height,object->pVideoCodecCtx->pix_fmt,width,height,AV_PIX_FMT_RGBA,SWS_BILINEAR, NULL, NULL, NULL);
    uint8_t *buffer = (uint8_t *)av_malloc(
            av_image_get_buffer_size(AV_PIX_FMT_RGBA, width, height, 1) * sizeof(uint8_t));
    av_image_fill_arrays(object->pSurfaceNative->pSwsFrame->data, object->pSurfaceNative->pSwsFrame->linesize, buffer, AV_PIX_FMT_RGBA, width,height, 1);
    return 0;
}
int postFrame(FFmpeg_Object* object) {
    SurfaceNative* surfaceNative = object->pSurfaceNative;
    AVFrame* rsc = object->pVideoFrame;
    //转换成RGBA格式的图片
    sws_scale(surfaceNative->pVideoSwsCtx, (const uint8_t *const *)rsc->data, rsc->linesize, 0,
              rsc->height, surfaceNative->pSwsFrame->data,surfaceNative->pSwsFrame->linesize);

    ANativeWindow_lock(surfaceNative->pWindow, &surfaceNative->windowBuffer, 0);
    uint8_t* dst_data = (uint8_t*) surfaceNative->windowBuffer.bits;
    int dst_size = surfaceNative->windowBuffer.stride * 4;

    uint8_t* rsc_data = surfaceNative->pSwsFrame->data[0];
    int rsc_size = surfaceNative->pSwsFrame->linesize[0];

    for (int i = 0;i < surfaceNative->height;i++){
        memcpy(dst_data + i * dst_size,   rsc_data + i * rsc_size,rsc_size * 4);
    }
    ANativeWindow_unlockAndPost(surfaceNative->pWindow);
    return 0;
}

int initSurfaceNative(FFmpeg_Object* object) {
    SurfaceNative* surfaceNative = object->pSurfaceNative;
    int ret = ANativeWindow_setBuffersGeometry(surfaceNative->pWindow, surfaceNative->width, surfaceNative->height, WINDOW_FORMAT_RGBA_8888);
    init_video_sws(object);
    return ret;
}
