//
// Created by 27515 on 2025-05-17.
//

#ifndef FFMPEG_ANDROID_FFDECODE_H
#define FFMPEG_ANDROID_FFDECODE_H

#include "IDecode.h"

#include <mutex>

class AVCodecContext;
class AVFrame;

class FFDecode : public IDecode {
public:
    static void InitHard(void * vm);

    bool Open(XParameter para, bool isHard) override;

    virtual void Close();

    bool SendPacket(XData pkt) override;

    XData RecvFrame() override;

protected:
    AVCodecContext *codec = nullptr;
    AVFrame *frame = nullptr;
    std::mutex mux;
};


#endif //FFMPEG_ANDROID_FFDECODE_H
