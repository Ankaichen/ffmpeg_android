//
// Created by 27515 on 2025-05-17.
//

#ifndef FFMPEG_ANDROID_XTHREAD_H
#define FFMPEG_ANDROID_XTHREAD_H

void XSleep(int mis);

class XThread {
public:
    virtual ~XThread() = default;

    // 启动线程
    virtual bool Start();

    // 安全停止线程
    virtual void Stop();

    virtual void SetPause(bool isP);

    virtual bool IsPause() {
        isPausing = isPause;
        return isPause;
    }

    // 入口主函数
    virtual void Main() {}

private:
    void ThreadMain();

protected:
    bool isExit{false};
    bool isRunning{false};
    bool isPause{false};
    bool isPausing{false};
};


#endif //FFMPEG_ANDROID_XTHREAD_H
