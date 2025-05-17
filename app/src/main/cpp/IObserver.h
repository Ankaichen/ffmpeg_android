//
// Created by 27515 on 2025-05-17.
//

#ifndef FFMPEG_ANDROID_IOBSERVER_H
#define FFMPEG_ANDROID_IOBSERVER_H

#include <vector>
#include <mutex>

#include "XData.h"
#include "XThread.h"

// 观察者和主体
class IObserver : public XThread {
public:
    ~IObserver() noexcept override = default;

    // 观察者函数
    virtual void Update(XData data) {};

    // 主体函数 添加观察者
    void AddObs(IObserver *obs);

    // 通知所有观察者
    void Notify(XData data);

protected:
    std::vector<IObserver *> obss{};
    std::mutex mux;
};


#endif //FFMPEG_ANDROID_IOBSERVER_H
