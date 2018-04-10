package org.iii.snsi.markerposition;
public class IrArucoMarker {
    private static final String TAG = "IrArMarker";
    public int mid;
    public int mori;
    public int mxzangle;
    public int myzangle;
    public double mxzdistance;
    public double mdistance;
    public Point2D[] mcorners;
    public Point2D[] mrejecteds;
    public Point3D camposition;
    public Point2D[] injectpoints;
    public Point2D mcenter;

    public IrArucoMarker() {
        mid=-1;
        mori=-1;
        mxzangle=-1;
        myzangle=-1;
        mxzdistance=-1;
        mdistance=-1;
        camposition.x=-1;
        camposition.y=-1;
        camposition.z=-1;
        mcenter.x=-1;
        mcenter.y=-1;
    }
}