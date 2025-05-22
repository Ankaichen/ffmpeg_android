//
// Created by 27515 on 2025-05-17.
//

#include "FFDecode.h"
#include "XLog.h"

#include "jni.h"

extern "C" {
#include <libavcodec/jni.h>
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
}

bool FFDecode::Open(XParameter para, bool isHard) {
    this->Close();
    if (para.para == nullptr) return false;
    AVCodecParameters *p = para.para;
    AVCodec *cd = avcodec_find_decoder(p->codec_id);
    if (isHard) {
        cd = avcodec_find_decoder_by_name("h264_mediacodec");
    }
    if (cd == nullptr) {
        XLOGE("avcodec_find_decoder %d failed: %d", p->codec_id, isHard);
        return false;
    }
    XLOGI("avcodec_find_decoder %d success: %d", p->codec_id, isHard);
    std::lock_guard<std::mutex> guard(this->mux);
    // 创建解码器上下文 复制参数
    this->codec = avcodec_alloc_context3(cd);
    avcodec_parameters_to_context(this->codec, p);
    this->codec->thread_count = 8;
    // 打开解码器
    int re = avcodec_open2(this->codec, nullptr, nullptr);
    if (re != 0) {
        char buf[1024] = {0};
        av_strerror(re, buf, sizeof(buf) - 1);
        XLOGE("avcodec_open2 failed!\n%s", buf);
    }
    this->isAudio = (this->codec->codec_type == AVMEDIA_TYPE_AUDIO);
    XLOGI("avcodec_open2 success!");
    return true;
}

void FFDecode::Close() {
    this->Clear();
    std::lock_guard<std::mutex> guard(this->mux);
    this->pts = 0;
    if (this->frame) av_frame_free(&this->frame);
    if (this->codec) {
        avcodec_close(this->codec);
        avcodec_free_context(&this->codec);
    }
}

bool FFDecode::SendPacket(XData pkt) {
    if (pkt.size <= 0 || pkt.data == nullptr) return false;
    std::lock_guard<std::mutex> guard(this->mux);
    if (this->codec == nullptr) {
        return false;
    }
    int re = avcodec_send_packet(this->codec, reinterpret_cast<AVPacket *>(pkt.data));
    if (re != 0) {
        XLOGE("avcodec_send_packet failed!");
        return false;
    }
    return true;
}

XData FFDecode::RecvFrame() {
    XData d;
    std::lock_guard<std::mutex> guard(this->mux);
    if (this->codec == nullptr) return d;
    if (frame == nullptr) {
        frame = av_frame_alloc();
    }
    int re = avcodec_receive_frame(this->codec, frame);
    if (re != 0) {
        return d;
    }
    d.data = reinterpret_cast<unsigned char *>(frame);
    if (this->codec->codec_type == AVMEDIA_TYPE_VIDEO) {
        d.size = (frame->linesize[0] + frame->linesize[1] + frame->linesize[2]) * frame->height;
        d.width = frame->width;
        d.height = frame->height;
    } else if (this->codec->codec_type == AVMEDIA_TYPE_AUDIO) {
        // 样本字节数 * 单通道样本数 * 通道数
        d.size = av_get_bytes_per_sample(static_cast<AVSampleFormat>(frame->format)) * frame->nb_samples * 2;
    }
    d.format = frame->format;
    d.pts = frame->pts;
    this->pts = d.pts;
    memcpy(d.dates, frame->data, sizeof(d.dates));
    return d;
}

void FFDecode::InitHard(void *vm) {
    av_jni_set_java_vm(vm, nullptr);
}
