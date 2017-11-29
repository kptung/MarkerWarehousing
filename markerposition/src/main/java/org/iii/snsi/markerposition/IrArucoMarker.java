package org.iii.snsi.markerposition;
public class IrArucoMarker {
    private static final String TAG = "IrMarker";
    public int mid;
    public int mori;
    public double mdistance;
    public Point2D[] mcorners;
    public Point2D[] mrejecteds;
    public Point3D camposition;
    public Point2D[] injectpoints;
    public Point2D mcenter;

    public IrArucoMarker() {
    }

    @Override
    public String toString() {
        StringBuilder stringBuilder = new StringBuilder();
        stringBuilder.append(String.format("id:%d", mid));
        stringBuilder.append(String.format("ori:%d", mori));
        stringBuilder.append(String.format("distance:%f", mdistance));
        stringBuilder.append(camposition.toString());
        stringBuilder.append(mcenter.toString());
        for (Point2D corner:
        mcorners) {
            stringBuilder.append(corner.toString());
        }
        for (Point2D rejected:
                mrejecteds) {
            stringBuilder.append(rejected.toString());
        }
        for (Point2D injectpoint: injectpoints)
        {
            stringBuilder.append(injectpoint.toString());
        }

        return stringBuilder.toString();
    }
}