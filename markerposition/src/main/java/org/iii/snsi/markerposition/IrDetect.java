package org.iii.snsi.markerposition;

public class IrDetect {

    public native static boolean importYMLCameraParameters(String filename);

    public native static boolean importYMLDetectParameters(String filename);

    public native static IrArucoMarker[] findArucoMarkersWithMarkerSize(byte[] bytes, int width,int height, float markerSize);

    public native static IrArucoMarker[] findBasicMarkers(byte[] bytes, int width,int height, float markerSize);

}
