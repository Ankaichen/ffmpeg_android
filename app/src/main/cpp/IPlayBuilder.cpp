//
// Created by 27515 on 2025-05-21.
//

#include "IPlayBuilder.h"

#include "IDemux.h"
#include "IDecode.h"
#include "IResample.h"
#include "IVideoView.h"
#include "IAudioPlay.h"
#include "XLog.h"

IPlayer *IPlayBuilder::BuildPlayer(unsigned char index) {
    IPlayer *play = CreatePlayer(index);

    IDemux *de = this->CreateDemux();
    IDecode *vdecode = this->CreateDecode();
    IDecode *adecode = this->CreateDecode();
    IVideoView *view = this->CreateVideoView();
    IResample *resample = this->CreateResample();
    IAudioPlay *audioPlay = this->CreateAudioPlay();

    de->AddObs(vdecode);
    de->AddObs(adecode);
    vdecode->AddObs(view);
    adecode->AddObs(resample);
    resample->AddObs(audioPlay);

    play->demux = de;
    play->vdecode = vdecode;
    play->adecode = adecode;
    play->resample = resample;
    play->audioPlay = audioPlay;
    play->videoView = view;

    XLOGI("IPlayBuilder::BuildPlayer success!");
    return play;
}
