//
// Created by 27515 on 2025-05-17.
//

#ifndef FFMPEG_ANDROID_XLOG_H
#define FFMPEG_ANDROID_XLOG_H


class XLog {

};

#ifdef ANDROID
#include <android/log.h>
#define XLOGD(...) __android_log_print(ANDROID_LOG_DEBUG, "XPlay", __VA_ARGS__)
#define XLOGI(...) __android_log_print(ANDROID_LOG_INFO, "XPlay", __VA_ARGS__)
#define XLOGE(...) __android_log_print(ANDROID_LOG_ERROR, "XPlay", __VA_ARGS__)
#elif
#define XLOGD(...) printf(ANDROID_LOG_DEBUG, "XPlay", __VA_ARGS__)
#define XLOGI(...) printf(ANDROID_LOG_INFO, "XPlay", __VA_ARGS__)
#define XLOGE(...) printf(ANDROID_LOG_ERROR, "XPlay", __VA_ARGS__)
#endif

#endif //FFMPEG_ANDROID_XLOG_H
