package org.iii.snsi.markerposition;

import android.util.Log;

public class IrArDetect {

    static {
        try {
            System.loadLibrary("markerposition");
        } catch (Exception e) {
            Log.d("TAG", "Load Lib Fail...");
        }
    }

    public native static boolean importYMLCameraParameters(String filename);

    public native static boolean importYMLDetectParameters(String filename);

    public native static IrArucoMarker[] findAppMarkers(byte[] bytes, int width,int height, float markerSize);

    public native static IrArucoMarker[] findAdvMarkers(byte[] bytes, int width,int height, float markerSize);

    public native static IrArucoMarker[] findBasicMarkers(byte[] bytes, int width,int height, float markerSize);

}
