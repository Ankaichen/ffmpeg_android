//
// Created by 27515 on 2025-05-18.
//

#include "GLVideoView.h"
#include "XTexture.h"
#include "XLog.h"

void GLVideoView::SetRender(void *win) {
    this->view = win;
}

void GLVideoView::Render(XData data) {
    if (this->view == nullptr) return;
    if (this->txt == nullptr) {
        this->txt = XTexture::Create();
        this->txt->Init(this->view);
    }
    this->txt->Draw(data.dates, data.width, data.height);
}
