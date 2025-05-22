//
// Created by 27515 on 2025-05-21.
//

#include "FFPlayBuilder.h"

#include "FFDemux.h"
#include "FFDecode.h"
#include "FFResample.h"
#include "GLVideoView.h"
#include "SLAudioPlay.h"
#include "IPlayer.h"

IDemux *FFPlayBuilder::CreateDemux() {
    IDemux *demux = new FFDemux();
    return demux;
}

IDecode *FFPlayBuilder::CreateDecode() {
    IDecode *decode = new FFDecode();
    return decode;
}

IResample *FFPlayBuilder::CreateResample() {
    IResample *resample = new FFResample();
    return resample;
}

IAudioPlay *FFPlayBuilder::CreateAudioPlay() {
    IAudioPlay *audioPlay = new SLAudioPlay();
    return audioPlay;
}

IVideoView *FFPlayBuilder::CreateVideoView() {
    IVideoView *videoView = new GLVideoView();
    return videoView;
}

IPlayer *FFPlayBuilder::CreatePlayer(unsigned char index) {
    return IPlayer::Get(index);
}

FFPlayBuilder *FFPlayBuilder::Get() {
    static FFPlayBuilder ffPlayBuilder;
    return &ffPlayBuilder;
}

void FFPlayBuilder::InitHard(void *vm) {
    FFDecode::InitHard(vm);
}
