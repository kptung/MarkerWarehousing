package org.iii.snsi.markertest;

import android.os.Environment;

import org.iii.snsi.markerposition.IrArucoMarker;
import org.iii.snsi.markerposition.IrArDetect;

import java.io.File;

public class MarkerHelper {

    private static final String TAG = "MarkerHelper";
    private static final String Cam_YML = "/markpos/bt300-camera.yml";
    private static final String Det_YML = "/markpos/detector_params.yml";

    public static void initialization() {
        String camfile = Environment.getExternalStorageDirectory().getPath() + Cam_YML;
        String detfile = Environment.getExternalStorageDirectory().getPath() + Det_YML;
        File f1=new File(camfile);
        File f2=new File(detfile);

        if(f1.exists()&&f2.exists()) {
            IrArDetect.importYMLCameraParameters(camfile);
            IrArDetect.importYMLDetectParameters(detfile);
        }
        else
        {
            System.out.println("Error!! No parameters. Please check /sdcard/markpos/bt300-camera.yml + detector_params.yml");
            return;
        }
    }

    public static IrArucoMarker[] nFindAppMarkers(byte[] bytes, int width,int height, float markerSize, double[] markerArea){
        if (bytes == null) {
            System.out.println("Error!! Image is NULL. Please check it");
            return null;
        }
        return IrArDetect.findAppMarkers(bytes, width, height, markerSize, markerArea);
    }

    public static IrArucoMarker[] nFindBasicMarkers(byte[] bytes, int width,int height){
        if (bytes == null) {
            System.out.println("Error!! Image is NULL. Please check it");
            return null;
        }
        return IrArDetect.findBasicMarkers(bytes, width, height);
    }

    public static IrArucoMarker[] nFindAdvMarkers(byte[] bytes, int width,int height, float markerSize){
        if (bytes == null) {
            System.out.println("Error!! Image is NULL. Please check it");
            return null;
        }
        return IrArDetect.findAdvMarkers(bytes, width, height, markerSize);
    }

}
