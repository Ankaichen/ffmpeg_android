//
// Created by 27515 on 2025-05-17.
//

#include "XData.h"

extern "C" {
#include <libavformat/avformat.h>
}

void XData::Drop() {
    if (!data) return;
    if (this->type == XDataType::AVPACKET_TYPE) {
        av_packet_free(reinterpret_cast<AVPacket **>(&data));
    } else {
        delete data;
    }
    data = nullptr;
    size = 0;
}

bool XData::Alloc(int s, const char *d) {
    this->Drop();
    this->type = XDataType::UCHAR_TYPE;
    if (s <= 0) return false;
    this->data = new unsigned char[s];
    if (!this->data) return false;
    if (d) {
        memcpy(this->data, d, s);
    }
    this->size = s;
    return true;
}
