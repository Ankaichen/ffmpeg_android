package com.example.ffmpegandroid;

import android.os.Bundle;
import android.view.View;

import androidx.annotation.Nullable;
import androidx.appcompat.app.AppCompatActivity;

import com.example.ffmpegandroid.databinding.OpenurlBinding;

public class OpenUrl extends AppCompatActivity {

    private OpenurlBinding binding;

    @Override
    protected void onCreate(@Nullable Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.openurl);

        binding = OpenurlBinding.inflate(getLayoutInflater());
        setContentView(binding.getRoot());

        binding.playvideo.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                Open(binding.fileurl.getText().toString());
                finish(); // 关闭窗口
            }
        });

        binding.playrtmp.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                Open(binding.rtmpurl.getText().toString());
                finish(); // 关闭窗口
            }
        });
    }

    public native void Open(String url);
}
