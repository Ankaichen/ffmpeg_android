//
// Created by 27515 on 2025-05-18.
//

#ifndef FFMPEG_ANDROID_XSHADER_H
#define FFMPEG_ANDROID_XSHADER_H

#include <mutex>

enum class XShaderType {
    XSHADER_YUV420P = 0,
    XSHADER_NV12 = 25,
    XSHADER_NV21 = 26
};

class XShader {
public:
    virtual bool Init(XShaderType type);
    virtual void Close();


    // 获取材质并映射到内存
    virtual void GetTexture(unsigned int index, int width, int height, unsigned char *buf);

    virtual void Draw();

protected:
    unsigned int vsh{0};
    unsigned int fsh{0};
    unsigned int program{0};
    unsigned int texts[100] = {0};
    std::mutex mux;
};


#endif //FFMPEG_ANDROID_XSHADER_H
