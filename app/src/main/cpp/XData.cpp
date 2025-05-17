//
// Created by 27515 on 2025-05-17.
//

#include "XData.h"

extern "C" {
#include <libavformat/avformat.h>
}

void XData::Drop() {
    if (!data) return;
    av_packet_free(reinterpret_cast<AVPacket**>(&data));
    data = nullptr;
    size = 0;
}
