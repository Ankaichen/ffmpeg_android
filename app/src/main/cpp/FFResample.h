//
// Created by 27515 on 2025-05-20.
//

#ifndef FFMPEG_ANDROID_FFRESAMPLE_H
#define FFMPEG_ANDROID_FFRESAMPLE_H

#include "IResample.h"

#include <mutex>

class SwrContext;

class FFResample : public IResample {
public:
    bool Open(XParameter in, XParameter out) override;
    void Close() override;
    XData Resample(XData indata) override;


protected:
    SwrContext *actx;
    std::mutex mux;
};


#endif //FFMPEG_ANDROID_FFRESAMPLE_H
