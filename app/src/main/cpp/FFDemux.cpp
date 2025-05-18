//
// Created by 27515 on 2025-05-17.
//

#include "FFDemux.h"

#include "./XLog.h"

extern "C" {
#include <libavformat/avformat.h>
}

FFDemux::FFDemux() {
    static bool isFirst = true;
    if (isFirst) {
        isFirst = false;
        // 注册所有封装器
        av_register_all();
        // 注册所有的解码器
        avcodec_register_all();
        // 初始化网络
        avformat_network_init();
        XLOGI("register ffmpeg!");
    }
}

bool FFDemux::Open(std::string_view url) {
    XLOGI("Open file %s begin", url.data());
    int re = avformat_open_input(&(this->ic), url.data(), nullptr, nullptr);
    if (re != 0) {
        char buf[1024] = {0};
        av_strerror(re, buf, sizeof(buf));
        XLOGE("FFDemux open %s failed!\n%s", url.data(), buf);
        return false;
    }
    XLOGI("FFDemux open %s success!", url.data());
    // 读取文件信息
    re = avformat_find_stream_info(this->ic, nullptr);
    if (re != 0) {
        char buf[1024] = {0};
        av_strerror(re, buf, sizeof(buf));
        XLOGE("avformat_find_stream_info %s failed!\n%s", url.data(), buf);
        return false;
    }
    this->totalMs = static_cast<int>(this->ic->duration / (AV_TIME_BASE / 1000));
    XLOGI("total ms = %d", this->totalMs);
    this->GetVPara();
    this->GetAPara();
    return true;
}

XParameter FFDemux::GetVPara() {
    XParameter para{};
    if (this->ic == nullptr) {
        XLOGE("GetVPara failed, ic is nullptr!");
        return para;
    }
    // 获取视频流索引
    int re = av_find_best_stream(this->ic, AVMEDIA_TYPE_VIDEO, -1, -1, nullptr, -1);
    if (re < 0) {
        XLOGE("av_find_best_stream failed!");
        return para;
    }
    this->videoStream = re;
    para.para = ic->streams[re]->codecpar;
    return para;
}

XParameter FFDemux::GetAPara() {
    XParameter para{};
    if (this->ic == nullptr) {
        XLOGE("GetAPara failed, ic is nullptr!");
        return para;
    }
    // 获取音频流索引
    int re = av_find_best_stream(this->ic, AVMEDIA_TYPE_AUDIO, -1, -1, nullptr, -1);
    if (re < 0) {
        XLOGE("av_find_best_stream failed!");
        return para;
    }
    this->audioStream = re;
    para.para = ic->streams[re]->codecpar;
    return para;
}

XData FFDemux::Read() {
    if (!this->ic) return {};
    XData d;
    AVPacket *pkt = av_packet_alloc();
    int re = av_read_frame(this->ic, pkt);
    if (re != 0) {
        av_packet_free(&pkt);
        return d;
    }
//    XLOGI("pack size is %d, pts is %lld", pkt->size, pkt->pts);
    if (pkt->stream_index == this->audioStream) {
        d.isAudio = true;
    } else if (pkt->stream_index == this->videoStream) {
        d.isAudio = false;
    } else {
        av_packet_free(&pkt);
        return d;
    }
    d.data = reinterpret_cast<unsigned char *>(pkt);
    d.size = pkt->size;
    return d;
}
