//
// Created by 27515 on 2025-05-21.
//

#ifndef FFMPEG_ANDROID_FFPLAYBUILDER_H
#define FFMPEG_ANDROID_FFPLAYBUILDER_H

#include "IPlayBuilder.h"

class FFPlayBuilder : public IPlayBuilder {
public:
    static FFPlayBuilder *Get();

    static void InitHard(void *vm);

    ~FFPlayBuilder() noexcept override = default;

protected:
    FFPlayBuilder() = default;

    IDemux *CreateDemux() override;

    IDecode *CreateDecode() override;

    IResample *CreateResample() override;

    IAudioPlay *CreateAudioPlay() override;

    IVideoView *CreateVideoView() override;

    IPlayer *CreatePlayer(unsigned char index) override;
};


#endif //FFMPEG_ANDROID_FFPLAYBUILDER_H
