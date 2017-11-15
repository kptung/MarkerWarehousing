package org.iii.snsi.markerposition;

import android.content.Context;
import android.util.Log;

public class IrDetect {
    static {
        try {
            System.loadLibrary("videotracking");
        } catch (Exception e) {
            Log.d("TAG", "Load Lib Fail...");
        }
    }
    private static final String TAG = "IrDetect";
    private static final String YML_FILE = "/sdcard/camera.yml";
    private static final String YML_FILE2 = "/sdcard/detector_params.yml";

    public static void initialization(Context context) {
        importYMLCameraParameters(YML_FILE);
        importYMLDetectParameters(YML_FILE2);
    }

    public native static boolean importYMLCameraParameters(String filename);

    public native static boolean importYMLDetectParameters(String filename);

//    public native static IrArucoMarker[] findArucoMarkersWithMarkerSize(long matObjectAddress, float markerSizeInMeter, float distanceBelowMarkerCenter);
//
//    public static IrArucoMarker[] findArucoMarkersWithMarkerSize(byte[] bytes, int width,int height, float markerSizeInMeter, float distanceBelowMarkerCenter){
//        if (bytes == null)
//            return null;
//        Mat mat = ConvertUtil.convertNv21ToMat(bytes, width, height);
//        return findArucoMarkersWithMarkerSize(mat.getNativeObjAddr(), markerSizeInMeter, distanceBelowMarkerCenter);
//    }
}
