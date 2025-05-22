//
// Created by 27515 on 2025-05-20.
//

#include "IAudioPlay.h"
#include "XLog.h"

void IAudioPlay::Update(XData data) {
    // 压入缓冲队列
    if (data.size <= 0 || data.data == nullptr) return;
    while (!this->isExit) {
        std::unique_lock<std::mutex> guard(this->framesMutex);
        if (frames.size() > this->maxFrames) {
            guard.unlock();
            XSleep(1);
            continue;
        }
        this->frames.push_back(data);
        break;
    }
}

XData IAudioPlay::GetData() {
    XData data;
    while (!this->isExit) {
        std::unique_lock<std::mutex> guard(this->framesMutex);
        if (IsPause()) {
            XSleep(2);
            continue;
        }
        if (this->frames.empty()) {
            guard.unlock();
            XSleep(1);
            continue;
        }
        data = this->frames.front();
        this->frames.pop_front();
        break;
    }
    this->pts = data.pts;
    return data;
}

void IAudioPlay::Clear() {
    std::lock_guard<std::mutex> guard(this->framesMutex);
    while (!this->frames.empty()) {
        this->frames.front().Drop();
        this->frames.pop_front();
    }
    this->pts = 0;
}
