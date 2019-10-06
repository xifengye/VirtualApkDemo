package com.moregood.virtualapkdemo;

import android.content.Context;
import android.content.Intent;
import android.os.Environment;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.view.View;
import android.widget.Toast;

import com.didi.virtualapk.PluginManager;

import java.io.File;

public class MainActivity extends AppCompatActivity {

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        loadPlugin(getApplication());
    }

    private void loadPlugin(Context base) {
        PluginManager pluginManager = PluginManager.getInstance(base);
        File apk = new File( "/sdcard/content.apk");
        if (apk.exists()) {
            try {
                pluginManager.loadPlugin(apk);
            } catch (Exception e) {
                e.printStackTrace();
            }
        } else {
            Toast.makeText(getApplicationContext(),
                    "SDcard根目录未检测到content.apk插件", Toast.LENGTH_LONG).show();
        }
    }

    public void onBtnClicked(View view){
        Intent intent = new Intent();
        intent.setClassName("com.moregood.content","com.moregood.content.ContentMainActivity");
        startActivity(intent);
    }
}
