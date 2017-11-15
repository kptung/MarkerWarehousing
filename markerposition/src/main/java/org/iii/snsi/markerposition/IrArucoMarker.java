package org.iii.snsi.markerposition;
import android.graphics.Point;

public class IrArucoMarker {
    private static final String TAG = "IrMarker";
    public int id;
    public int ori;
    public Point[] corners;
    public Point3D position;
    public Point[] injectpoints;

    public IrArucoMarker() {
    }

}