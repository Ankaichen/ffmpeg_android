//
// Created by 27515 on 2025-05-20.
//

#ifndef FFMPEG_ANDROID_IAUDIOPLAY_H
#define FFMPEG_ANDROID_IAUDIOPLAY_H

#include "IObserver.h"
#include "XParameter.h"

#include <list>
#include <mutex>

class IAudioPlay : public IObserver {
public:
    IAudioPlay() = default;

    ~IAudioPlay() noexcept override = default;

    // 缓冲满后阻塞
    void Update(XData data) override;

    // 获取换成数据 没有则阻塞
    virtual XData GetData();

    virtual bool StartPlay(XParameter out) = 0;

    virtual void Close() = 0;

    virtual void Clear();

    inline long long getPts() const { return this->pts; }

protected:
    std::list<XData> frames{};
    std::mutex framesMutex;
    int maxFrames{100};
    long long pts = 0;
};


#endif //FFMPEG_ANDROID_IAUDIOPLAY_H
