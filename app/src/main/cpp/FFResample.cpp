//
// Created by 27515 on 2025-05-20.
//

#include "FFResample.h"
#include "XLog.h"

extern "C" {
#include <libavformat/avformat.h>
#include <libswresample/swresample.h>
}

bool FFResample::Open(XParameter in, XParameter out) {
    this->Close();
    std::lock_guard<std::mutex> guard(this->mux);
    // 音频重采样上下文初始化
    this->actx = swr_alloc();
    this->actx = swr_alloc_set_opts(this->actx, av_get_default_channel_layout(out.para->channels),
                                    AV_SAMPLE_FMT_S16, out.para->sample_rate,
                                    av_get_default_channel_layout(in.para->channels),
                                    static_cast<AVSampleFormat>(in.para->format),
                                    in.para->sample_rate,
                                    0, nullptr);
    int re = swr_init(actx);
    if (re != 0) {
        XLOGE("swr_init failed!");
        return false;
    }
    XLOGI("swr_init success!");
    this->outChannels = in.para->channels;
    this->outFormat = AV_SAMPLE_FMT_S16;
    return true;
}

void FFResample::Close() {
    std::lock_guard<std::mutex> guard(this->mux);
    if (this->actx) swr_free(&this->actx);
}

XData FFResample::Resample(XData indata) {
    XData out;
    std::lock_guard<std::mutex> guard(this->mux);
    if (this->actx == nullptr || indata.size <= 0 || !indata.data) return out;
    auto *frame = reinterpret_cast<AVFrame *>(indata.data);
    // 通道数 * 单通道样本数 * 样本字节大小
    int outSize = this->outChannels * frame->nb_samples *
                  av_get_bytes_per_sample(static_cast<AVSampleFormat>(this->outFormat));
    if (outSize <= 0) return out;
    out.Alloc(outSize);
    uint8_t *outArr[2] = {0};
    outArr[0] = out.data;
    int len = swr_convert(this->actx, outArr, frame->nb_samples,
                          reinterpret_cast<const uint8_t **>(&frame->data), frame->nb_samples);
    if (len < 0) {
        out.Drop();
        XLOGE("swr_convert failed!");
        return out;
    }
    out.pts = indata.pts;
    return out;
}
