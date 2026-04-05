package com.example.gnssdemo;

public class RtkLibWrapper {
    static {
        System.loadLibrary("rtklib");
    }

    public native int rtkpos(String roverObs, String baseObs, String outputPath);
}
