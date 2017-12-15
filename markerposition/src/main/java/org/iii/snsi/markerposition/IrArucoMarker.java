package org.iii.snsi.markerposition;
public class IrArucoMarker {
    private static final String TAG = "IrArMarker";
    public int mid;
    public int mori;
    public double mxzdistance;
    public Point2D[] mcorners;
    public Point2D[] mrejecteds;
    public Point3D camposition;
    public Point2D[] injectpoints;
    public Point2D mcenter;

    public IrArucoMarker() {
    }
}