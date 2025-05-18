//
// Created by 27515 on 2025-05-17.
//

#ifndef FFMPEG_ANDROID_FFDECODE_H
#define FFMPEG_ANDROID_FFDECODE_H

#include "IDecode.h"

class AVCodecContext;
class AVFrame;

class FFDecode : public IDecode {
public:
    bool Open(XParameter para) override;

    bool SendPacket(XData pkt) override;

    XData RecvFrame() override;

protected:
    AVCodecContext *codec = nullptr;
    AVFrame *frame = nullptr;
};


#endif //FFMPEG_ANDROID_FFDECODE_H
