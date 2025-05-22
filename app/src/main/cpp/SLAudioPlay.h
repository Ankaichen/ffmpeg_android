//
// Created by 27515 on 2025-05-20.
//

#ifndef FFMPEG_ANDROID_SLAUDIOPLAY_H
#define FFMPEG_ANDROID_SLAUDIOPLAY_H

#include "IAudioPlay.h"

#include <mutex>

class SLAudioPlay : public IAudioPlay {
public:
    SLAudioPlay();
    ~SLAudioPlay() noexcept override;

    bool StartPlay(XParameter out) override;
    void PlayCall(const void *const bufq);

    void Close() override;

protected:
    unsigned char *buf = nullptr;
    std::mutex mux;
};


#endif //FFMPEG_ANDROID_SLAUDIOPLAY_H
