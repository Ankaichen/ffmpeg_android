//
// Created by 27515 on 2025-05-18.
//

#ifndef FFMPEG_ANDROID_GLVIDEOVIEW_H
#define FFMPEG_ANDROID_GLVIDEOVIEW_H

#include "IVideoView.h"

#include <mutex>

class XTexture;

class GLVideoView : public IVideoView {
public:
    void SetRender(void *win) override;

    void Render(XData data) override;

    void Close() override;

protected:
    void *view = nullptr;
    XTexture *txt = nullptr;
    std::mutex mux;
};


#endif //FFMPEG_ANDROID_GLVIDEOVIEW_H
