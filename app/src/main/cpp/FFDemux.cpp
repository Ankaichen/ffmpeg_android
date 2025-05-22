//
// Created by 27515 on 2025-05-17.
//

#include "FFDemux.h"

#include "./XLog.h"

extern "C" {
#include <libavformat/avformat.h>
}

static double r2d(AVRational r) {
    if (r.num == 0 || r.den == 0) return 0.;
    return static_cast<double>(r.num) / static_cast<double>(r.den);
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
    this->Close();
    std::unique_lock<std::mutex> guard(this->mux);
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
    guard.unlock();
    this->GetVPara();
    this->GetAPara();
    return true;
}

void FFDemux::Close() {
    std::lock_guard<std::mutex> guard(this->mux);
    if (this->ic)
        avformat_close_input(&this->ic);
}

XParameter FFDemux::GetVPara() {
    std::lock_guard<std::mutex> guard(this->mux);
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
    std::lock_guard<std::mutex> guard(this->mux);
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
    para.channels = ic->streams[re]->codecpar->channels;
    para.sample_rate = ic->streams[re]->codecpar->sample_rate;
    return para;
}

XData FFDemux::Read() {
    std::lock_guard<std::mutex> guard(this->mux);
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
    // 转换pts
    pkt->pts = pkt->pts * 1000 * r2d(ic->streams[pkt->stream_index]->time_base);
    pkt->dts = pkt->dts * 1000 * r2d(ic->streams[pkt->stream_index]->time_base);
    d.pts = static_cast<long long>(pkt->pts);
    return d;
}

bool FFDemux::Seek(double pos) {
    bool re = false;
    if (pos < 0 || pos > 1) {
        XLOGE("Seek value must be 0.0 - 1.0");
        return re;
    }
    std::lock_guard<std::mutex> guard(this->mux);
    if (this->ic == nullptr) return re;
    // 清除读取的缓冲
    avformat_flush(this->ic);
    long long seekPts = 0;
    seekPts = this->ic->streams[this->videoStream]->duration * pos;
    // 往后跳转到关键帧
    re = av_seek_frame(this->ic, this->videoStream, seekPts,
                       AVSEEK_FLAG_FRAME | AVSEEK_FLAG_BACKWARD);
    return re;
}
