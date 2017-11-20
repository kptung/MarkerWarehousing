package org.iii.snsi.markertpositioning;

public class Point3D {
        public double x;
        public double y;
        public double z;

        public Point3D() {}

        public Point3D(double x, double y, double z) {
            this.x = x;
            this.y = y;
            this.z = z;
        }

        public Point3D(Point3D src) {
            this.x = src.x;
            this.y = src.y;
            this.z = src.z;
        }

    @Override
    public String toString() {

        return "Point(" + x + ", " + y + ", " + z + ")";
    }
}
