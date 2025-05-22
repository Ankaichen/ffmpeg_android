//
// Created by 27515 on 2025-05-21.
//

#include "IPlayer.h"
#include "IDemux.h"
#include "IDecode.h"
#include "IAudioPlay.h"
#include "IResample.h"
#include "IVideoView.h"
#include "XLog.h"

IPlayer *IPlayer::Get(unsigned char index) {

    static IPlayer iPlayer[256];
    return &iPlayer[index];
}

bool IPlayer::Open(std::string_view path) {
    this->Close();
    std::lock_guard<std::mutex> guard(this->mux);
    // 解封装
    if (this->demux == nullptr || !this->demux->Open(path)) {
        XLOGE("Demux Open failed: %s", path.data());
        return false;
    }
    // 解码 有可能不需要 解封装之后可能是原始数据
    if (this->vdecode == nullptr ||
        !this->vdecode->Open(this->demux->GetVPara(), this->isHardDecode)) {
        XLOGE("VDecode Open failed: %s", path.data());
    }
    if (this->adecode == nullptr || !this->adecode->Open(this->demux->GetAPara(), false)) {
        XLOGE("ADecode Open failed: %s", path.data());
    }
    // 重采样 可能不需要 解码后或解封装后可能是能播放的数据
    this->outPara = demux->GetAPara();
    if (this->resample == nullptr ||
        !this->resample->Open(this->demux->GetAPara(), this->outPara)) {
        XLOGE("Resample Open failed: %s", path.data());
    }
    XLOGI("IPlayer::Open %s success!", path.data());
    return true;
}

bool IPlayer::Start() {
    std::lock_guard<std::mutex> guard(this->mux);
    if (audioPlay)
        audioPlay->StartPlay(this->outPara);
    if (adecode)
        adecode->Start();
    if (vdecode)
        vdecode->Start();
    if (!demux || !demux->Start()) {
        XLOGE("Demux Start failed!");
        return false;
    }
    XThread::Start();
    return true;
}

void IPlayer::InitView(void *win) {
    std::lock_guard<std::mutex> guard(this->mux);
    if (this->videoView) {
        this->videoView->Close();
        this->videoView->SetRender(win);
    }
}

void IPlayer::Main() {
    while (!isExit) {
        std::unique_lock<std::mutex> guard(this->mux);
        if (!this->audioPlay || !this->vdecode) {
            guard.unlock();
            XSleep(2);
            continue;
        }
        // 同步
        // 获取音频的pts 告诉视频
        long long apts = this->audioPlay->getPts();
        this->vdecode->setSynPts(apts);
        guard.unlock();
        XSleep(2);
    }
}

void IPlayer::Close() {
    // 关闭主体线程 再清理观察者
    std::lock_guard<std::mutex> guard(this->mux);
    // 同步线程
    XThread::Stop();
    // 解封装
    if (this->demux) this->demux->Stop();
    // 解码
    if (this->vdecode) this->vdecode->Stop();
    if (this->adecode) this->adecode->Stop();
    // 音频缓冲线程
    // 清理缓冲队列
    if (this->vdecode) this->vdecode->Clear();
    if (this->adecode) this->adecode->Clear();
    if (this->audioPlay) this->audioPlay->Clear();
    // 清理资源
    if (this->audioPlay) this->audioPlay->Close();
    if (this->videoView) this->videoView->Close();
    if (this->vdecode) this->vdecode->Close();
    if (this->adecode) this->adecode->Close();
    if (this->demux) this->demux->Close();
}

double IPlayer::PlayPos() {
    double pos = 0.0;
    std::lock_guard<std::mutex> guard(this->mux);
    int total = 0;
    if (this->demux)
        total = this->demux->getTotalMs();
    if (total > 0) {
        if (this->vdecode) {
            pos = static_cast<double >(this->vdecode->getPts()) / static_cast<double >(total);
        }
    }
    return pos;
}

bool IPlayer::Seek(double pos) {
    bool re = false;
    std::lock_guard<std::mutex> guard(this->mux);
    // 暂停所有线程

    if (this->demux)
        re = demux->Seek(pos);
    return re;
}

void IPlayer::SetPause(bool isP) {
    std::lock_guard<std::mutex> guard(this->mux);
    XThread::SetPause(isP);
    if (this->demux) this->demux->SetPause(isP);
    if (this->vdecode) this->vdecode->SetPause(isP);
    if (this->adecode) this->adecode->SetPause(isP);
    if (this->audioPlay) this->audioPlay->SetPause(isP);
//    if (this->videoView) this->videoView->SetPause(isP);
}
