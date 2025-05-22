//
// Created by 27515 on 2025-05-17.
//

#ifndef FFMPEG_ANDROID_XDATA_H
#define FFMPEG_ANDROID_XDATA_H

enum class XDataType {
    AVPACKET_TYPE = 0,
    UCHAR_TYPE = 1
};

struct XData {
    XDataType type = XDataType::AVPACKET_TYPE;
    long long pts = 0;
    unsigned char *data = nullptr;
    unsigned char *dates[8] = {nullptr};
    int size = 0;
    bool isAudio = false;
    int width = 0;
    int height = 0;
    int format = 0;

    bool Alloc(int s, const char *d = nullptr);

    void Drop();
};


#endif //FFMPEG_ANDROID_XDATA_H
