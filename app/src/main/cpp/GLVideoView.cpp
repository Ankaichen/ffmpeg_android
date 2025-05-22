//
// Created by 27515 on 2025-05-18.
//

#include "GLVideoView.h"
#include "XTexture.h"
#include "XLog.h"

void GLVideoView::SetRender(void *win) {
    std::lock_guard<std::mutex> guard(this->mux);
    this->view = win;
}

void GLVideoView::Render(XData data) {
    std::lock_guard<std::mutex> guard(this->mux);
    if (this->view == nullptr) return;
    if (this->txt == nullptr) {
        this->txt = XTexture::Create();
        this->txt->Init(this->view, static_cast<XTextureType>(data.format));
    }
    this->txt->Draw(data.dates, data.width, data.height);
}

void GLVideoView::Close() {
    std::lock_guard<std::mutex> guard(this->mux);
    if (this->txt) {
        this->txt->Drop();
        this->txt = nullptr;
    }
}
