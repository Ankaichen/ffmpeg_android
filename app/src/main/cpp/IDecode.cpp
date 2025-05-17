//
// Created by 27515 on 2025-05-17.
//

#include "IDecode.h"
#include "XLog.h"

using namespace std;

void IDecode::Update(XData data) {
    if (data.isAudio != this->isAudio) return;
    while (!isExit) {
        XSleep(1);
        lock_guard<mutex> guard(this->packsMutex);
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
        unique_lock<mutex> guard(this->packsMutex);
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
                XLOGI("RecvFrame %d", frame.size);
                // 发送数据给观察者
                this->Notify(frame);
            }
        }
        pack.Drop();
    }
}
