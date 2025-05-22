//
// Created by 27515 on 2025-05-20.
//

#include "IResample.h"
#include "XLog.h"
#include <jni.h>

void IResample::Update(XData data) {
    XData d = this->Resample(data);
    if (d.size > 0) {
        this->Notify(d);
    }
}