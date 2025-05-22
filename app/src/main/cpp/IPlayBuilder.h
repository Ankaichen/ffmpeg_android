//
// Created by 27515 on 2025-05-21.
//

#ifndef FFMPEG_ANDROID_IPLAYBUILDER_H
#define FFMPEG_ANDROID_IPLAYBUILDER_H

#include "IPlayer.h"

class IPlayBuilder {
public:

    virtual ~IPlayBuilder() noexcept = default;

    virtual IPlayer *BuildPlayer(unsigned char index);

protected:
    virtual IDemux *CreateDemux() = 0;

    virtual IDecode *CreateDecode() = 0;

    virtual IResample *CreateResample() = 0;

    virtual IVideoView *CreateVideoView() = 0;

    virtual IAudioPlay *CreateAudioPlay() = 0;

    virtual IPlayer *CreatePlayer(unsigned char index) = 0;
};


#endif //FFMPEG_ANDROID_IPLAYBUILDER_H
