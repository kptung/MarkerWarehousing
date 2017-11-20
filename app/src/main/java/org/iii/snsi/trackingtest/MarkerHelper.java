package org.iii.snsi.trackingtest;

import android.content.Context;
import android.util.Log;
import android.widget.TextView;

import org.iii.snsi.videotracking.IrArucoMarker;
import org.iii.snsi.videotracking.IrDetect;

public class MarkerHelper {
    static {
        try {
            System.loadLibrary("opencv_java3");
            System.loadLibrary("videotracking");
        } catch (Exception e) {
            Log.d("TAG", "Load Lib Fail...");
        }
    }
    private static final String TAG = "IrDetect";
    private static final String YML_FILE = "/sdcard/camera.yml";
    private static final String YML_FILE2 = "/sdcard/detector_params.yml";
    private static final String YML_FILE3 = "/sdcard/bt300-RTmatrices.yml";
    private static boolean enabelRTMatrices = false;

    public static void initialization(Context context) {
        IrDetect.importYMLCameraParameters(YML_FILE);
        IrDetect.importYMLDetectParameters(YML_FILE2);
        if (enabelRTMatrices)
            IrDetect.importYMLRTParameters(YML_FILE3);
    }

    public static IrArucoMarker[] nFindArucoMarkersWithMarkerSize(byte[] bytes, int width,int height, float markerSizeInMeter, float distanceBelowMarkerCenter){
        if (bytes == null)
            return null;

        return IrDetect.findArucoMarkersWithMarkerSize(bytes, width, height, markerSizeInMeter, distanceBelowMarkerCenter);
    }

    public static void printFullMarkerSet(IrArucoMarker[] markerSet, TextView tv) {
        if (markerSet != null && markerSet.length > 0) {
            tv.append("Markers: " + markerSet.length + "\n");
            tv.append("<<<< ---- ---- ----\n");
            for (int i = 0; i < markerSet.length; i++) {
                tv.append("ID: " + markerSet[i].id + "; ");
                //                mMarkerInfoText.append(
                //                        "Orientation: " + fullMarkerSet[i].ori + "; ");
                //                mMarkerInfoText.append(
                //                        "Score: " + fullMarkerSet[i].score + "\n");

                if (markerSet[i].corners != null
                        && markerSet[i].corners.length == 4)
                {
                    tv.append(
                            "Top-left: (" + markerSet[i].corners[0].x + ", "
                                    + markerSet[i].corners[0].y + "); ");
                    tv.append("Top-right: (" + markerSet[i].corners[1].x
                            + ", " + markerSet[i].corners[1].y + "); ");
                    tv.append("Bottom-right: (" + markerSet[i].corners[2].x
                            + ", " + markerSet[i].corners[2].y + "); ");
                    tv.append("Bottom-left: (" + markerSet[i].corners[3].x
                            + ", " + markerSet[i].corners[3].y + ")\n");
                }

                //                mMarkerInfoText.append(
                //                        "Position: (" + fullMarkerSet[i].position.x + ", "
                //                                + fullMarkerSet[i].position.y + ", "
                //                                + fullMarkerSet[i].position.z + ")\n");
                //                mMarkerInfoText.append(
                //                        "Rotation: (" + fullMarkerSet[i].rotation.dump()
                //                                + ")\n");

                //                if (fullMarkerSet[i].keypoints != null) {
                //                    for (int j = 0; j < fullMarkerSet[i].keypoints.length; j++)
                //                    {
                //                        mMarkerInfoText.append(
                //                                "KeyPoint: (" + fullMarkerSet[i].keypoints[j].x
                //                                        + ", " + fullMarkerSet[i].keypoints[j].y
                //                                        + ")\n");
                //                    }
                //                }

                if (markerSet[i].injectpoints != null) {
                    tv.append("InjectPoint: ("
                            + markerSet[i].injectpoints[0].x + ", "
                            + markerSet[i].injectpoints[0].y + ")\n");
                }

                tv.append("Distance: " + markerSet[i].distance);
            }
            tv.append("---- ---- ---- >>>>\n");
        } else {
            tv.append("Did not find any markers in the image.\n");
        }

    }
}
