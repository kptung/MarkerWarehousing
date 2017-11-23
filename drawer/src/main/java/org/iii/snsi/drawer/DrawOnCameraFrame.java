package org.iii.snsi.drawer;

import android.content.Context;
import android.graphics.Canvas;
import android.graphics.Color;
import android.graphics.Paint;
import android.graphics.Point;

import java.util.HashMap;
import java.util.Map;

/**
 * Created by jimchen on 2017/11/22.
 */
public class DrawOnCameraFrame extends DrawTrackingRect {

    private HashMap<Integer, int[]> rectSets;
    private int layoutWidth;
    private int layoutHeight;

    public DrawOnCameraFrame(Context context) {
        super(context);
        rectSets = new HashMap<Integer, int[]>();
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
    public int[] processTrackingRect(int width, int height, int[] data) {
        rectSets.clear();
        if (data == null) {
            return null;
        }

        for (int i = 0; i < data.length; i += 5) {
            double left = (double) (data[i + 1]) / width * layoutWidth;
            double top = (double) (data[i + 2]) / height * layoutHeight;
            double right =
                    (double) (data[i + 1] + data[i + 3]) / width * layoutWidth;
            double bottom = (double) (data[i + 2] + data[i + 4]) / height
                    * layoutHeight;

            if ((top > layoutHeight) || (bottom < 0) || (right < 0) || (left
                    > layoutWidth))
            {
                //                System.out.println("not added : " + data[i]);
                //                System.out.println("leftR = " + leftR);
                //                System.out.println("topR = " + topR);
                //                System.out.println("bottomR = " + bottomR);
                //                System.out.println("leftR = " + leftR);
            } else {
                //                System.out.println("add : " + data[i]);
                //                System.out.println("leftR = " + leftR);
                //                System.out.println("topR = " + topR);
                //                System.out.println("bottomR = " + bottomR);
                //                System.out.println("leftR = " + leftR);
                //                System.out.println("leftL = " + leftL);
                //                System.out.println("rightL = " + rightL);

                rectSets.put(data[i], new int[] {(int) Math.round(left),
                        (int) Math.round(top), (int) Math.round(right),
                        (int) Math.round(bottom)});

            }
        }

        return data;
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

            drawFilledRect(canvas, paint, new Point(rectData[0], rectData[1]),
                    new Point(rectData[2], rectData[3]));
        }

    }

    private void drawFilledRect(Canvas canvas, Paint paint, Point topLeftPoint,
            Point bottomRightPoint) {
        paint.setStyle(Paint.Style.FILL);
        canvas.drawRect(topLeftPoint.x, topLeftPoint.y, bottomRightPoint.x,
                bottomRightPoint.y, paint);

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
