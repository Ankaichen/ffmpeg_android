#include <jni.h>
#include <string>
#include <android/log.h>

#define LOGW(...) __android_log_print(ANDROID_LOG_WARN, "testff", __VA_ARGS__)

extern "C" {
#include <libavcodec/avcodec.h>
}


extern "C" JNIEXPORT jstring JNICALL
Java_com_example_ffmpegandroid_MainActivity_stringFromJNI(
        JNIEnv *env,
        jobject /* this */) {
    std::string hello = "Hello from C++ : ";
    hello += avcodec_configuration();
    return env->NewStringUTF(hello.c_str());
}

extern "C"
JNIEXPORT jboolean JNICALL
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