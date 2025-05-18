//
// Created by 27515 on 2025-05-18.
//

#ifndef FFMPEG_ANDROID_XTEXTURE_H
#define FFMPEG_ANDROID_XTEXTURE_H


class XTexture {
public:
    virtual bool Init(void *win) = 0;

    virtual void Draw(unsigned char *data[], int width, int height) = 0;

    static XTexture *Create();
};


#endif //FFMPEG_ANDROID_XTEXTURE_H
