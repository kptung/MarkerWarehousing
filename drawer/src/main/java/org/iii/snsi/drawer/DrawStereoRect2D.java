package org.iii.snsi.drawer;

import android.content.Context;
import android.graphics.Bitmap;
import android.graphics.Canvas;
import android.graphics.Color;
import android.graphics.Paint;

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
    public int[] processTrackingRect(int width, int height, int[] data) {
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

            if ( (topR > layoutHeight) || (bottomR < 0) ||
                    (rightL < 0) || (leftR > (layoutWidth*2)))
            {
                data[i] = -1;
            } else {
                rectSets.put(data[i], new int[] {(int) Math.round(leftR),
                        (int) Math.round(topR), (int) Math.round(rightR),
                        (int) Math.round(bottomR)});
                rectSetsLR.put(data[i], new int[] {(int) Math.round(leftL),
                        (int) Math.round(rightL)});
            }
        }

        return data;
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
        paint.setColor(Color.RED);
        paint.setStyle(Paint.Style.STROKE);
        paint.setStrokeWidth(5);
        for (Map.Entry entry : rectSets.entrySet()) {
            Object key = entry.getKey();
            int[] rectData = rectSets.get(key);
            int[] rectLR = rectSetsLR.get(key);
            //---------------left eye
            if (rectLR[0] < layoutWidth) {
                //left line
                canvas.drawLine(rectLR[0], rectData[1], rectLR[0], rectData[3],
                        paint);

                if (rectLR[1] >= layoutWidth) {
                    //top line
                    canvas.drawLine(rectLR[0], rectData[1], layoutWidth - 1,
                            rectData[1], paint);
                    //bottom line
                    canvas.drawLine(rectLR[0], rectData[3], layoutWidth - 1,
                            rectData[3], paint);
                } else {
                    //top line
                    canvas.drawLine(rectLR[0], rectData[1], rectLR[1],
                            rectData[1], paint);
                    //right line
                    canvas.drawLine(rectLR[1], rectData[1], rectLR[1],
                            rectData[3], paint);
                    //bottom line
                    canvas.drawLine(rectLR[0], rectData[3], rectLR[1],
                            rectData[3], paint);
                }
            }
            //---------------end left eye
            //------------------right eye

            if (rectData[2] >= layoutWidth) {
                //right line
                canvas.drawLine(rectData[2], rectData[1], rectData[2],
                        rectData[3], paint);

                if (rectData[0] < layoutWidth) {
                    //top line
                    canvas.drawLine(layoutWidth, rectData[1], rectData[2],
                            rectData[1], paint);
                    //bottom line
                    canvas.drawLine(layoutWidth, rectData[3], rectData[2],
                            rectData[3], paint);
                } else {
                    //left line
                    canvas.drawLine(rectData[0], rectData[1], rectData[0],
                            rectData[3], paint);
                    //top line
                    canvas.drawLine(rectData[0], rectData[1], rectData[2],
                            rectData[1], paint);
                    //bottom line
                    canvas.drawLine(rectData[0], rectData[3], rectData[2],
                            rectData[3], paint);
                }
            }
            //----------------end left eye
        }
    }

    private void calibrateOutsideDraw(Canvas canvas) {
        Paint paint = new Paint();
        paint.setAntiAlias(true);
        paint.setColor(Color.RED);
        paint.setStyle(Paint.Style.STROKE);
        paint.setStrokeWidth(5);
    }
}
