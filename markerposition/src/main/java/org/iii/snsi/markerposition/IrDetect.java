package org.iii.snsi.markerposition;

import android.util.Log;

public class IrDetect {

    static {
        try {
            System.loadLibrary("markerposition");
        } catch (Exception e) {
            Log.d("TAG", "Load Lib Fail...");
        }
    }

    public native static boolean importYMLCameraParameters(String filename);

    public native static boolean importYMLDetectParameters(String filename);

    public native static IrArucoMarker[] findArucoMarkersWithMarkerSize(byte[] bytes, int width,int height, float markerSize);

    public native static IrArucoMarker[] findBasicMarkers(byte[] bytes, int width,int height, float markerSize);

}
