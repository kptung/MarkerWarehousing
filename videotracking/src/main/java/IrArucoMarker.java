import org.iii.snsi.markertpositioning.Point2D;
import org.iii.snsi.markertpositioning.Point3D;

public class IrArucoMarker {
    private static final String TAG = "IrMarker";
    public int id;
    public int ori;
    public double distance;
    public Point2D[] corners;
    public Point3D position;
    public Point2D[] injectpoints;


    public IrArucoMarker() {
    }

    @Override
    public String toString() {
        StringBuilder stringBuilder = new StringBuilder();
        stringBuilder.append(String.format("id:%d", id));
        stringBuilder.append(String.format("ori:%d", ori));
        stringBuilder.append(String.format("distance:%f", distance));
        for (Point2D corner:
        corners) {
            stringBuilder.append(corner.toString());
        }

        stringBuilder.append(position.toString());

        for (Point2D injectpoint: injectpoints)
        {
            stringBuilder.append(injectpoint.toString());
        }

        return stringBuilder.toString();
    }
}