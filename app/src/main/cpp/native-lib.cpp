#include <jni.h>
#include <string>
#include <android/log.h>
#include <android/native_window.h>
#include <android/native_window_jni.h>
#include <SLES/OpenSLES.h>
#include <SLES/OpenSLES_Android.h>
#include <EGL/egl.h>
#include <GLES2/gl2.h>

#define LOGW(...) __android_log_print(ANDROID_LOG_WARN, "testff", __VA_ARGS__)

extern "C" {
#include <libavcodec/jni.h>
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libswscale/swscale.h>
#include <libswresample/swresample.h>
}

static double r2d(AVRational r) {
    if (r.num == 0 || r.den == 0) return 0.;
    return static_cast<double>(r.num) / static_cast<double>(r.den);
}

// 当前时间戳
long long GetNowMs() {
    struct timeval tv{};
    gettimeofday(&tv, nullptr);
    int sec = tv.tv_sec % 360000;
    long long t = sec * 1000 + tv.tv_usec / 1000;
    return t;
}

//extern "C"
//JNIEXPORT jint JNI_OnLoad(JavaVM *vm, void *res) {
//    av_jni_set_java_vm(vm, nullptr);
//    return JNI_VERSION_1_4;
//}

extern "C" JNIEXPORT jstring JNICALL
Java_com_example_ffmpegandroid_MainActivity_stringFromJNI(
        JNIEnv *env,
        jobject /* this */) {
    std::string hello = "Hello from C++ : ";
    hello += avcodec_configuration();
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

extern "C"
JNIEXPORT void JNICALL
Java_com_example_ffmpegandroid_XPlay_Open(JNIEnv *env, jobject thiz, jstring url, jobject surface) {

    const char *path = env->GetStringUTFChars(url, nullptr);

    // 初始化解封装
    av_register_all();
    // 初始化网络
    avformat_network_init();
    // 初始化解码器
    avcodec_register_all();
    // 打开文件
    AVFormatContext *ic = nullptr;

    int re = avformat_open_input(&ic, path, nullptr, nullptr);
    if (re != 0) {
        LOGW("avformat_open_input failed!: %s", av_err2str(re));
        return;
    }
    LOGW("avformat_open_input %s success!", path);
    // 获取流信息 （通过读取部分流数据进行探测，用于没有文件头的情况）
    re = avformat_find_stream_info(ic, nullptr);
    if (re != 0) {
        LOGW("avformat_find_stream_info failed!: %s", av_err2str(re));
        return;
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
    // *******************************************************************************
    // 取出解码器 视频解码器
    AVCodec *codec = nullptr;
    // 软解码
    codec = avcodec_find_decoder(ic->streams[videoStream]->codecpar->codec_id);
    // 硬解码
//    codec = avcodec_find_decoder_by_name("h264_mediacodec");
    if (!codec) {
        LOGW("avcodec_find_decoder failed!");
        return;
    }
    // 解码器初始化
    AVCodecContext *vc = avcodec_alloc_context3(codec);
    vc->thread_count = 20;
    avcodec_parameters_to_context(vc, ic->streams[videoStream]->codecpar);
    // 打开解码器
    re = avcodec_open2(vc, nullptr, nullptr);
    if (re != 0) {
        LOGW("avcodec_open2 video failed!");
        return;
    }
    // ************************************************************************************
    // 取出解码器 音频解码器
    AVCodec *acodec = nullptr;
    // 软解码
    acodec = avcodec_find_decoder(ic->streams[audioStream]->codecpar->codec_id);
    // 硬解码
//    acodec = avcodec_find_decoder_by_name("h264_mediacodec");
    if (!acodec) {
        LOGW("avcodec_find_decoder failed!");
        return;
    }
    // 解码器初始化
    AVCodecContext *ac = avcodec_alloc_context3(acodec);
    ac->thread_count = 20;
    avcodec_parameters_to_context(ac, ic->streams[audioStream]->codecpar);
    // 打开解码器
    re = avcodec_open2(ac, nullptr, nullptr);
    if (re != 0) {
        LOGW("avcodec_open2 audio failed!");
        return;
    }

    // 初始化像素格式转换的上下文
    SwsContext *vctx = nullptr;
    int outWidth = 1280, outHeight = 720;
//    int outWidth = 1920, outHeight = 1080;
    char *rgb = new char[1920 * 1080 * 4];
    char *pcm = new char[48000 * 4 * 2];

    // 读取帧数据
    AVPacket *pkt = av_packet_alloc();
    AVFrame *frame = av_frame_alloc();
    long long start = GetNowMs();
    int frameCount = 0;

    // 显示窗口初始化
    ANativeWindow *nwin = ANativeWindow_fromSurface(env, surface);
    ANativeWindow_setBuffersGeometry(nwin, outWidth, outHeight, WINDOW_FORMAT_RGBA_8888);
    ANativeWindow_Buffer wbuf;
    for (;;) {

        // 超过3秒
        if (GetNowMs() - start >= 3000) {
            LOGW("now decode fps is %d", frameCount / 3);
            start = GetNowMs();
            frameCount = 0;
        }

        int re = av_read_frame(ic, pkt);

        if (re != 0) {
            LOGW("读取到结尾处!");
            break;
//            int pos = static_cast<int>(70 * r2d(ic->streams[videoStream]->time_base));
//            av_seek_frame(ic, videoStream, pos, AVSEEK_FLAG_BACKWARD | AVSEEK_FLAG_FRAME);
//            continue;
        }
//        LOGW("stream = %d, size = %d, pts = %lld, flag = %d",
//             pkt->stream_index, pkt->size, pkt->pts, pkt->flags);

        // 解码
        AVCodecContext *cc = pkt->stream_index == audioStream ? ac : vc;
        // 发送到线程中解码
        re = avcodec_send_packet(cc, pkt);
        // 清理
        av_packet_unref(pkt);
        if (re != 0) {
            LOGW("avcodec_send_packet failed!");
            continue;
        }

        // 音频重采样上下文初始化
        SwrContext *actx = swr_alloc();
        actx = swr_alloc_set_opts(actx, av_get_default_channel_layout(2),
                                  AV_SAMPLE_FMT_S16, ac->sample_rate,
                                  av_get_default_channel_layout(ac->channels),
                                  ac->sample_fmt, ac->sample_rate,
                                  0, nullptr);
        re = swr_init(actx);
        if (re != 0) {
            LOGW("swr_init failed!");
        } else {
//            LOGW("swr_init success!");
        }

        for (;;) {
            re = avcodec_receive_frame(cc, frame);
            if (re != 0) {
                // LOGW("avcodec_receive_frame failed!");
                break;
            }

//            LOGW("avcodec_receive_frame %lld", frame->pts);
            // 记录视频帧数量
            if (cc == vc) {
                ++frameCount;
                vctx = sws_getCachedContext(vctx, frame->width, frame->height,
                                            static_cast<AVPixelFormat>(frame->format),
                                            outWidth, outHeight, AV_PIX_FMT_RGBA,
                                            SWS_FAST_BILINEAR, nullptr, nullptr, nullptr);
                if (!vctx) {
                    LOGW("sws_getCachedContext failed!");
                } else {
                    uint8_t *data[AV_NUM_DATA_POINTERS] = {0};
                    data[0] = reinterpret_cast<uint8_t *>(rgb);
                    int lines[AV_NUM_DATA_POINTERS] = {0};
                    lines[0] = outWidth * 4;
                    int h = sws_scale(vctx, frame->data, frame->linesize, 0, frame->height,
                                      data, lines);
//                    LOGW("sws_scale = %d", h);
                    if (h > 0) {
                        ANativeWindow_lock(nwin, &wbuf, nullptr);
                        auto *dst = reinterpret_cast<uint8_t *>(wbuf.bits);
                        memcpy(dst, rgb, outWidth * outHeight * 4);
                        ANativeWindow_unlockAndPost(nwin);
                        LOGW("ANativeWindow");
                    }
                }
            } else { // 音频帧
                uint8_t *out[2] = {0};
                out[0] = reinterpret_cast<uint8_t *>(pcm);
                // 音频重采样
                int len = swr_convert(actx, out, frame->nb_samples,
                                      reinterpret_cast<const uint8_t **>(&frame->data),
                                      frame->nb_samples);
//                LOGW("swr_convert = %d", len);
            }
        }
    }

    delete[] rgb;
    delete[] pcm;
    // 关闭文件
    avformat_close_input(&ic);

    env->ReleaseStringUTFChars(url, path);

}

static SLObjectItf enginSL = nullptr;

SLEngineItf CreateSL() {
    SLresult re;
    SLEngineItf en;
    re = slCreateEngine(&enginSL, 0, 0, 0, 0, 0);
    if (re != SL_RESULT_SUCCESS) return nullptr;
    re = (*enginSL)->Realize(enginSL, SL_BOOLEAN_FALSE);
    if (re != SL_RESULT_SUCCESS) return nullptr;
    re = (*enginSL)->GetInterface(enginSL, SL_IID_ENGINE, &en);
    if (re != SL_RESULT_SUCCESS) return nullptr;
    return en;
}

void PcmCall(SLAndroidSimpleBufferQueueItf bf, void *contex) {
    static FILE *fp = nullptr;
    static char *buf = nullptr;
    if (!buf) {
        buf = new char[1024 * 1024];
    }
    if (!fp) {
        fp = fopen("/sdcard/test.pcm", "rb");
    }
    if (!fp) {
        LOGW("fopen failed!");
        return;
    }
    if (feof(fp) == 0) {
        int len = fread(buf, 1, 1024, fp);
        if (len > 0) {
            (*bf)->Enqueue(bf, buf, len);
        }
    }
}

extern "C"
JNIEXPORT void JNICALL
Java_com_example_ffmpegandroid_XPlay_OpenAudio(JNIEnv *env, jobject thiz) {
    // 创建引擎
    SLEngineItf eng = CreateSL();
    if (eng) {
        LOGW("CreateSL success!");
    } else {
        LOGW("CreateSL failed!");
        return;
    }
    // 创建混音器
    SLObjectItf mix = nullptr;
    SLresult re;
    re = (*eng)->CreateOutputMix(eng, &mix, 0, 0, 0);
    if (re != SL_RESULT_SUCCESS) {
        LOGW("CreateOutputMix failed!");
        return;
    }
    re = (*mix)->Realize(mix, SL_BOOLEAN_FALSE);
    if (re != SL_RESULT_SUCCESS) {
        LOGW("Realize failed!");
        return;
    }
    SLDataLocator_OutputMix outmix = {SL_DATALOCATOR_OUTPUTMIX, mix};
    SLDataSink audioSink = {&outmix, 0};
    // 配置音频信息
    // 缓冲队列
    SLDataLocator_AndroidSimpleBufferQueue que = {SL_DATALOCATOR_ANDROIDSIMPLEBUFFERQUEUE, 10};
    // 音频格式
    SLDataFormat_PCM pcm = {
            SL_DATAFORMAT_PCM,
            2, // 声道数
            SL_SAMPLINGRATE_44_1,
            SL_PCMSAMPLEFORMAT_FIXED_16,
            SL_PCMSAMPLEFORMAT_FIXED_16,
            SL_SPEAKER_FRONT_LEFT | SL_SPEAKER_FRONT_RIGHT,
            SL_BYTEORDER_LITTLEENDIAN // 小端字节序
    };
    SLDataSource ds = {&que, &pcm};
    // 创建播放器
    SLObjectItf player = nullptr;
    SLPlayItf iplayer = nullptr;
    SLAndroidSimpleBufferQueueItf pcmQue = nullptr;
    const SLInterfaceID ids[] = {SL_IID_BUFFERQUEUE};
    const SLboolean req[] = {SL_BOOLEAN_TRUE};
    re = (*eng)->CreateAudioPlayer(eng, &player, &ds, &audioSink,
                                   sizeof(ids) / sizeof(SLInterfaceID), ids, req);
    if (re == SL_RESULT_SUCCESS) {
        LOGW("CreateAudioPlayer success!");
    } else {
        LOGW("CreateAudioPlayer failed!");
        return;
    }
    (*player)->Realize(player, SL_BOOLEAN_FALSE);
    // 获取接口
    re = (*player)->GetInterface(player, SL_IID_PLAY, &iplayer);
    if (re != SL_RESULT_SUCCESS) {
        LOGW("GetInterface SL_IID_PLAY failed!");
        return;
    }
    re = (*player)->GetInterface(player, SL_IID_BUFFERQUEUE, &pcmQue);
    if (re != SL_RESULT_SUCCESS) {
        LOGW("GetInterface SL_IID_BUFFERQUEUE failed!");
        return;
    }
    // 设置回调函数 播放队列空的时候调用
    (*pcmQue)->RegisterCallback(pcmQue, PcmCall, 0);
    // 设置为播放状态
    (*iplayer)->SetPlayState(iplayer, SL_PLAYSTATE_PLAYING);
    // 启动队列回调
    (*pcmQue)->Enqueue(pcmQue, "", 1);
}

// 顶点着色器
#define GET_STR(x) #x
static const char *vertexShader = GET_STR(
        attribute vec4 aPosition; // 顶点坐标
        attribute vec2 aTexCoord; // 材质顶点坐标
        varying vec2 vTexCoord;   // 输出的材质坐标

        void main() {
            vTexCoord = vec2(aTexCoord.x, 1.f - aTexCoord.y);
            gl_Position = aPosition;
        }
);

// 片元着色器 软解码和部分x86硬解码
static const char *fragmentShaderYUV420P = GET_STR(
        precision mediump float; // 精度
        varying vec2 vTexCoord;  // 顶点着色器传递的坐标
        uniform sampler2D yTexture; // 输入的材质 (灰度)
        uniform sampler2D uTexture;
        uniform sampler2D vTexture;

        void main() {
            vec3 yuv;
            vec3 rgb;
            yuv.r = texture2D(yTexture, vTexCoord).r;
            yuv.g = texture2D(uTexture, vTexCoord).r - 0.5;
            yuv.b = texture2D(vTexture, vTexCoord).r - 0.5;
            rgb = mat3(1.f, 1.f, 1.f,
                       0.f, -0.39465f, 2.03211f,
                       1.13983f, -0.5806f, 0.f) * yuv;
            gl_FragColor = vec4(rgb.rgb, 1.f);
        }

);

GLuint InitShader(const char *code, GLint type) {
    // 创建shader
    GLuint sh = glCreateShader(type);
    if (sh == 0) {
        LOGW("glCreateShader %d failed!", type);
        return 0;
    }
    // 加载shader
    glShaderSource(sh, 1, &code, 0); // 代码长度
    // 编译shader
    glCompileShader(sh);
    // 获取编译情况
    GLint status;
    glGetShaderiv(sh, GL_COMPILE_STATUS, &status);
    if (status == 0) {
        LOGW("glCompileShader failed!");
        return 0;
    }
    LOGW("glCompileShader success!");
    return sh;
}

extern "C"
JNIEXPORT void JNICALL
Java_com_example_ffmpegandroid_XPlay_OpenYuv(JNIEnv *env, jobject thiz, jstring url,
                                             jobject surface) {
    const char *path = env->GetStringUTFChars(url, 0);
    LOGW("open url is %s", path);

    FILE *fp = fopen(path, "rb");
    if (!fp) {
        LOGW("open file failed!");
        return;
    }

    // 获取原始窗口
    ANativeWindow *nwin = ANativeWindow_fromSurface(env, surface);
    // 创建EGL
    // display创建和初始化
    EGLDisplay display = eglGetDisplay(EGL_DEFAULT_DISPLAY);
    if (display == EGL_NO_DISPLAY) {
        LOGW("eglGetDisplay failed!");
        return;
    }
    if (EGL_TRUE != eglInitialize(display, 0, 0)) {
        LOGW("eglInitialize failed!");
        return;
    }
    // 创建surface
    // surface窗口配置
    // 输出配置
    EGLConfig config;
    EGLint configNum;
    EGLint configSpec[] = {
            EGL_RED_SIZE, 8,
            EGL_GREEN_SIZE, 8,
            EGL_BLUE_SIZE, 8,
            EGL_SURFACE_TYPE, EGL_WINDOW_BIT, EGL_NONE
    };
    if (EGL_TRUE != eglChooseConfig(display, configSpec, &config, 1, &configNum)) {
        LOGW("eglChooseConfig failed!");
        return;
    }
    EGLSurface winSurface = eglCreateWindowSurface(display, config, nwin, 0);
    if (winSurface == EGL_NO_SURFACE) {
        LOGW("eglCreateWindowSurface failed!");
        return;
    }
    // 创建关联的上下文 context
    const EGLint ctxAttr[] = {
            EGL_CONTEXT_CLIENT_VERSION, 2, EGL_NONE
    };
    EGLContext context = eglCreateContext(display, config, EGL_NO_CONTEXT, ctxAttr);
    if (context == EGL_NO_CONTEXT) {
        LOGW("eglCreateContext failed!");
        return;
    }
    if (EGL_TRUE != eglMakeCurrent(display, winSurface, winSurface, context)) {
        LOGW("eglMakeCurrent failed!");
        return;
    }
    LOGW("EGL Init Success!");
    // shader初始化
    GLuint vsh = InitShader(vertexShader, GL_VERTEX_SHADER);
    GLuint fsh = InitShader(fragmentShaderYUV420P, GL_FRAGMENT_SHADER);
    // 创建渲染程序
    GLuint program = glCreateProgram();
    if (program == 0) {
        LOGW("glCreateProgram failed!");
        return;
    }
    // 向渲染程序中加入shader
    glAttachShader(program, vsh);
    glAttachShader(program, fsh);
    // 链接
    glLinkProgram(program);
    GLint status;
    glGetProgramiv(program, GL_LINK_STATUS, &status);
    if (status != GL_TRUE) {
        LOGW("glLinkProgram failed!");
        return;
    }
    glUseProgram(program);
    LOGW("glLinkProgram success!");
    // 加入顶点数据
    static GLfloat vers[] = {
            1.f, -1.f, 0.f,
            -1.f, -1.f, 0.f,
            1.f, 1.f, 0.f,
            -1.f, 1.f, 0.f
    };
    GLint apos = glGetAttribLocation(program, "aPosition");
    glEnableVertexAttribArray(apos);
    // 传递顶点
    glVertexAttribPointer(apos, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), vers);
    // 加入材质坐标数据
    static GLfloat txts[] = {
            1.f, 0.f,
            0.f, 0.f,
            1.f, 1.f,
            0.f, 1.f
    };
    GLint atex = glGetAttribLocation(program, "aTexCoord");
    glEnableVertexAttribArray(atex);
    // 传递顶点
    glVertexAttribPointer(atex, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), txts);

    int width = 424, height = 240;

    // 材质纹理初始化
    // 设置纹理层
    glUniform1i(glGetUniformLocation(program, "yTexture"), 0); // 纹理第一层
    glUniform1i(glGetUniformLocation(program, "uTexture"), 1); // 纹理第二层
    glUniform1i(glGetUniformLocation(program, "vTexture"), 2); // 纹理第三层
    // 创建opengl材质
    GLuint texts[3] = {0};
    glGenTextures(3, texts);
    // 设置纹理属性
    glBindTexture(GL_TEXTURE_2D, texts[0]);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    // 设置纹理的格式和大小
    glTexImage2D(GL_TEXTURE_2D, 0, /* 灰度图 */ GL_LUMINANCE, width, height, 0, GL_LUMINANCE,
                 GL_UNSIGNED_BYTE,
                 nullptr);

    // 设置纹理属性
    glBindTexture(GL_TEXTURE_2D, texts[1]);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    // 设置纹理的格式和大小
    glTexImage2D(GL_TEXTURE_2D, 0, /* 灰度图 */ GL_LUMINANCE, width / 2, height / 2, 0,
                 GL_LUMINANCE, GL_UNSIGNED_BYTE,
                 nullptr);

    // 设置纹理属性
    glBindTexture(GL_TEXTURE_2D, texts[2]);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    // 设置纹理的格式和大小
    glTexImage2D(GL_TEXTURE_2D, 0, /* 灰度图 */ GL_LUMINANCE, width / 2, height / 2, 0,
                 GL_LUMINANCE, GL_UNSIGNED_BYTE,
                 nullptr);

    // 纹理的修改和显示
    unsigned char *buf[3] = {0};
    buf[0] = new unsigned char[width * height];
    buf[1] = new unsigned char[width * height / 4];
    buf[2] = new unsigned char[width * height / 4];

    for (;;) {

        // 420P
        if (feof(fp) == 0) {
            fread(buf[0], 1, width * height, fp);
            fread(buf[1], 1, width * height / 4, fp);
            fread(buf[2], 1, width * height / 4, fp);
        } else {
            break;
        }

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texts[0]);
        // 替换纹理内容
        glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, width, height, GL_LUMINANCE, GL_UNSIGNED_BYTE,
                        buf[0]);

        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, texts[1]);
        // 替换纹理内容
        glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, width / 2, height / 2, GL_LUMINANCE,
                        GL_UNSIGNED_BYTE, buf[1]);

        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D, texts[2]);
        // 替换纹理内容
        glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, width / 2, height / 2, GL_LUMINANCE,
                        GL_UNSIGNED_BYTE, buf[2]);

        // 三维绘制
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
        // 窗口显示
        eglSwapBuffers(display, winSurface);
        LOGW("glDrawArrays success!");
    }
    env->ReleaseStringUTFChars(url, path);
}


#include "IPlayerPorxy.h"

extern "C"
JNIEXPORT jint JNI_OnLoad(JavaVM *vm, void *res) {
    IPlayerPorxy::Get()->Init(vm);
    return JNI_VERSION_1_4;
}

extern "C"
JNIEXPORT void JNICALL
Java_com_example_ffmpegandroid_XPlay_Test(JNIEnv *env, jobject thiz) {
//    IPlayerPorxy::Get()->Open("/sdcard/1080.mp4");
//    IPlayerPorxy::Get()->Start();
//    XSleep(5000);
//    IPlayerPorxy::Get()->Open("/sdcard/test1.mp4");
//    IPlayerPorxy::Get()->Start();
}

extern "C"
JNIEXPORT void JNICALL
Java_com_example_ffmpegandroid_XPlay_InitView(JNIEnv *env, jobject thiz, jobject surface) {
    ANativeWindow *win = ANativeWindow_fromSurface(env, surface);
    IPlayerPorxy::Get()->InitView(win);
}

extern "C"
JNIEXPORT void JNICALL
Java_com_example_ffmpegandroid_OpenUrl_Open(JNIEnv *env, jobject thiz, jstring url) {
    const char *path = env->GetStringUTFChars(url, 0);

    IPlayerPorxy::Get()->Open(path);
    IPlayerPorxy::Get()->Start();

    env->ReleaseStringUTFChars(url, path);
}

extern "C"
JNIEXPORT jdouble JNICALL
Java_com_example_ffmpegandroid_MainActivity_PlayPos(JNIEnv *env, jobject thiz) {
    return IPlayerPorxy::Get()->PlayPos();
}

extern "C"
JNIEXPORT void JNICALL
Java_com_example_ffmpegandroid_MainActivity_Seek(JNIEnv *env, jobject thiz, jdouble pos) {
    IPlayerPorxy::Get()->Seek(pos);
}
extern "C"
JNIEXPORT void JNICALL
Java_com_example_ffmpegandroid_XPlay_PlayOrPause(JNIEnv *env, jobject thiz) {
    IPlayerPorxy::Get()->SetPause(!IPlayerPorxy::Get()->IsPause());
}