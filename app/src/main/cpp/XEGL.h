//
// Created by 27515 on 2025-05-18.
//

#ifndef FFMPEG_ANDROID_XEGL_H
#define FFMPEG_ANDROID_XEGL_H

class XEGL {
public:
    virtual bool Init(void *win) = 0;
    virtual void Close() = 0;

    virtual void Draw() = 0;

    static XEGL *Get();

protected:
    XEGL() = default;
};


#endif //FFMPEG_ANDROID_XEGL_H
