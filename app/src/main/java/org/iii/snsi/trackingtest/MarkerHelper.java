package org.iii.snsi.trackingtest;

import android.util.Log;
import android.widget.TextView;

import org.iii.snsi.markerposition.IrArucoMarker;
import org.iii.snsi.markerposition.IrDetect;

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
    private static final String YML_FILE = "/sdcard/a6k_mr/bt300jj130-camera.yml";
    private static final String YML_FILE2 = "/sdcard/detector_params.yml";
    private static final String YML_FILE3 = "/sdcard/bt300-RTmatrices.yml";
    private static boolean enabelRTMatrices = false;

    public static void initialization() {
        IrDetect.importYMLCameraParameters(YML_FILE);
        IrDetect.importYMLDetectParameters(YML_FILE2);
        if (enabelRTMatrices)
            IrDetect.importYMLRTParameters(YML_FILE3);
    }

    public static IrArucoMarker[] nFindArucoMarkersWithMarkerSize(byte[] bytes, int width,int height, float markerSizeInMeter){
        if (bytes == null)
            return null;

        return IrDetect.findArucoMarkersWithMarkerSize(bytes, width, height, markerSizeInMeter);
    }

    public static void printFullMarkerSet(IrArucoMarker[] markerSet, TextView tv) {
        if (markerSet != null && markerSet.length > 0) {
            tv.append("Markers: " + markerSet.length + "\n");
            tv.append("<<<< ---- ---- ----\n");
            for (int i = 0; i < markerSet.length; i++) {
                tv.append("ID: " + markerSet[i].mid + "; ");

                if (markerSet[i].mcorners != null
                        && markerSet[i].mcorners.length == 4)
                {
                    tv.append(
                            "Top-left: (" + markerSet[i].mcorners[0].x + ", "
                                    + markerSet[i].mcorners[0].y + "); ");
                    tv.append("Top-right: (" + markerSet[i].mcorners[1].x
                            + ", " + markerSet[i].mcorners[1].y + "); ");
                    tv.append("Bottom-right: (" + markerSet[i].mcorners[2].x
                            + ", " + markerSet[i].mcorners[2].y + "); ");
                    tv.append("Bottom-left: (" + markerSet[i].mcorners[3].x
                            + ", " + markerSet[i].mcorners[3].y + ")\n");
                }


                if (markerSet[i].injectpoints != null) {
                    tv.append("InjectPoint: ("
                            + markerSet[i].injectpoints[0].x + ", "
                            + markerSet[i].injectpoints[0].y + ")\n");
                }

                tv.append("Distance: " + markerSet[i].mdistance);
            }
            tv.append("---- ---- ---- >>>>\n");
        } else {
            tv.append("Did not find any markers in the image.\n");
        }

    }
}
