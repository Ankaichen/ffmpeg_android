//
// Created by 27515 on 2025-05-17.
//

#ifndef FFMPEG_ANDROID_XDATA_H
#define FFMPEG_ANDROID_XDATA_H


struct XData {
    unsigned char *data = nullptr;
    unsigned char *dates[8] = {nullptr};
    int size = 0;
    bool isAudio = false;
    int width = 0;
    int height = 0;

    void Drop();
};


#endif //FFMPEG_ANDROID_XDATA_H
