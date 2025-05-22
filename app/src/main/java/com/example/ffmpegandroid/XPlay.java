package com.example.ffmpegandroid;

import android.annotation.SuppressLint;
import android.content.Context;
import android.graphics.SurfaceTexture;
import android.util.AttributeSet;
import android.util.Log;
import android.view.MotionEvent;
import android.view.Surface;
import android.view.TextureView;
import android.view.View;

import androidx.annotation.NonNull;

public class XPlay extends TextureView implements TextureView.SurfaceTextureListener, View.OnClickListener {

    public XPlay(Context context) {
        super(context);
        setSurfaceTextureListener(this);
        setOnClickListener(this);
    }

    public XPlay(Context context, AttributeSet attrs) {
        super(context, attrs);
        setSurfaceTextureListener(this);
        setOnClickListener(this);
    }

    @Override
    public void onSurfaceTextureAvailable(@NonNull SurfaceTexture surface, int width, int height) {
        // 初始化opengl egl显示
        this.InitView(new Surface(surface));
    }

    @Override
    public void onSurfaceTextureSizeChanged(@NonNull SurfaceTexture surface, int width, int height) {

    }

    @Override
    public boolean onSurfaceTextureDestroyed(@NonNull SurfaceTexture surface) {
        return false;
    }

    @Override
    public void onSurfaceTextureUpdated(@NonNull SurfaceTexture surface) {

    }

    @Override
    public void onClick(View v) {
        PlayOrPause();
    }

    public native void Open(String url, Object surface);

    public native void OpenAudio();

    public native void OpenYuv(String url, Object surface);

    public native void Test();

    public native void InitView(Object surface);
    public native void PlayOrPause();
}
