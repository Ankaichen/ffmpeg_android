//
// Created by 27515 on 2025-05-17.
//

#ifndef FFMPEG_ANDROID_IDECODE_H
#define FFMPEG_ANDROID_IDECODE_H

#include <list>

#include "XParameter.h"
#include "IObserver.h"

// 解码接口，支持硬解码
class IDecode : public IObserver {
public:
    // 打开解码器
    virtual bool Open(XParameter para, bool isHard) = 0;

    virtual void Close() = 0;

    virtual void Clear();

    // future模型 发送数据到线程解码
    virtual bool SendPacket(XData pkt) = 0;

    // 从线程中获取解码结果 再次调用会复用上次空间 线程不安全
    virtual XData RecvFrame() = 0;

    // 由主体notify的数据
    void Update(XData data) override;

    int getMaxList() const { return this->maxList; }

    void setMaxList(int m) { this->maxList = m; }

    void setSynPts(long long pts) { this->synPts = pts; }

    long long getPts() const { return this->pts; }

protected:
    void Main() override;

protected:
    bool isAudio = false;
    // 读取缓冲
    std::list<XData> packs;
    std::mutex packsMutex;
    // 最大队列缓冲
    int maxList{100};
    // 同步时间 再次打开文件要清理
    long long synPts = 0;
    // 当前播放到的位置
    long long pts = 0;
};


#endif //FFMPEG_ANDROID_IDECODE_H
