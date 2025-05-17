package com.example.ffmpegandroid;

import android.content.Context;
import android.opengl.GLSurfaceView;
import android.util.AttributeSet;
import android.view.SurfaceHolder;
import android.view.SurfaceView;

import androidx.annotation.NonNull;

public class XPlay extends SurfaceView implements Runnable, SurfaceHolder.Callback {

    public XPlay(Context context) {
        super(context);
        getHolder().addCallback(this);
    }

    public XPlay(Context context, AttributeSet attrs) {
        super(context, attrs);
        getHolder().addCallback(this);
    }

    @Override
    public void run() {
//        this.Open("/sdcard/1080.mp4", getHolder().getSurface());
//        this.Open("/sdcard/test1.mp4", getHolder().getSurface());
//        this.Open("/sdcard/test2.flv", getHolder().getSurface());
//        this.Open("/sdcard/test3.mp4", getHolder().getSurface());
//        this.OpenAudio();
//        this.OpenYuv("/sdcard/test.yuv", getHolder().getSurface());
        this.Test();
    }

    @Override
    public void surfaceCreated(@NonNull SurfaceHolder holder) {
        new Thread(this).start();
    }

    @Override
    public void surfaceChanged(@NonNull SurfaceHolder holder, int format, int w, int h) {
    }

    @Override
    public void surfaceDestroyed(@NonNull SurfaceHolder holder) {
    }

    public native void Open(String url, Object surface);

    public native void OpenAudio();

    public native void OpenYuv(String url, Object surface);

    public native void Test();
}
