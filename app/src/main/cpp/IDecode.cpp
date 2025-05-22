//
// Created by 27515 on 2025-05-17.
//

#include "IDecode.h"
#include "XLog.h"

void IDecode::Update(XData data) {
    if (data.isAudio != this->isAudio) return;
    while (!isExit) {
        XSleep(1);
        std::lock_guard<std::mutex> guard(this->packsMutex);
        // 阻塞
        if (this->packs.size() < this->maxList) {
            // 生产者
            this->packs.push_back(data);
            break;
        }
    }
}

void IDecode::Main() {
    while (!this->isExit) {
        std::unique_lock<std::mutex> guard(this->packsMutex);
        if (IsPause()) {
            XSleep(2);
            continue;
        }
        // 判断音视频同步
        if (!this->isAudio && this->synPts > 0) {
            if (this->synPts < this->pts) {
                guard.unlock();
                XSleep(1);
                continue;
            }
        }
        if (this->packs.empty()) {
            guard.unlock();
            XSleep(1);
            continue;
        }
        // 消费者
        XData pack = packs.front();
        packs.pop_front();
        // 发送数据到解码线程
        if (this->SendPacket(pack)) {
            while (!isExit) {
                // 获取解码数据
                XData frame = this->RecvFrame();
                if (frame.data == nullptr) break;
                // XLOGD("RecvFrame %d", frame.size);
                // 发送数据给观察者
                this->pts = frame.pts;
                this->Notify(frame);
            }
        }
        pack.Drop();
    }
}

void IDecode::Clear() {
    std::lock_guard<std::mutex> guard(this->packsMutex);
    while (!this->packs.empty()) {
        packs.front().Drop();
        packs.pop_front();
    }
    this->pts = 0;
    this->synPts = 0;
}
