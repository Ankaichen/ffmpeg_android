//
// Created by 27515 on 2025-05-21.
//

#ifndef FFMPEG_ANDROID_IPLAYER_H
#define FFMPEG_ANDROID_IPLAYER_H

#include "XThread.h"
#include "XParameter.h"

#include <string_view>
#include <mutex>

class IDemux;
class IDecode;
class IResample;
class IVideoView;
class IAudioPlay;

class IPlayer : public XThread {
public:
    static IPlayer *Get(unsigned char index = 0);

    virtual bool Open(std::string_view path);

    virtual void Close();

    bool Start() override;

    virtual void InitView(void *win);

    // 获取当前的播放进度 0.0 - 1.0
    virtual double PlayPos();

    virtual bool Seek(double pos);

    void SetPause(bool isP) override;

protected:
    IPlayer() = default;

    // 用于音视频同步
    void Main() override;

public:
    IDemux *demux = nullptr;
    IDecode *vdecode = nullptr;
    IDecode *adecode = nullptr;
    IResample *resample = nullptr;
    IVideoView *videoView = nullptr;
    IAudioPlay *audioPlay = nullptr;
    bool isHardDecode = false;
    XParameter outPara{};
    std::mutex mux;
};


#endif //FFMPEG_ANDROID_IPLAYER_H
