//
// Created by 27515 on 2025-05-17.
//

#ifndef FFMPEG_ANDROID_XPARAMETER_H
#define FFMPEG_ANDROID_XPARAMETER_H

class AVCodecParameters;

class XParameter {
public:
    AVCodecParameters *para = nullptr;
    int channels = 2;
    int sample_rate = 44100;
};


#endif //FFMPEG_ANDROID_XPARAMETER_H
