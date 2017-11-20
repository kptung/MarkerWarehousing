package org.iii.snsi.videotracking;

import android.content.Context;
import android.util.Log;
import android.widget.TextView;

import org.iii.snsi.videotracking.IrArucoMarker;

public class IrDetect {

    public native static boolean importYMLCameraParameters(String filename);

    public native static boolean importYMLDetectParameters(String filename);

    public native static boolean importYMLRTParameters(String filename);

    public native static IrArucoMarker[] findArucoMarkersWithMarkerSize(byte[] bytes, int width,int height, float markerSizeInMeter, float distanceBelowMarkerCenter);



}
