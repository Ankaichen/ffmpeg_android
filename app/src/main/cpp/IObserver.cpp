//
// Created by 27515 on 2025-05-17.
//

#include "IObserver.h"

#include <algorithm>

using namespace std;

void IObserver::AddObs(IObserver *obs) {
    if (obs == nullptr) return;
    lock_guard<mutex> lock(this->mux);
    this->obss.push_back(obs);
}

void IObserver::Notify(XData data) {
    lock_guard<mutex> lock(this->mux);
    std::for_each(this->obss.begin(), this->obss.end(),
                  [&data](IObserver *obs) { obs->Update(data); });
}
