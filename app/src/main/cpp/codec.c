//
// Created by 13342 on 2017/9/11.
//

#include <asm/errno.h>
#include <libavutil/error.h>
#include <libavcodec/avcodec.h>
#include <my_queue.h>
#include <Log.h>
#include "codec.h"

int decode(AVCodecContext *codecCtx, Queue *queue, AVFrame **f) {
    AVFrame* frame = (*f);
    AVPacket *pkt;
    int ret = 0;
    while (1) {
        while(1) {
            ret = avcodec_receive_frame(codecCtx, frame);
            if (ret == AVERROR(EAGAIN)) {//数据太少不能解出一帧，重新读取
                break;
            } else if (ret == AVERROR(EINVAL)) {//不支持数据，错误，退出
                return -1;
            }
            (*f) = frame;
            av_packet_unref(pkt);
            return 0;
        }
        pkt = get_packet(queue);
        if ((ret = avcodec_send_packet(codecCtx, pkt)) != 0) {
            LOGE("avcodec_send_packet error = %d", ret);
            return ret;
        }
    }
}

