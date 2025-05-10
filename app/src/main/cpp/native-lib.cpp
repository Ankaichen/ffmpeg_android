#include <jni.h>
#include <string>
#include <android/log.h>

#define LOGW(...) __android_log_print(ANDROID_LOG_WARN, "testff", __VA_ARGS__)

extern "C" {
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
}

static double r2d(AVRational r) {
    if (r.num == 0 || r.den == 0) return 0.;
    return static_cast<double>(r.num) / static_cast<double>(r.den);
}

extern "C" JNIEXPORT jstring JNICALL
Java_com_example_ffmpegandroid_MainActivity_stringFromJNI(
        JNIEnv *env,
        jobject /* this */) {
    std::string hello = "Hello from C++ : ";
    hello += avcodec_configuration();

    // 初始化解封装
    av_register_all();
    // 初始化网络
    avformat_network_init();
    // 打开文件
    AVFormatContext *ic = nullptr;
    const char *path = "/sdcard/test3.mp4";
//    const char* path = "/sdcard/test2.flv";
    int re = avformat_open_input(&ic, path, nullptr, nullptr);
    if (re != 0) {
        LOGW("avformat_open_input failed!: %s", av_err2str(re));
        return env->NewStringUTF(hello.c_str());
    }
    LOGW("avformat_open_input %s success!", path);
    // 获取流信息 （通过读取部分流数据进行探测，用于没有文件头的情况）
    re = avformat_find_stream_info(ic, nullptr);
    if (re != 0) {
        LOGW("avformat_find_stream_info failed!: %s", av_err2str(re));
        return env->NewStringUTF(hello.c_str());
    }
    LOGW("duration = %lld, nb_streams = %d", ic->duration, ic->nb_streams);
    int fps = 0, videoStream = 0, audioStream = 0;
    for (int i = 0; i < ic->nb_streams; ++i) {
        AVStream *as = ic->streams[i];
        if (as->codecpar->codec_type == AVMEDIA_TYPE_VIDEO) {
            LOGW("视频数据");
            fps = static_cast<int>(r2d(as->avg_frame_rate));
            LOGW("fps = %d, width = %d, height = %d, code_id = %d, pix_format = %d", fps,
                 as->codecpar->width, as->codecpar->height, as->codecpar->codec_id,
                 as->codecpar->format);
        } else if (as->codecpar->codec_type == AVMEDIA_TYPE_AUDIO) {
            LOGW("音频数据");
            LOGW("sample_rate = %d, channels = %d, sample_format = %d",
                 as->codecpar->sample_rate, as->codecpar->channels, as->codecpar->format);
        }
    }
    // 获取视频流信息
    videoStream = av_find_best_stream(ic, AVMEDIA_TYPE_VIDEO, -1, -1, nullptr, -1);
    LOGW("av_find_best_stream videoStream = %d", videoStream);
    // 获取音频流信息
    audioStream = av_find_best_stream(ic, AVMEDIA_TYPE_AUDIO, -1, -1, nullptr, -1);
    LOGW("av_find_best_stream audioStream = %d", audioStream);

    // 读取帧数据
    AVPacket *pkt = av_packet_alloc();
    
    for (;;) {
        int re = av_read_frame(ic, pkt);
        if (re != 0) {
            LOGW("读取到结尾处!");
            int pos = static_cast<int>(70 * r2d(ic->streams[videoStream]->time_base));
            av_seek_frame(ic, videoStream, pos, AVSEEK_FLAG_BACKWARD | AVSEEK_FLAG_FRAME);
            continue;
        }
        LOGW("stream = %d, size = %d, pts = %lld, flag = %d",
             pkt->stream_index, pkt->size, pkt->pts, pkt->flags);
        av_packet_unref(pkt);
    }

    // 关闭文件
    avformat_close_input(&ic);

    return env->NewStringUTF(hello.c_str());
}

extern "C" JNIEXPORT jboolean JNICALL
Java_com_example_ffmpegandroid_MainActivity_Open(JNIEnv *env, jobject thiz, jstring url_,
                                                 jobject handle) {
    const char *url = env->GetStringUTFChars(url_, 0);
    FILE *fp = fopen(url, "rb");
    if (!fp) {
        LOGW("%s open failed!", url);
    } else {
        LOGW("%s open success!", url);
        fclose(fp);
    }
    env->ReleaseStringUTFChars(url_, url);
    return true;
}