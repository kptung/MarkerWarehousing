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
        mid=-1;
        mori=-1;
        mxzdistance=-1;
        camposition.x=-1;
        camposition.y=-1;
        camposition.z=-1;
        mcenter.x=-1;
        mcenter.y=-1;
    }
}