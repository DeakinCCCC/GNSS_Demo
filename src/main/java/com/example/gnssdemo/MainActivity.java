package com.example.gnssdemo;

import android.Manifest;
import android.content.Context;
import android.content.pm.PackageManager;
import androidx.annotation.NonNull;
import androidx.core.app.ActivityCompat;

import android.location.GnssMeasurement;
import android.location.GnssMeasurementsEvent;
import android.location.LocationManager;

import android.os.Bundle;
import android.view.View;
import android.widget.Button;
import android.widget.TextView;
import android.widget.Toast;
import androidx.appcompat.app.AppCompatActivity;

import com.chaquo.python.Python;
import com.chaquo.python.android.AndroidPlatform;
import com.chaquo.python.PyObject;

public class MainActivity extends AppCompatActivity {
	private static MainActivity instance;
    private LocationManager locationManager;
    private TextView logView;
    private TextView statusView;

	// 回调对象：处理收到的原始观测数据
    private final GnssMeasurementsEvent.Callback measurementCallback = 
        new GnssMeasurementsEvent.Callback() {
            @Override
            public void onGnssMeasurementsReceived(GnssMeasurementsEvent event) {
                StringBuilder sb = new StringBuilder();
                for (GnssMeasurement m : event.getMeasurements()) {
					double adrMeters = m.getAccumulatedDeltaRangeMeters();
					int adrState = m.getAccumulatedDeltaRangeState();
					String adr = "";
					
					if ((adrState & GnssMeasurement.ADR_STATE_VALID) != 0) {
					    if ((adrState & GnssMeasurement.ADR_STATE_CYCLE_SLIP) != 0) {
					        // 发现周跳，需要重新初始化滤波器
					        adr = "ADR: CYCLE_SLIP";
					    } else {
					        // 数据可靠，可用于高精度解算
					        double sigma = m.getAccumulatedDeltaRangeUncertaintyMeters(); // 精度标准差
					        adr = String.format("ADR: %.4f m (±%.4f)", adrMeters, sigma);
					    }
					} else {
					    logView.append("ADR 无效\n");
					}

                    // 格式化输出：系统ID, 卫星号, 载噪比, 伪距率
                    sb.append(String.format("Const:%d | SVID:%3d | CN0:%.1f | PRate:%.1f | %s\n",
                            m.getConstellationType(),
                            m.getSvid(),
                            m.getCn0DbHz(),
                            m.getPseudorangeRateMetersPerSecond(),
							adr));
                }
                runOnUiThread(() -> {
                    logView.setText(sb.toString()); // 更新当前所有可见卫星数据
                    statusView.setText("当前可见卫星数: " + event.getMeasurements().size());
                });
            }

			/**
            @Override
            public void onStatusChanged(int status) {
                // 处理 GNSS 状态变化（如不支持原始数据输出）
            }
			**/
        };
	
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
		instance = this;

		logView = findViewById(R.id.log_view);
        statusView = findViewById(R.id.status_text);
        locationManager = (LocationManager) getSystemService(Context.LOCATION_SERVICE);
		
        Button button = findViewById(R.id.button);
        TextView textView = findViewById(R.id.mytext);
		if (!Python.isStarted()) {
			Python.start(new AndroidPlatform(this));
		}
		
        button.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
				RtkLibWrapper rtkW = new RtkLibWrapper(instance);
				String testStr = rtkW.test();
				textView.setText(testStr);
				Python py = Python.getInstance();
				PyObject module = py.getModule("rtk_demo");
				PyObject result = module.callAttr("rtk_test", 5, 3);
				int sum = result.toInt();
                Toast.makeText(MainActivity.this, "Hello, Android! Result is " + sum , Toast.LENGTH_SHORT).show();
            }
        });
		checkPermissionsAndStart();
    }
	
	private void checkPermissionsAndStart() {
        if (ActivityCompat.checkSelfPermission(this, Manifest.permission.ACCESS_FINE_LOCATION) 
                != PackageManager.PERMISSION_GRANTED) {
            ActivityCompat.requestPermissions(this, 
                    new String[]{Manifest.permission.ACCESS_FINE_LOCATION}, 100);
        } else {
            startGnssTracking();
        }
    }

	private void startGnssTracking() {
        try {
            locationManager.registerGnssMeasurementsCallback(measurementCallback);
        } catch (SecurityException e) {
            statusView.setText("权限不足");
        }
    }

    @Override
    protected void onDestroy() {
        super.onDestroy();
        // 记得注销监听，否则会导致内存泄漏和耗电
        locationManager.unregisterGnssMeasurementsCallback(measurementCallback);
    }

    @Override
    public void onRequestPermissionsResult(int requestCode, @NonNull String[] permissions, @NonNull int[] grantResults) {
        super.onRequestPermissionsResult(requestCode, permissions, grantResults);
        if (requestCode == 100 && grantResults.length > 0 && grantResults[0] == PackageManager.PERMISSION_GRANTED) {
            startGnssTracking();
        }
    }

	@Override
	protected void onResume() {
	    super.onResume();
	    // 注册监听（注意：需检查权限）
	    checkPermissionsAndStart();
	}
	
	@Override
	protected void onPause() {
	    super.onPause();
	    // 停止监听以节省电力
	    locationManager.unregisterGnssMeasurementsCallback(measurementCallback);
	}
}
