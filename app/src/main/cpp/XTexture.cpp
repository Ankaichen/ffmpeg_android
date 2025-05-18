//
// Created by 27515 on 2025-05-18.
//

#include "XTexture.h"
#include "XLog.h"
#include "XEGL.h"
#include "XShader.h"

class CXTexture : public XTexture {
public:

    XShader sh;

    bool Init(void *win) override {
        if (win == nullptr) {
            XLOGE("XTexture Init failed win is nullptr!");
            return false;
        }
        if (!XEGL::Get()->Init(win)) return false;
        if (!sh.Init()) return false;
        XLOGI("XTexture Init success!");
        return true;
    }

    void Draw(unsigned char *data[], int width, int height) override {
        sh.GetTexture(0, width, height, data[0]);         // Y
        sh.GetTexture(1, width / 2, height / 2, data[1]); // U
        sh.GetTexture(2, width / 2, height / 2, data[2]); // V
        sh.Draw();
        XEGL::Get()->Draw();
    }
};

XTexture *XTexture::Create() {
    return new CXTexture;
}
