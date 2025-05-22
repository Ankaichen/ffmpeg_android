//
// Created by 27515 on 2025-05-21.
//

#include "IPlayerPorxy.h"

#include "FFPlayBuilder.h"

bool IPlayerPorxy::Open(std::string_view path) {
    std::lock_guard<std::mutex> guard(this->mux);
    if (this->player) {
        this->player->isHardDecode = this->isHardDecode;
        return this->player->Open(path);
    }
    return false;
}

void IPlayerPorxy::Close() {
    std::lock_guard<std::mutex> guard(this->mux);
    if (this->player) this->player->Close();
}

bool IPlayerPorxy::Start() {
    std::lock_guard<std::mutex> guard(this->mux);
    if (this->player) return this->player->Start();
    return false;
}

void IPlayerPorxy::InitView(void *win) {
    std::lock_guard<std::mutex> guard(this->mux);
    if (this->player) this->player->InitView(win);
}

void IPlayerPorxy::Init(void *vm) {
    std::lock_guard<std::mutex> guard(this->mux);
    if (vm) FFPlayBuilder::InitHard(vm);
    if (!this->player)
        this->player = FFPlayBuilder::Get()->BuildPlayer(0);
}

IPlayerPorxy *IPlayerPorxy::Get() {
    static IPlayerPorxy px;
    return &px;
}

double IPlayerPorxy::PlayPos() {
    double pos = 0.0;
    std::lock_guard<std::mutex> guard(this->mux);
    if (this->player)
        pos = player->PlayPos();
    return pos;
}

bool IPlayerPorxy::Seek(double pos) {
    bool re = false;
    std::lock_guard<std::mutex> guard(this->mux);
    if (this->player)
        re = this->player->Seek(pos);
    return re;
}

void IPlayerPorxy::SetPause(bool isP) {
    std::lock_guard<std::mutex> guard(this->mux);
    if (this->player)
        this->player->SetPause(isP);
}

bool IPlayerPorxy::IsPause() {
    std::lock_guard<std::mutex> guard(this->mux);
    if (this->player)
        this->player->IsPause();
}
