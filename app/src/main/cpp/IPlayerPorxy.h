//
// Created by 27515 on 2025-05-21.
//

#ifndef FFMPEG_ANDROID_IPLAYERPORXY_H
#define FFMPEG_ANDROID_IPLAYERPORXY_H

#include "IPlayer.h"

#include <mutex>

class IPlayerPorxy : public IPlayer {
public:

    static IPlayerPorxy *Get();

    void Init(void *vm);

    bool Open(std::string_view path) override;

    void Close() override;

    bool Start() override;

    void InitView(void *win) override;

    double PlayPos() override;

    bool Seek(double pos) override;

    void SetPause(bool isP) override;

    bool IsPause() override;

protected:
    IPlayerPorxy() = default;

protected:
    IPlayer *player = nullptr;
    std::mutex mux;
};


#endif //FFMPEG_ANDROID_IPLAYERPORXY_H
