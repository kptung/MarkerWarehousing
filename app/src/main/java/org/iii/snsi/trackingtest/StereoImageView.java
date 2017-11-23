package org.iii.snsi.trackingtest;

import android.content.Context;
import android.graphics.Bitmap;
import android.graphics.Canvas;
import android.graphics.Color;
import android.graphics.Paint;
import android.graphics.Rect;
import android.support.annotation.Nullable;
import android.util.AttributeSet;
import android.view.View;

/**
 * Created by liting on 2017/8/1.
 */
public class StereoImageView extends View {
    private Paint paint;
    private Paint paintText;

    private boolean is3dMode = false;

    private int midX;
    private Rect dstFull, dstL, dstR;

    private Bitmap bmp;
    private Rect src;

    private Bitmap bmpOverlay;
    private Rect srcOverlay;
    private boolean showBitmapOverlay;
    private long timeBmpOverlay;
    private int minShowTime = 0;

    private String strToast;
    private long timeToast;
    private int minShowToastTime = 1500;

    public StereoImageView(Context context) {
        super(context);
        init();
    }

    public StereoImageView(Context context,
            @Nullable AttributeSet attrs) {
        super(context, attrs);
        init();
    }

    public StereoImageView(Context context,
            @Nullable AttributeSet attrs, int defStyleAttr) {
        super(context, attrs, defStyleAttr);
        init();
    }

    private void init() {
        paint = new Paint();
        paintText = new Paint(Paint.ANTI_ALIAS_FLAG);
        paintText.setColor(Color.RED);
        paintText.setTextSize(40);
    }

    public void set3dMode(boolean is3dMode) {
        if (this.is3dMode == is3dMode) {
            return;
        }
        this.is3dMode = is3dMode;
        invalidate();
    }

    public void showToast(String str) {
        timeToast = System.currentTimeMillis();
        strToast = str;
        invalidate();
        postDelayed(clearToastRunnable, minShowToastTime);
    }

    public void setBitmap(Bitmap bmp) {
        this.bmp = bmp;
        if (bmp != null) {
            src = new Rect(0, 0, bmp.getWidth(), bmp.getHeight());
        } else {
            src = null;
        }
        invalidate();
    }

    public void setBitmapOverlay(Bitmap bmp) {
        this.bmpOverlay = bmp;
        if (bmpOverlay != null) {
            srcOverlay = new Rect(0, 0, bmp.getWidth(), bmp.getHeight());
            timeBmpOverlay = System.currentTimeMillis();
        } else {
            srcOverlay = null;
        }
        invalidate();
    }

    public void showBitmapOverlay(boolean show) {
        if (show == showBitmapOverlay) {
            return;
        }

        if (!show && minShowTime > 0) {
            if (System.currentTimeMillis() - timeBmpOverlay < minShowTime) {
                // You can post a delayed runnable to this view to schedule
                // hiding bitmap overlay, but since this function is called
                // on every camera frame, we don't do that.
                return;
            }
        }

        showBitmapOverlay = show;
        invalidate();
    }

    public void setMinShowTime(int minShowTime) {
        this.minShowTime = minShowTime;
    }

    @Override
    protected void onDraw(Canvas canvas) {
        super.onDraw(canvas);
        if (is3dMode) {
            if (bmp != null) {
                canvas.drawBitmap(bmp, src, dstL, paint);
                canvas.drawBitmap(bmp, src, dstR, paint);
            }
            if (showBitmapOverlay && bmpOverlay != null) {
                canvas.drawBitmap(bmpOverlay, srcOverlay, dstL, paint);
                canvas.drawBitmap(bmpOverlay, srcOverlay, dstR, paint);
            }
            if (strToast != null) {
                Rect r = new Rect();
                paintText.getTextBounds(strToast, 0, strToast.length(), r);
                float x = (midX - r.width()) / 2;
                float y = (canvas.getHeight()
                        - (paintText.ascent() + paintText.descent())) / 2;
                canvas.drawText(strToast, x, y, paintText);
                canvas.drawText(strToast, x + midX, y, paintText);
            }
        } else {
            if (bmp != null) {
                canvas.drawBitmap(bmp, src, dstFull, paint);
            }
            if (showBitmapOverlay && bmpOverlay != null) {
                canvas.drawBitmap(bmpOverlay, srcOverlay, dstFull, paint);
            }
            if (strToast != null) {
                Rect r = new Rect();
                paintText.getTextBounds(strToast, 0, strToast.length(), r);
                float x = (canvas.getWidth() - r.width()) / 2;
                float y = (canvas.getHeight()
                        - (paintText.ascent() + paintText.descent())) / 2;
                canvas.drawText(strToast, x, y, paintText);
            }
        }
    }

    private Runnable clearToastRunnable = new Runnable() {
        @Override
        public void run() {
            long timePassed = System.currentTimeMillis() - timeToast;
            if (timePassed >= minShowToastTime) {
                strToast = null;
                invalidate();
            } else {
                postDelayed(clearToastRunnable, minShowToastTime - timePassed);
            }
        }
    };

    @Override
    protected void onSizeChanged(int w, int h, int oldw, int oldh) {
        super.onSizeChanged(w, h, oldw, oldh);
        midX = w / 2;
        dstFull = new Rect(0, 0, w, h);
        dstL = new Rect(0, 0, midX, h);
        dstR = new Rect(midX, 0, w, h);
    }
}
