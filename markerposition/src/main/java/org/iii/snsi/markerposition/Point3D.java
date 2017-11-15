package org.iii.snsi.markerposition;

public class Point3D {
        public int x;
        public int y;
        public int z;

        public Point3D() {}

        public Point3D(int x, int y, int z) {
            this.x = x;
            this.y = y;
        }

        public Point3D(Point3D src) {
            this.x = src.x;
            this.y = src.y;
            this.z = src.z;
        }
}
