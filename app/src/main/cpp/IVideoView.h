//
// Created by 27515 on 2025-05-18.
//

#ifndef FFMPEG_ANDROID_IVIDEOVIEW_H
#define FFMPEG_ANDROID_IVIDEOVIEW_H

#include "XData.h"
#include "IObserver.h"

class IVideoView : public IObserver {
public:
    virtual void SetRender(void *win) = 0;
    virtual void Render(XData data) = 0;
    void Update(XData data) override;
};


#endif //FFMPEG_ANDROID_IVIDEOVIEW_H
