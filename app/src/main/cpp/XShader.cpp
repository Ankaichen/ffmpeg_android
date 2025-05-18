//
// Created by 27515 on 2025-05-18.
//

#include "XShader.h"
#include "XLog.h"

#include <GLES2/gl2.h>

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

static GLuint InitShader(const char *code, GLint type) {
    // 创建shader
    GLuint sh = glCreateShader(type);
    if (sh == 0) {
        XLOGE("glCreateShader %d failed!", type);
        return 0;
    }
    XLOGI("glCreateShader %d success!", type);
    // 加载shader
    glShaderSource(sh, 1, &code, nullptr); // 代码长度
    // 编译shader
    glCompileShader(sh);
    // 获取编译情况
    GLint status;
    glGetShaderiv(sh, GL_COMPILE_STATUS, &status);
    if (status == 0) {
        XLOGE("glCompileShader failed!");
        return 0;
    }
    XLOGI("glCompileShader success!");
    return sh;
}

bool XShader::Init() {
    // shader初始化
    this->vsh = InitShader(vertexShader, GL_VERTEX_SHADER);
    if (this->vsh == 0) {
        XLOGE("InitShader GL_VERTEX_SHADER failed!");
        return false;
    }
    XLOGI("InitShader GL_VERTEX_SHADER success!");
    this->fsh = InitShader(fragmentShaderYUV420P, GL_FRAGMENT_SHADER);
    if (this->fsh == 0) {
        XLOGE("InitShader GL_FRAGMENT_SHADER failed!");
        return false;
    }
    XLOGI("InitShader GL_FRAGMENT_SHADER success!");
    // 创建渲染程序
    this->program = glCreateProgram();
    if (this->program == 0) {
        XLOGE("glCreateProgram failed!");
        return false;
    }
    XLOGI("glCreateProgram success!");
    // 向渲染程序中加入shader
    glAttachShader(this->program, this->vsh);
    glAttachShader(this->program, this->fsh);
    // 链接
    glLinkProgram(this->program);
    GLint status;
    glGetProgramiv(this->program, GL_LINK_STATUS, &status);
    if (status != GL_TRUE) {
        XLOGE("glLinkProgram failed!");
        return false;
    }
    XLOGI("glLinkProgram success!");
    glUseProgram(this->program);
    // 加入顶点数据
    static GLfloat vers[] = {
            1.f, -1.f, 0.f,
            -1.f, -1.f, 0.f,
            1.f, 1.f, 0.f,
            -1.f, 1.f, 0.f
    };
    GLint apos = glGetAttribLocation(this->program, "aPosition");
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
    GLint atex = glGetAttribLocation(this->program, "aTexCoord");
    glEnableVertexAttribArray(atex);
    // 传递顶点
    glVertexAttribPointer(atex, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), txts);

    // 材质纹理初始化
    // 设置纹理层
    glUniform1i(glGetUniformLocation(this->program, "yTexture"), 0); // 纹理第一层
    glUniform1i(glGetUniformLocation(this->program, "uTexture"), 1); // 纹理第二层
    glUniform1i(glGetUniformLocation(this->program, "vTexture"), 2); // 纹理第三层
    XLOGI("XShader Init success!");
    return true;
}

void XShader::GetTexture(unsigned int index, int width, int height, unsigned char *buf) {
    if (this->texts[index] == 0) {
        // 材质初始化
        glGenTextures(1, &(this->texts[index]));
        // 设置纹理属性
        glBindTexture(GL_TEXTURE_2D, this->texts[index]);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        // 设置纹理的格式和大小
        glTexImage2D(GL_TEXTURE_2D, 0, /* 灰度图 */ GL_LUMINANCE, width, height, 0, GL_LUMINANCE,
                     GL_UNSIGNED_BYTE, nullptr);
    }
    glActiveTexture(GL_TEXTURE0 + index);
    glBindTexture(GL_TEXTURE_2D, this->texts[index]);
    // 替换纹理内容
    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, width, height,
                    GL_LUMINANCE, GL_UNSIGNED_BYTE, buf);
}

void XShader::Draw() {
    if (!this->program) return;
    // 三维绘制
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
}
