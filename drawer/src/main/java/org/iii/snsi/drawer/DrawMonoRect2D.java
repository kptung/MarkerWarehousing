package org.iii.snsi.drawer;

import android.content.Context;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.graphics.Canvas;
import android.graphics.Color;
import android.graphics.ImageFormat;
import android.graphics.Matrix;
import android.graphics.Paint;
import android.graphics.Rect;
import android.graphics.YuvImage;

import java.io.ByteArrayOutputStream;
import java.util.HashMap;
import java.util.Map;

/**
 * Created by randolphchen on 2016/12/30.
 */
public class DrawMonoRect2D extends DrawTrackingRect {

    private boolean dbgObjImg = false;
    private HashMap<Integer, int[]> rectSets;
    private int layoutWidth;
    private int layoutHeight;
    private int offsetWidth;
    private int offsetHeight;
    private int roiWidth;
    private int roiHeight;
    private int[] roiScope;
    private Bitmap bbitmap;

    public DrawMonoRect2D(Context context) {
        super(context);
        rectSets = new HashMap<Integer, int[]>();
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

    public void setDbgObjImg(int flag) {
        if (flag >= 1) {
            dbgObjImg = true;
        } else {
            dbgObjImg = false;
        }
    }

    public void setTrackingCalibration(int offsetWidth, int offsetHeight,
            int roiWidth, int roiHeight) {
        this.offsetWidth = offsetWidth;
        this.offsetHeight = offsetHeight;
        this.roiWidth = roiWidth;
        this.roiHeight = roiHeight;
        // roiScope leftTop x,y rightBottom x,y
        roiScope = new int[4];
        roiScope[0] = offsetWidth;
        roiScope[1] = offsetHeight;
        roiScope[2] = offsetWidth + roiWidth;
        roiScope[3] = offsetHeight + roiHeight;
    }

    public int[] getScopeROI() {
        return roiScope;
    }

    public void removeTrackingRect(int[] objIDs) {
        for (int objID : objIDs) {
            rectSets.remove(objID);
        }
    }

    public void removeTrackingRect() {
        rectSets.clear();
    }

    public int[] processTrackingRect(int width, int height, double data[]) {

        rectSets.clear();
        if (data == null) {
            return null;
        }

        double rateX = width / 320;
        double rateY = height / 240;

        for (int i = 0; i < data.length; i += 5) {
            double left =
                    (double) (data[i + 1] - (offsetWidth * rateX)) / (roiWidth
                            * rateX) * layoutWidth;
            double top =
                    (double) (data[i + 2] - (offsetHeight * rateY)) / (roiHeight
                            * rateY) * layoutHeight;
            double right =
                    (double) (data[i + 1] + data[i + 3] - (offsetWidth * rateX))
                            / (roiWidth * rateX) * layoutWidth;
            double bottom = (double) (data[i + 2] + data[i + 4] - (offsetHeight
                    * rateY)) / (roiHeight * rateY) * layoutHeight;
            if (!dbgObjImg && ((left < 0 || left > layoutWidth) || (top < 0
                    || top > layoutHeight)) && (
                    (right < 0 || right > layoutWidth) || (bottom < 0
                            || bottom > layoutHeight)))
            {
                data[i] = -1;
            } else {
                rectSets.put((int) Math.round(data[i]), new int[] {(int) Math.round(left),
                        (int) Math.round(top), (int) Math.round(right),
                        (int) Math.round(bottom)});
            }
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
    protected void onDraw(Canvas canvas) {
        super.onDraw(canvas);
        Paint paint = new Paint();
        paint.setAntiAlias(true);
        paint.setColor(Color.RED);
        paint.setStyle(Paint.Style.STROKE);
        paint.setStrokeWidth(5);
        for (Map.Entry entry : rectSets.entrySet()) {
            Object key = entry.getKey();
            int[] rectData = rectSets.get(key);
            Rect rect = new Rect(rectData[0], rectData[1], rectData[2],
                    rectData[3]);
            canvas.drawRect(rect, paint);
        }

    }

}
