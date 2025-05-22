//
// Created by 27515 on 2025-05-17.
//

#include "IDemux.h"
#include "XLog.h"

void IDemux::Main() {
    while (!this->isExit) {
        if (IsPause()) {
            XSleep(2);
            continue;
        }
        XData d = this->Read();
        if (d.size > 0) {
            this->Notify(d);
        } else {
            XSleep(2);
        }
//        XLOGI("IDemux Read %d", d.size);
    }
}
