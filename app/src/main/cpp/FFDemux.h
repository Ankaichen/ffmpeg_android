//
// Created by 27515 on 2025-05-17.
//

#ifndef FFMPEG_ANDROID_FFDEMUX_H
#define FFMPEG_ANDROID_FFDEMUX_H

#include "IDemux.h"

#include <mutex>

class AVFormatContext;

class FFDemux : public IDemux {
public:

    FFDemux();

    bool Open(std::string_view url) override;

    bool Seek(double pos) override;

    void Close() override;

    XParameter GetVPara() override;

    XParameter GetAPara() override;

    XData Read() override;

private:
    AVFormatContext *ic{nullptr};
    int audioStream = 1, videoStream = 0;
    std::mutex mux;
};


#endif //FFMPEG_ANDROID_FFDEMUX_H
