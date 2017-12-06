package org.iii.snsi.drawer;

import android.content.Context;
import android.graphics.Bitmap;
import android.graphics.Canvas;
import android.graphics.Color;
import android.graphics.Paint;
import android.graphics.Point;

import java.util.HashMap;
import java.util.Map;

/**
 * Created by jimchen on 2017/10/31.
 */
public class DrawStereoRect2D extends DrawTrackingRect {

    private boolean dbgObjImg = false;
    private boolean calibrateInside = false;
    private HashMap<Integer, int[]> rectSets;//old item
    private HashMap<Integer, int[]> rectSetsLR;//old item
    private HashMap<Integer, int[]> rectSetsL;//new item
    private HashMap<Integer, int[]> rectSetsR;//new item
    private HashMap<Integer, Bitmap> objImg;
    private int layoutWidth;
    private int layoutHeight;
    private int offsetWidth;
    private int offsetHeight;
    private int roiWidth;
    private int roiHeight;
    private int offsetLR;
    private int[] roiScopeR;
    private int[] roiScopeMix;

    public DrawStereoRect2D(Context context) {
        super(context);
        objImg = new HashMap<Integer, Bitmap>();
        rectSets = new HashMap<Integer, int[]>();
        rectSetsLR = new HashMap<Integer, int[]>();
        rectSetsL = new HashMap<Integer, int[]>();
        rectSetsR = new HashMap<Integer, int[]>();
    }

    public void setDbgObjImg(int flag) {
        if (flag >= 1) {
            dbgObjImg = true;
        } else {
            dbgObjImg = false;
        }
    }

    @Override
    public int[] getScopeROI() {
        roiScopeMix = new int[] {roiScopeR[0] - offsetLR, roiScopeR[1],
                roiScopeR[2] + offsetLR, roiScopeR[3]};
        return roiScopeMix;
    }

    @Override
    public void setTrackingCalibration(int offsetWidth, int offsetHeight,
            int roiWidth, int roiHeight) {
        this.offsetWidth = offsetWidth;
        this.offsetHeight = offsetHeight;
        this.roiWidth = roiWidth;
        this.roiHeight = roiHeight;
        // roiScope leftTop x,y rightBottom x,y
        roiScopeR = new int[4];
        roiScopeR[0] = offsetWidth;
        roiScopeR[1] = offsetHeight;
        roiScopeR[2] = offsetWidth + roiWidth;
        roiScopeR[3] = offsetHeight + roiHeight;
        calibrateInside = true;
    }

    public void setOffsetLR(int offsetLR) {
        this.offsetLR = offsetLR;
        calibrateInside = true;
    }

    public void drawLeftRect(int left, int top, int right, int bottom) {
        calibrateInside = false;
        rectSetsL.put(0, new int[] {left, top, right, bottom});
    }

    public void drawRightRect(int left, int top, int right, int bottom) {
        calibrateInside = false;
        rectSetsR.put(0, new int[] {left, top, right, bottom});
    }

    @Override
    public void removeTrackingRect(int[] objIDs) {
        for (int objID : objIDs) {
            rectSets.remove(objID);
            rectSetsLR.remove(objID);
            objImg.remove(objID);
        }
    }

    @Override
    public void removeTrackingRect() {
        rectSets.clear();
        rectSetsLR.clear();
        objImg.clear();
    }

    @Override
    public int[] processTrackingRect(int width, int height, double[] data) {
        rectSets.clear();
        rectSetsLR.clear();
        if (data == null) {
            return null;
        }

        double rateX = width / 320;
        double rateY = height / 240;

        for (int i = 0; i < data.length; i += 5) {
            double leftR =
                    (double) (data[i + 1] - (offsetWidth * rateX)) / (roiWidth
                            * rateX) * layoutWidth + layoutWidth;
            double topR =
                    (double) (data[i + 2] - (offsetHeight * rateY)) / (roiHeight
                            * rateY) * layoutHeight;
            double rightR =
                    (double) (data[i + 1] + data[i + 3] - (offsetWidth * rateX))
                            / (roiWidth * rateX) * layoutWidth + layoutWidth;
            double bottomR = (double) (data[i + 2] + data[i + 4] - (offsetHeight
                    * rateY)) / (roiHeight * rateY) * layoutHeight;

            double leftL = leftR + offsetLR * rateX - layoutWidth;
            double rightL = rightR + offsetLR * rateX - layoutWidth;

            if ((topR > layoutHeight) || (bottomR < 0) || (rightL < 0) || (leftR
                    > (layoutWidth * 2)))
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

                rectSets.put((int) Math.round(data[i]), new int[] {(int) Math.round(leftR),
                        (int) Math.round(topR), (int) Math.round(rightR),
                        (int) Math.round(bottomR)});
                rectSetsLR.put((int) Math.round(data[i]), new int[] {(int) Math.round(leftL),
                        (int) Math.round(rightL)});

            }
        }
        int[] xx = new int [1];
        return xx;
    }

    @Override
    public void setLayoutSize(int width, int height) {
        layoutWidth = width / 2;
        layoutHeight = height;
    }

    @Override
    protected void onDraw(Canvas canvas) {
        super.onDraw(canvas);
        if (calibrateInside) {
            calibrateInsideDraw(canvas);
        } else {
            calibrateOutsideDraw(canvas);
        }
    }

    private void calibrateInsideDraw(Canvas canvas) {
        Paint paint = new Paint();
        paint.setAntiAlias(true);
        paint.setStrokeWidth(5);
        Point topLeftPointLeftEye;
        Point bottomRightPointLeftEye;
        Point topLeftPointRightEye;
        Point bottomRightPointRightEye;

        for (Map.Entry entry : rectSets.entrySet()) {
            Object key = entry.getKey();
            int[] rectData = rectSets.get(key);
            int[] rectLR = rectSetsLR.get(key);
            topLeftPointLeftEye = new Point(rectLR[0], rectData[1]);
            bottomRightPointLeftEye = new Point(rectLR[1], rectData[3]);
            topLeftPointRightEye = new Point(rectData[0], rectData[1]);
            bottomRightPointRightEye = new Point(rectData[2], rectData[3]);
            if (key.equals(0)) {
                paint.setColor(Color.argb(128, 0, 255, 255));
            } else {
                paint.setColor(Color.argb(128, 255, 0, 255));
            }

            System.out.println("rectLR[0] , rectData[1] =  " + rectLR[0] + " , " + rectData[1]);
            System.out.println("rectLR[1] , rectData[3] =  " + rectLR[1] + " , " + rectData[3]);
            drawFilledRect(canvas, paint, topLeftPointLeftEye,
                    bottomRightPointLeftEye, topLeftPointRightEye,
                    bottomRightPointRightEye);

        }
    }

    private void calibrateOutsideDraw(Canvas canvas) {
        Paint paint = new Paint();
        paint.setAntiAlias(true);
        paint.setColor(Color.RED);
        paint.setStrokeWidth(5);
    }

    private void drawFilledRect(Canvas canvas, Paint paint,
            Point topLeftPointLeftEye, Point bottomRightPointLeftEye,
            Point topLeftPointRightEye, Point bottomRightPointRightEye) {
        paint.setStyle(Paint.Style.FILL);
        //---------------left eye
        Point realBottomRightPointLeftEye = bottomRightPointLeftEye;
        if (topLeftPointLeftEye.x < layoutWidth) {

            if (bottomRightPointLeftEye.x >= layoutWidth) {
                realBottomRightPointLeftEye = new Point(layoutWidth - 1,
                        bottomRightPointLeftEye.y);
            }

            canvas.drawRect(topLeftPointLeftEye.x, topLeftPointLeftEye.y,
                    realBottomRightPointLeftEye.x,
                    realBottomRightPointLeftEye.y, paint);
        }
        //---------------end left eye
        //------------------right eye
        Point realTopLeftPointRightEye = topLeftPointRightEye;

        if (bottomRightPointRightEye.x >= layoutWidth) {

            if (topLeftPointRightEye.x < layoutWidth) {
                realTopLeftPointRightEye = new Point(layoutWidth,
                        topLeftPointRightEye.y);
            }

            canvas.drawRect(realTopLeftPointRightEye.x,
                    realTopLeftPointRightEye.y, bottomRightPointRightEye.x,
                    bottomRightPointRightEye.y, paint);
        }
        //----------------end left eye
    }

    private void drawHollowRect(Canvas canvas, Paint paint,
            Point topLeftPointLeftEye, Point bottomRightPointLeftEye,
            Point topLeftPointRightEye, Point bottomRightPointRightEye) {
        paint.setStyle(Paint.Style.STROKE);
        //---------------left eye
        if (topLeftPointLeftEye.x < layoutWidth) {
            //left line
            canvas.drawLine(topLeftPointLeftEye.x, topLeftPointLeftEye.y,
                    topLeftPointLeftEye.x, bottomRightPointLeftEye.y, paint);

            if (bottomRightPointLeftEye.x >= layoutWidth) {
                //top line
                canvas.drawLine(topLeftPointLeftEye.x, topLeftPointLeftEye.y,
                        layoutWidth - 1, topLeftPointLeftEye.y, paint);
                //bottom line
                canvas.drawLine(topLeftPointLeftEye.x,
                        bottomRightPointLeftEye.y, layoutWidth - 1,
                        bottomRightPointLeftEye.y, paint);
            } else {
                //top line
                canvas.drawLine(topLeftPointLeftEye.x, topLeftPointLeftEye.y,
                        bottomRightPointLeftEye.x, topLeftPointLeftEye.y,
                        paint);
                //right line
                canvas.drawLine(bottomRightPointLeftEye.x,
                        topLeftPointLeftEye.y, bottomRightPointLeftEye.x,
                        bottomRightPointLeftEye.y, paint);
                //bottom line
                canvas.drawLine(topLeftPointLeftEye.x,
                        bottomRightPointLeftEye.y, bottomRightPointLeftEye.x,
                        bottomRightPointLeftEye.y, paint);
            }
        }
        //---------------end left eye
        //------------------right eye

        if (bottomRightPointRightEye.x >= layoutWidth) {
            //right line
            canvas.drawLine(bottomRightPointRightEye.x, topLeftPointRightEye.y,
                    bottomRightPointRightEye.x, bottomRightPointRightEye.y,
                    paint);

            if (topLeftPointRightEye.x < layoutWidth) {
                //top line
                canvas.drawLine(layoutWidth, topLeftPointRightEye.y,
                        bottomRightPointRightEye.x, topLeftPointRightEye.y,
                        paint);
                //bottom line
                canvas.drawLine(layoutWidth, bottomRightPointRightEye.y,
                        bottomRightPointRightEye.x, bottomRightPointRightEye.y,
                        paint);
            } else {
                //left line
                canvas.drawLine(topLeftPointRightEye.x, topLeftPointRightEye.y,
                        topLeftPointRightEye.x, bottomRightPointRightEye.y,
                        paint);
                //top line
                canvas.drawLine(topLeftPointRightEye.x, topLeftPointRightEye.y,
                        bottomRightPointRightEye.x, topLeftPointRightEye.y,
                        paint);
                //bottom line
                canvas.drawLine(topLeftPointRightEye.x,
                        bottomRightPointRightEye.y, bottomRightPointRightEye.x,
                        bottomRightPointRightEye.y, paint);
            }
        }
        //----------------end left eye
    }
}
