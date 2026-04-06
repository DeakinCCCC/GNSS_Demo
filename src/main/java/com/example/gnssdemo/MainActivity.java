package com.example.gnssdemo;

import android.os.Bundle;
import android.view.View;
import android.widget.Button;
import android.widget.Toast;
import androidx.appcompat.app.AppCompatActivity;

import com.chaquo.python.Python;
import com.chaquo.python.android.AndroidPlatform;
import com.chaquo.python.PyObject;

public class MainActivity extends AppCompatActivity {
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        Button button = findViewById(R.id.button);
        TextView textView = findViewById(R.id.mytext);
		if (!Python.isStarted()) {
			Python.start(new AndroidPlatform(this));
		}
        button.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
				RtkLibWrapper rtkW = new RtkLibWrapper();
				String testStr = rtkW.test();
				textView.setText(testStr);
				Python py = Python.getInstance();
				PyObject module = py.getModule("rtk_demo");
				PyObject result = module.callAttr("rtk_test", 5, 3);
				int sum = result.toInt();
                Toast.makeText(MainActivity.this, "Hello, Android! Result is " + sum , Toast.LENGTH_SHORT).show();
            }
        });
    }
}
