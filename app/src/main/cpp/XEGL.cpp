//
// Created by 27515 on 2025-05-18.
//

#include "XEGL.h"
#include "XLog.h"

#include <android/native_window.h>
#include <EGL/egl.h>
#include <mutex>

class CXEGL : public XEGL {
public:
    EGLDisplay display = EGL_NO_DISPLAY;
    EGLSurface surface = EGL_NO_SURFACE;
    EGLContext context = EGL_NO_CONTEXT;
    std::mutex mux;

    void Draw() override {
        std::lock_guard<std::mutex> guard(this->mux);
        if (this->display == EGL_NO_DISPLAY || this->surface == EGL_NO_SURFACE) return;
        eglSwapBuffers(this->display, this->surface);
    }

    bool Init(void *win) override {
        this->Close();
        auto *nwin = reinterpret_cast<ANativeWindow *>(win);
        std::lock_guard<std::mutex> guard(this->mux);
        // 初始化egl
        // display创建和初始化
        this->display = eglGetDisplay(EGL_DEFAULT_DISPLAY);
        if (this->display == EGL_NO_DISPLAY) {
            XLOGE("eglGetDisplay failed!");
            return false;
        }
        XLOGI("eglGetDisplay success!");
        if (EGL_TRUE != eglInitialize(this->display, nullptr, nullptr)) {
            XLOGE("eglInitialize failed!");
            return false;
        }
        XLOGI("eglInitialize success!");
        // 获取配置 创建surface
        EGLConfig config = nullptr;
        EGLint configNum, configSpec[] = {
                EGL_RED_SIZE, 8,
                EGL_GREEN_SIZE, 8,
                EGL_BLUE_SIZE, 8,
                EGL_SURFACE_TYPE, EGL_WINDOW_BIT, EGL_NONE
        };
        if (EGL_TRUE != eglChooseConfig(this->display, configSpec, &config, 1, &configNum)) {
            XLOGE("eglChooseConfig failed!");
            return false;
        }
        XLOGI("eglChooseConfig success!");
        this->surface = eglCreateWindowSurface(this->display, config, nwin, nullptr);
        if (this->surface == EGL_NO_SURFACE) {
            XLOGE("eglCreateWindowSurface failed!");
            return false;
        }
        XLOGI("eglCreateWindowSurface success!");
        // 创建并打开上下文
        const EGLint ctxAttr[] = {EGL_CONTEXT_CLIENT_VERSION, 2, EGL_NONE};
        this->context = eglCreateContext(this->display, config, EGL_NO_CONTEXT, ctxAttr);
        if (this->context == EGL_NO_CONTEXT) {
            XLOGE("eglCreateContext failed!");
            return false;
        }
        XLOGI("eglCreateContext success!");
        if (EGL_TRUE !=
            eglMakeCurrent(this->display, this->surface, this->surface, this->context)) {
            XLOGE("eglMakeCurrent failed!");
            return false;
        }
        XLOGI("eglMakeCurrent success!");
        return true;
    }

    void Close() override {
        std::lock_guard<std::mutex> guard(this->mux);
        if (this->display == EGL_NO_DISPLAY) return;
        eglMakeCurrent(this->display, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
        if (this->surface != EGL_NO_SURFACE)
            eglDestroySurface(this->display, this->surface);
        if (this->context != EGL_NO_CONTEXT)
            eglDestroyContext(this->display, this->context);
        eglTerminate(this->display);
        this->display = EGL_NO_DISPLAY;
        this->surface = EGL_NO_SURFACE;
        this->context = EGL_NO_CONTEXT;
    }
};

XEGL *XEGL::Get() {
    static CXEGL egl;
    return &egl;
}
