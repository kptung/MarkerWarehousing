package org.iii.snsi.drawer;

import android.content.Context;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.graphics.Canvas;
import android.graphics.Color;
import android.graphics.ImageFormat;
import android.graphics.Matrix;
import android.graphics.Paint;
import android.graphics.Point;
import android.graphics.Rect;
import android.graphics.YuvImage;

import java.io.ByteArrayOutputStream;
import java.util.HashMap;
import java.util.Map;

/**
 * Created by jimchen on 2017/11/22.
 */
public class DrawOnCameraFrame extends DrawTrackingRect {

    private HashMap<Integer, int[]> rectSets;
    private HashMap<Integer, int[]> circleSets;
    private int layoutWidth;
    private int layoutHeight;
    private Bitmap bbitmap;


    public DrawOnCameraFrame(Context context) {
        super(context);
        rectSets = new HashMap<Integer, int[]>();
        circleSets = new HashMap<Integer, int[]>();
    }

    public Bitmap getResizedBitmap(Bitmap bm, int newHeight, int newWidth)
    {
        int width = bm.getWidth();
        int height = bm.getHeight();
        float scaleWidth = ((float) newWidth) / width;
        float scaleHeight = ((float) newHeight) / height;
        // create a matrix for the manipulation
        Matrix matrix = new Matrix();
        // resize the bit map
        matrix.postScale(scaleWidth, scaleHeight);
        // recreate the new Bitmap
        Bitmap resizedBitmap = Bitmap.createBitmap(bm, 0, 0, width, height, matrix, false);
        return resizedBitmap;
    }

    public void SetBitmap(byte[] bytes, int width, int height)
    {
        // catch the image
        YuvImage im = new YuvImage(bytes, ImageFormat.NV21, width, height, null);
        ByteArrayOutputStream baos = new ByteArrayOutputStream();
        im.compressToJpeg(new Rect(0,0,width,height), 90, baos);
        Bitmap bitmap = BitmapFactory.decodeByteArray(baos.toByteArray(), 0, baos.toByteArray().length);
        bbitmap = getResizedBitmap(bitmap, layoutHeight, layoutWidth);
    }


    @Override
    public void removeTrackingRect(int[] objIDs) {
        for (int objID : objIDs) {
            rectSets.remove(objID);
        }
    }

    @Override
    public void removeTrackingRect() {
        rectSets.clear();
    }

    @Override
    public int[] processTrackingRect(int width, int height, double[] data) {
        rectSets.clear();
        if (data == null) {
            return null;
        }

        for (int i = 0; i < data.length; i += 5) {
            double left = (double) ((data[i + 1]) / width) * layoutWidth;
            double top = (double) ((data[i + 2]) / height) * layoutHeight;
            double right = (double) ((data[i + 1] + data[i + 3]) / width) * layoutWidth;
            double bottom = (double) ((data[i + 2] + data[i + 4]) / height) * layoutHeight;

            if ((top > layoutHeight) || (bottom < 0) || (right < 0) || (left > layoutWidth))
            {
            } else {
                rectSets.put((int) Math.round(data[i]), new int[] {(int) Math.round(left),
                        (int) Math.round(top), (int) Math.round(right),
                        (int) Math.round(bottom)});
            }
        }
        int[] xx = new int [1];
        return xx;
    }

    public int[] processTrackingCircle(int width, int height, double[] data) {
        rectSets.clear();
        if (data == null) {
            return null;
        }

        for (int i = 0; i < data.length; i += 5) {
            double left = (double) ((data[i + 1]) / width) * layoutWidth;
            double top = (double) ((data[i + 2]) / height) * layoutHeight;
            double radius = (double)  ((data[i + 3]) / width) * layoutWidth;
            circleSets.put((int) Math.round(data[i]), new int[] {(int) Math.round(left),
                        (int) Math.round(top), (int) Math.round(radius)});

        }
        int[] xx = new int [1];
        return xx;
    }

    @Override
    public void setLayoutSize(int width, int height) {
        layoutWidth = width;
        layoutHeight = height;
    }

    @Override
    public int[] getScopeROI() {
        return new int[0];
    }

    @Override
    public void setTrackingCalibration(int offsetWidth, int offsetHeight,
            int roiWidth, int roiHeight) {

    }



    @Override
    protected void onDraw(Canvas canvas) {
        super.onDraw(canvas);
        Paint paint = new Paint();
        paint.setFilterBitmap(true);
        if(bbitmap!=null)
        {
            canvas.drawBitmap(bbitmap, 0, 0, paint);
        }
        paint.setAntiAlias(true);
        paint.setColor(Color.RED);
        paint.setStyle(Paint.Style.FILL);
        paint.setStrokeWidth(5);
        for (Map.Entry entry : rectSets.entrySet()) {
            Object key = entry.getKey();
            int[] rectData = rectSets.get(key);
            if (key.equals(0)) {
                paint.setColor(Color.argb(128, 0, 255, 255));
            } else {
                paint.setColor(Color.argb(128, 255, 0, 255));
            }
            drawFilledRect(canvas, paint, new Point(rectData[0], rectData[1]), new Point(rectData[2], rectData[3]));
            //canvas.drawCircle(rectData[0], rectData[1], 30, paint);
        }
        for (Map.Entry entry : circleSets.entrySet()) {
            Object key = entry.getKey();
            int[] circleData = circleSets.get(key);
            if (key.equals(0)) {
                paint.setColor(Color.argb(128, 0, 255, 255));
            } else {
                paint.setColor(Color.argb(128, 255, 0, 255));
            }
            canvas.drawCircle(circleData[0], circleData[1], circleData[2], paint);
            //drawFilledCircle(canvas, paint, new Point(circleData[0], circleData[1]), circleData[2]);
        }
    }

    private void drawFilledRect(Canvas canvas, Paint paint, Point topLeftPoint,
            Point bottomRightPoint) {
        paint.setStyle(Paint.Style.FILL);
        canvas.drawRect(topLeftPoint.x, topLeftPoint.y, bottomRightPoint.x,
                bottomRightPoint.y, paint);

    }

    private void drawFilledCircle(Canvas canvas, Paint paint, Point center,
                                int radius) {
        paint.setStyle(Paint.Style.FILL);
        canvas.drawCircle(center.x, center.y, radius, paint);

    }


    private void drawHollowRect(Canvas canvas, Paint paint, Point topLeftPoint,
            Point bottomRightPoint) {
        paint.setStyle(Paint.Style.STROKE);

        if (topLeftPoint.x < layoutWidth) {
            //left line
            canvas.drawLine(topLeftPoint.x, topLeftPoint.y, topLeftPoint.x,
                    bottomRightPoint.y, paint);

            if (bottomRightPoint.x >= layoutWidth) {
                //top line
                canvas.drawLine(topLeftPoint.x, topLeftPoint.y, layoutWidth - 1,
                        topLeftPoint.y, paint);
                //bottom line
                canvas.drawLine(topLeftPoint.x, bottomRightPoint.y,
                        layoutWidth - 1, bottomRightPoint.y, paint);
            } else {
                //top line
                canvas.drawLine(topLeftPoint.x, topLeftPoint.y,
                        bottomRightPoint.x, topLeftPoint.y, paint);
                //right line
                canvas.drawLine(bottomRightPoint.x, topLeftPoint.y,
                        bottomRightPoint.x, bottomRightPoint.y, paint);
                //bottom line
                canvas.drawLine(topLeftPoint.x, bottomRightPoint.y,
                        bottomRightPoint.x, bottomRightPoint.y, paint);
            }
        }

    }

}
