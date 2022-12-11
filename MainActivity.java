package com.wz.melodymemorize;

import android.app.Activity;
import android.os.Bundle;
import android.os.Handler;
import android.os.Message;
import android.view.View;
import android.widget.Button;
import android.widget.TextView;
import android.widget.Toast;

import com.wz.jnidriver.JNIDriver;
import com.wz.jnidriver.JNIListener;

public class MainActivity extends Activity implements JNIListener {
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
    }
    
    @Override
    public void onPause() {
        driver.close();
        super.onPause();
    }
    
    @Override
    protected void onResume() {
        if (driver.open() < 0) {
            // TODO: 실패 문구 출력
        }
        super.onResume();
    }
}