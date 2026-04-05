package com.example.gnssdemo;

public class RtkLibWrapper {
    static {
        System.loadLibrary("rtklib");
    }

    public native String getRtklibVersion();
    public native long initRtk();
    public native void freeRtk(long rtkPtr);
    
    // 处理一帧观测数据
    public native int processRtk(long rtkPtr, long obsPtr, int nObs, long navPtr);
}
