package com.example.ffmpegandroid;

import androidx.annotation.NonNull;
import androidx.appcompat.app.AppCompatActivity;
import androidx.core.app.ActivityCompat;

import android.os.Bundle;
import android.widget.TextView;
import android.Manifest;

import com.example.ffmpegandroid.databinding.ActivityMainBinding;

public class MainActivity extends AppCompatActivity {

    // Used to load the 'ffmpeg_android' library on application startup.
    static {
        System.loadLibrary("ffmpeg_android");
    }

    private ActivityMainBinding binding;

    private static final String[] PERMISSIONS = new String[]{
            Manifest.permission.WRITE_EXTERNAL_STORAGE,
    };
    private static final int REQUEST_CODE = 1;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        binding = ActivityMainBinding.inflate(getLayoutInflater());
        setContentView(binding.getRoot());

        ActivityCompat.requestPermissions(this, PERMISSIONS, REQUEST_CODE);

        TextView tv = binding.sampleText;
        tv.setText(stringFromJNI());
    }

    @Override
    public void onRequestPermissionsResult(int requestCode, @NonNull String[] permissions, @NonNull int[] grantResults) {
        super.onRequestPermissionsResult(requestCode, permissions, grantResults);
//        Open("/sdcard/test1.mp4", this);
    }

    /**
     * A native method that is implemented by the 'ffmpeg_android' native library,
     * which is packaged with this application.
     */
    public native String stringFromJNI();

    public native boolean Open(String url, Object handle);
}