package org.iii.snsi.trackingtest;

import android.os.Environment;
import android.util.Log;

import org.iii.snsi.markerposition.IrArucoMarker;
import org.iii.snsi.markerposition.IrDetect;

import java.io.File;

public class MarkerHelper {
    static {
        try {
            System.loadLibrary("opencv_java3");
            System.loadLibrary("markerposition");
        } catch (Exception e) {
            Log.d("TAG", "Load Lib Fail...");
        }
    }
    private static final String TAG = "MarkerHelper";
    private static final String Cam_YML = "/markpos/bt300-camera.yml";
    private static final String Det_YML = "/markpos/detector_params.yml";

    public static void initialization() {
        String camfile = Environment.getExternalStorageDirectory().getPath() + Cam_YML;
        String detfile = Environment.getExternalStorageDirectory().getPath() + Det_YML;
        File f1=new File(camfile);
        File f2=new File(detfile);

        if(f1.exists()&&f2.exists()) {
            IrDetect.importYMLCameraParameters(camfile);
            IrDetect.importYMLDetectParameters(detfile);
        }
        else
        {
            System.out.println("Error!! No parameters. Please check /sdcard/markpos/bt300-camera.yml + detector_params.yml");
            return;
        }
    }

    public static IrArucoMarker[] nFindArucoMarkersWithMarkerSize(byte[] bytes, int width,int height, float markerSize){
        if (bytes == null) {
            System.out.println("Error!! Image is NULL. Please check it");
            return null;
        }
        return IrDetect.findArucoMarkersWithMarkerSize(bytes, width, height, markerSize);
    }

}
