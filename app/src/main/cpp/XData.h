//
// Created by 27515 on 2025-05-17.
//

#ifndef FFMPEG_ANDROID_XDATA_H
#define FFMPEG_ANDROID_XDATA_H


struct XData {
    unsigned char *data = nullptr;
    int size = 0;
    bool isAudio = false;
    void Drop();
};


#endif //FFMPEG_ANDROID_XDATA_H
