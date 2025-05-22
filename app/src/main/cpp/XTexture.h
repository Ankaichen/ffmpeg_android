//
// Created by 27515 on 2025-05-18.
//

#ifndef FFMPEG_ANDROID_XTEXTURE_H
#define FFMPEG_ANDROID_XTEXTURE_H

enum class XTextureType {
    XTEXTURE_YUV420P = 0,
    XTEXTURE_NV12 = 25,
    XTEXTURE_NV21 = 26
};

class XTexture {
public:
    virtual bool Init(void *win, XTextureType type) = 0;

    virtual void Draw(unsigned char *data[], int width, int height) = 0;

    virtual void Drop() = 0;

    static XTexture *Create();

    virtual ~XTexture() = default;

protected:
    XTexture() = default;
};


#endif //FFMPEG_ANDROID_XTEXTURE_H
