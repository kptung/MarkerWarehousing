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
 * Created by jimchen on 2017/10/31.
 */
public class DrawStereoRect2D extends DrawTrackingRect {

    private static final int COLOR_RECT = Color.argb(255, 255, 255, 0);
    private static final int COLOR_CIRCLE = Color.argb(255, 255, 0, 255);
    private boolean dbgObjImg = false;
    private boolean calibrateInside = false;
    private HashMap<Integer, int[]> rectSets;//old item
    private HashMap<Integer, int[]> rectSetsLR;//old item
    private HashMap<Integer, int[]> rectSetsL;//new item
    private HashMap<Integer, int[]> rectSetsR;//new item
    private HashMap<Integer, int[]> circleSets;//old item
    private HashMap<Integer, int[]> circleSetsLR;//old item
    private HashMap<Integer, int[]> circleSetsL;//new item
    private HashMap<Integer, int[]> circleSetsR;//new item
    private HashMap<Integer, Bitmap> objImg;
    private int layoutWidth;
    private int layoutHeight;
    private int screenWidth;
    private int screenHeight;
    private int offsetWidth;
    private int offsetHeight;
    private int roiWidth;
    private int roiHeight;
    private int offsetLR;
    private int[] roiScopeR;
    private int[] roiScopeMix;
    private Bitmap bBitmap;
    private int drawShape = 0;//0:circle, 1:rectangle
    private int[] circleData;

    public DrawStereoRect2D(Context context) {
        super(context);
        objImg = new HashMap<Integer, Bitmap>();
        rectSets = new HashMap<Integer, int[]>();
        rectSetsLR = new HashMap<Integer, int[]>();
        rectSetsL = new HashMap<Integer, int[]>();
        rectSetsR = new HashMap<Integer, int[]>();
        circleSets = new HashMap<Integer, int[]>();
        circleSetsLR = new HashMap<Integer, int[]>();
        circleSetsL = new HashMap<Integer, int[]>();
        circleSetsR = new HashMap<Integer, int[]>();
    }

    public Bitmap getResizedBitmap(Bitmap bm, int newHeight, int newWidth) {
        int width = bm.getWidth();
        int height = bm.getHeight();
        float scaleWidth = ((float) newWidth) / width;
        float scaleHeight = ((float) newHeight) / height;
        // create a matrix for the manipulation
        Matrix matrix = new Matrix();
        // resize the bit map
        matrix.postScale(scaleWidth, scaleHeight);
        // recreate the new Bitmap
        Bitmap resizedBitmap = Bitmap.createBitmap(bm, 0, 0, width, height,
                matrix, false);
        return resizedBitmap;
    }

    public void SetBitmap(byte[] bytes, int width, int height) {
        // catch the image
        YuvImage im = new YuvImage(bytes, ImageFormat.NV21, width, height,
                null);
        ByteArrayOutputStream baos = new ByteArrayOutputStream();
        im.compressToJpeg(new Rect(0, 0, width, height), 90, baos);
        Bitmap bitmap = BitmapFactory.decodeByteArray(baos.toByteArray(), 0,
                baos.toByteArray().length);
        bBitmap = getResizedBitmap(bitmap, layoutHeight, layoutWidth);
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
        drawShape = 1;
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
            } else {
                rectSets.put((int) Math.round(data[i]),
                        new int[] {(int) Math.round(leftR),
                                (int) Math.round(topR),
                                (int) Math.round(rightR),
                                (int) Math.round(bottomR)});
                rectSetsLR.put((int) Math.round(data[i]),
                        new int[] {(int) Math.round(leftL),
                                (int) Math.round(rightL)});

            }
        }
        int[] xx = new int[1];
        return xx;
    }

    public int[] processTrackingCircle(int width, int height, double[] data) {
        circleSets.clear();
        circleSetsLR.clear();
        drawShape = 0;
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
                    (double) (data[i + 1] + data[i + 3] * 2 - (offsetWidth
                            * rateX)) / (roiWidth * rateX) * layoutWidth
                            + layoutWidth;
            double bottomR =
                    (double) (data[i + 2] + data[i + 4] * 2 - (offsetHeight
                            * rateY)) / (roiHeight * rateY) * layoutHeight;

            double leftL = leftR + offsetLR * rateX - layoutWidth;
            double rightL = rightR + offsetLR * rateX - layoutWidth;

            if ((topR > layoutHeight) || (bottomR < 0) || (rightL < 0) || (leftR
                    > (layoutWidth * 2)))
            {
            } else {
                circleSets.put((int) Math.round(data[i]),
                        new int[] {(int) Math.round(leftR),
                                (int) Math.round(topR),
                                (int) Math.round(rightR),
                                (int) Math.round(bottomR)});
                circleSetsLR.put((int) Math.round(data[i]),
                        new int[] {(int) Math.round(leftL),
                                (int) Math.round(rightL)});

            }
        }
        int[] xx = new int[1];
        return xx;
    }

    @Override
    public void setLayoutSize(int width, int height) {
        screenWidth = width;
        screenHeight = height;
        layoutWidth = width / 2;
        layoutHeight = height;
    }

    @Override
    protected void onDraw(Canvas canvas) {
        super.onDraw(canvas);
        if (calibrateInside) {
            if (drawShape == 0) {
                calibrateInsideDrawCircle(canvas);
            } else {
                calibrateInsideDraw(canvas);
            }
            //
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
            paint.setColor(COLOR_RECT);

            drawFilledRect(canvas, paint, topLeftPointLeftEye,
                    bottomRightPointLeftEye, topLeftPointRightEye,
                    bottomRightPointRightEye);

        }
    }

    private void drawFilledRect(Canvas canvas, Paint paint,
            Point topLeftPointLeftEye, Point bottomRightPointLeftEye,
            Point topLeftPointRightEye, Point bottomRightPointRightEye) {
        paint.setStyle(Paint.Style.FILL);
        Bitmap b1 = Bitmap.createBitmap(layoutWidth, layoutHeight,
                Bitmap.Config.ARGB_8888);
        Bitmap b2 = Bitmap.createBitmap(layoutWidth, layoutHeight,
                Bitmap.Config.ARGB_8888);
        //---------------left eye
        Point realBottomRightPointLeftEye = bottomRightPointLeftEye;
        if (topLeftPointLeftEye.x < layoutWidth) {

            if (bottomRightPointLeftEye.x >= layoutWidth) {
                realBottomRightPointLeftEye = new Point(layoutWidth - 1,
                        bottomRightPointLeftEye.y);
            }
            Canvas c1 = new Canvas(b1);
            System.out.println(
                    "Left Eye: TopLeft.x , TopLeft.y, Width, Height=  "
                            + topLeftPointLeftEye.x + " , "
                            + topLeftPointLeftEye.y + " , "
                            + realBottomRightPointLeftEye.x + " , "
                            + realBottomRightPointLeftEye.y);
            c1.drawRect(topLeftPointLeftEye.x, topLeftPointLeftEye.y,
                    realBottomRightPointLeftEye.x,
                    realBottomRightPointLeftEye.y, paint);
            //            canvas.drawRect(topLeftPointLeftEye.x, topLeftPointLeftEye.y,
            //                    realBottomRightPointLeftEye.x,
            //                    realBottomRightPointLeftEye.y, paint);
        }
        //---------------end left eye
        //------------------right eye
        Point realTopLeftPointRightEye = topLeftPointRightEye;

        if (bottomRightPointRightEye.x >= layoutWidth) {

            if (topLeftPointRightEye.x < layoutWidth) {
                realTopLeftPointRightEye = new Point(layoutWidth,
                        topLeftPointRightEye.y);
            }
            Canvas c2 = new Canvas(b2);
            int rltx = -1, rrbx =
                    -1;//rltx, rlty: Right-eye Left Top (x, y);  //rrbx, rrby: Right-eye Right Bottom (x, y);
            rltx = (realTopLeftPointRightEye.x >= layoutWidth) ?
                    realTopLeftPointRightEye.x - layoutWidth :
                    realTopLeftPointRightEye.x;
            rrbx = (bottomRightPointRightEye.x >= layoutWidth) ?
                    bottomRightPointRightEye.x - layoutWidth :
                    bottomRightPointRightEye.x;
            System.out.println(
                    "Right Eye: TopLeft.x , TopLeft.y, Width, Height=  " + rltx
                            + " , " + realTopLeftPointRightEye.y + " , " + rrbx
                            + " , " + bottomRightPointRightEye.y);
            c2.drawRect(rltx, realTopLeftPointRightEye.y, rrbx,
                    bottomRightPointRightEye.y, paint);
            //canvas.drawRect(realTopLeftPointRightEye.x, realTopLeftPointRightEye.y, bottomRightPointRightEye.x, bottomRightPointRightEye.y, paint);
        }
        //----------------end right eye
        canvas.drawBitmap(b1, 0, 0, paint);
        canvas.drawBitmap(b2, layoutWidth, 0, paint);
        canvas.save(Canvas.ALL_SAVE_FLAG);
        canvas.restore();
    }

    private void calibrateInsideDrawCircle(Canvas canvas) {
        Paint paint = new Paint();
        paint.setAntiAlias(true);
        paint.setStrokeWidth(5);
        Point topLeftPointLeftEye;
        Point bottomRightPointLeftEye;
        Point topLeftPointRightEye;
        Point bottomRightPointRightEye;

        for (Map.Entry entry : circleSets.entrySet()) {
            Object key = entry.getKey();
            int[] circleData = circleSets.get(key);
            int[] circleLR = circleSetsLR.get(key);
            topLeftPointLeftEye = new Point(circleLR[0], circleData[1]);
            bottomRightPointLeftEye = new Point(circleLR[1], circleData[3]);
            topLeftPointRightEye = new Point(circleData[0], circleData[1]);
            bottomRightPointRightEye = new Point(circleData[2], circleData[3]);
            paint.setColor(COLOR_CIRCLE);

            drawFilledCircle(canvas, paint, topLeftPointLeftEye,
                    bottomRightPointLeftEye, topLeftPointRightEye,
                    bottomRightPointRightEye);
        }
    }

    private void drawFilledCircle(Canvas canvas, Paint paint,
            Point topLeftPointLeftEye, Point bottomRightPointLeftEye,
            Point topLeftPointRightEye, Point bottomRightPointRightEye) {
        paint.setStyle(Paint.Style.FILL);
        Bitmap b1 = Bitmap.createBitmap(layoutWidth, layoutHeight,
                Bitmap.Config.ARGB_8888);
        Bitmap b2 = Bitmap.createBitmap(layoutWidth, layoutHeight,
                Bitmap.Config.ARGB_8888);

        //---------------left eye
        Point realBottomRightPointLeftEye = bottomRightPointLeftEye;
        if (topLeftPointLeftEye.x < layoutWidth) {

            if (bottomRightPointLeftEye.x >= layoutWidth) {
                realBottomRightPointLeftEye = new Point(layoutWidth - 1,
                        bottomRightPointLeftEye.y);
            }
            int cx =
                    (topLeftPointLeftEye.x + realBottomRightPointLeftEye.x) / 2;
            int cy =
                    (topLeftPointLeftEye.y + realBottomRightPointLeftEye.y) / 2;
            int radius = Math.abs(
                    topLeftPointLeftEye.x - realBottomRightPointLeftEye.x);
            System.out.println(
                    "Left Eye:  Cx, Cy, Radius =  " + cx + " , " + cy + " , "
                            + radius);
            Canvas c1 = new Canvas(b1);
            c1.drawCircle(cx, cy, radius, paint);
        }
        //---------------end left eye
        //------------------right eye
        Point realTopLeftPointRightEye = topLeftPointRightEye;

        if (bottomRightPointRightEye.x >= layoutWidth) {

            if (topLeftPointRightEye.x < layoutWidth) {
                realTopLeftPointRightEye = new Point(layoutWidth,
                        topLeftPointRightEye.y);
            }
            Canvas c2 = new Canvas(b2);
            int rltx = -1, rrbx =
                    -1;//rltx, rlty: Right-eye Left Top (x, y);  //rrbx, rrby: Right-eye Right Bottom (x, y);
            rltx = (realTopLeftPointRightEye.x >= layoutWidth) ?
                    realTopLeftPointRightEye.x - layoutWidth :
                    realTopLeftPointRightEye.x;
            rrbx = (bottomRightPointRightEye.x >= layoutWidth) ?
                    bottomRightPointRightEye.x - layoutWidth :
                    bottomRightPointRightEye.x;
            int cx = (rltx + rrbx) / 2;
            int cy = (realTopLeftPointRightEye.y + bottomRightPointRightEye.y)
                    / 2;
            int radius = Math.abs(realTopLeftPointRightEye.y - cy);
            System.out.println(
                    "Right Eye:  Cx, Cy, Radius =  " + cx + " , " + cy + " , "
                            + radius);
            c2.drawCircle(cx, cy, radius, paint);
        }
        //----------------end right eye
        canvas.drawBitmap(b1, 0, 0, paint);
        canvas.drawBitmap(b2, layoutWidth, 0, paint);
        canvas.save(Canvas.ALL_SAVE_FLAG);
        canvas.restore();
    }

    private void calibrateOutsideDraw(Canvas canvas) {
        Paint paint = new Paint();
        paint.setAntiAlias(true);
        paint.setColor(Color.RED);
        paint.setStrokeWidth(5);
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
