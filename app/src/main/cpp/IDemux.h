//
// Created by 27515 on 2025-05-17.
//

#ifndef FFMPEG_ANDROID_IDEMUX_H
#define FFMPEG_ANDROID_IDEMUX_H

#include <string_view>

#include "./XData.h"
#include "./IObserver.h"
#include "./XParameter.h"

// 解封装接口类
class IDemux : public IObserver {
public:
    ~IDemux() noexcept override = default;

    // 打开文件 或者流媒体 rmtp http rtsp
    virtual bool Open(std::string_view url) = 0;

    // 获取视频参数
    virtual XParameter GetVPara() = 0;

    // 获取音频参数
    virtual XParameter GetAPara() = 0;

    // 读取一帧数据 数据由调用者清理
    virtual XData Read() = 0;

protected:
    void Main() override;

protected:
    // 总时长 （毫秒）
    int totalMs = 0;
};


#endif //FFMPEG_ANDROID_IDEMUX_H
