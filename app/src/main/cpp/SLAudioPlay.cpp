//
// Created by 27515 on 2025-05-20.
//

#include "SLAudioPlay.h"
#include "XLog.h"
#include <SLES/OpenSLES.h>
#include <SLES/OpenSLES_Android.h>

static SLObjectItf enginSL = nullptr;
static SLEngineItf eng = nullptr;
static SLObjectItf mix = nullptr;
static SLObjectItf player = nullptr;
static SLPlayItf iplayer = nullptr;
static SLAndroidSimpleBufferQueueItf pcmQue = nullptr;


SLAudioPlay::SLAudioPlay() : buf{new unsigned char[1024 * 1024]} {}

SLAudioPlay::~SLAudioPlay() noexcept {
    delete[] this->buf;
}

static SLEngineItf CreateSL() {
    SLresult re;
    SLEngineItf en;
    re = slCreateEngine(&enginSL, 0, 0, 0, 0, 0);
    if (re != SL_RESULT_SUCCESS) return nullptr;
    re = (*enginSL)->Realize(enginSL, SL_BOOLEAN_FALSE);
    if (re != SL_RESULT_SUCCESS) return nullptr;
    re = (*enginSL)->GetInterface(enginSL, SL_IID_ENGINE, &en);
    if (re != SL_RESULT_SUCCESS) return nullptr;
    return en;
}

static void PcmCall(SLAndroidSimpleBufferQueueItf bf, void *contex) {
    auto *ap = reinterpret_cast<SLAudioPlay *>(contex);
    if (ap == nullptr) {
        XLOGE("PcmCall failed!");
        return;
    }
    ap->PlayCall(bf);
}

void SLAudioPlay::PlayCall(const void *const bufq) {
    if (bufq == nullptr) {
        XLOGE("SLAudioPlay::PlayCall failed!");
        return;
    }
    auto bf = reinterpret_cast<SLAndroidSimpleBufferQueueItf>(bufq);
    XData data = this->GetData();
    if (data.size <= 0) {
        XLOGE("GetData failed!");
        return;
    }
    std::lock_guard<std::mutex> guard(this->mux);
    if (this->buf == nullptr) {
        XLOGE("buf is nullptr!");
        return;
    }
    memcpy(this->buf, data.data, data.size);
    if (bf && (*bf)) (*bf)->Enqueue(bf, this->buf, data.size);
    data.Drop();
}

bool SLAudioPlay::StartPlay(XParameter out) {
    this->Close();
    std::lock_guard<std::mutex> guard(this->mux);
    // 创建引擎
    eng = CreateSL();
    if (eng == nullptr) {
        XLOGE("CreateSL failed!");
        return false;
    }
    XLOGI("CreateSL success!");
    // 创建混音器
    SLresult re;
    re = (*eng)->CreateOutputMix(eng, &mix, 0, nullptr, nullptr);
    if (re != SL_RESULT_SUCCESS) {
        XLOGE("CreateOutputMix failed!");
        return false;
    }
    XLOGI("CreateOutputMix success!");
    re = (*mix)->Realize(mix, SL_BOOLEAN_FALSE);
    if (re != SL_RESULT_SUCCESS) {
        XLOGE("Realize failed!");
        return false;
    }
    XLOGI("Realize success!");
    SLDataLocator_OutputMix outmix = {SL_DATALOCATOR_OUTPUTMIX, mix};
    SLDataSink audioSink = {&outmix, nullptr};
    // 配置音频信息
    // 缓冲队列
    SLDataLocator_AndroidSimpleBufferQueue que = {SL_DATALOCATOR_ANDROIDSIMPLEBUFFERQUEUE, 10};
    // 音频格式
    SLDataFormat_PCM pcm = {
            SL_DATAFORMAT_PCM,
            static_cast<SLuint32>(out.channels), // 声道数
            static_cast<SLuint32>(out.sample_rate * 1000),
            SL_PCMSAMPLEFORMAT_FIXED_16,
            SL_PCMSAMPLEFORMAT_FIXED_16,
            SL_SPEAKER_FRONT_LEFT | SL_SPEAKER_FRONT_RIGHT,
            SL_BYTEORDER_LITTLEENDIAN // 小端字节序
    };
    SLDataSource ds = {&que, &pcm};
    // 创建播放器
    const SLInterfaceID ids[] = {SL_IID_BUFFERQUEUE};
    const SLboolean req[] = {SL_BOOLEAN_TRUE};
    re = (*eng)->CreateAudioPlayer(eng, &player, &ds, &audioSink,
                                   sizeof(ids) / sizeof(SLInterfaceID), ids, req);
    if (re != SL_RESULT_SUCCESS) {
        XLOGE("CreateAudioPlayer failed!");
        return false;
    }
    XLOGI("CreateAudioPlayer success!");
    (*player)->Realize(player, SL_BOOLEAN_FALSE);
    // 获取接口
    re = (*player)->GetInterface(player, SL_IID_PLAY, &iplayer);
    if (re != SL_RESULT_SUCCESS) {
        XLOGE("GetInterface SL_IID_PLAY failed!");
        return false;
    }
    XLOGI("GetInterface SL_IID_PLAY success!");
    re = (*player)->GetInterface(player, SL_IID_BUFFERQUEUE, &pcmQue);
    if (re != SL_RESULT_SUCCESS) {
        XLOGE("GetInterface SL_IID_BUFFERQUEUE failed!");
        return false;
    }
    XLOGI("GetInterface SL_IID_BUFFERQUEUE success!");
    // 设置回调函数 播放队列空的时候调用
    (*pcmQue)->RegisterCallback(pcmQue, PcmCall, this);
    // 设置为播放状态
    (*iplayer)->SetPlayState(iplayer, SL_PLAYSTATE_PLAYING);
    // 启动队列回调
    (*pcmQue)->Enqueue(pcmQue, "", 1);
    XLOGI("SLAudioPlay::StartPlay success!");
    return true;
}

void SLAudioPlay::Close() {
    this->Clear();
    std::lock_guard<std::mutex> guard(this->mux);
    // 停止播放
    if (iplayer && (*iplayer))
        (*iplayer)->SetPlayState(iplayer, SL_PLAYSTATE_STOPPED);
    // 清理播放队列
    if (pcmQue && (*pcmQue))
        (*pcmQue)->Clear(pcmQue);
    // 销毁Player对象
    if (player && (*player))
        (*player)->Destroy(player);
    // 销毁混音器
    if (mix && (*mix))
        (*mix)->Destroy(mix);
    // 销毁播放引擎
    if (enginSL && (*enginSL))
        (*enginSL)->Destroy(enginSL);
    iplayer = nullptr;
    pcmQue = nullptr;
    player = nullptr;
    mix = nullptr;
    enginSL = nullptr;
}
