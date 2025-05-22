//
// Created by 27515 on 2025-05-20.
//

#ifndef FFMPEG_ANDROID_IRESAMPLE_H
#define FFMPEG_ANDROID_IRESAMPLE_H

#include "IObserver.h"
#include "XParameter.h"

class IResample : public IObserver {
public:
    virtual bool Open(XParameter in, XParameter out) = 0;
    virtual void Close() = 0;
    virtual XData Resample(XData indata) = 0;
    void Update(XData data) override;

protected:
    int outChannels = 2;
    int outFormat = 1;
};


#endif //FFMPEG_ANDROID_IRESAMPLE_H
