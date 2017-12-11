package org.iii.snsi.drawer;

import android.content.Context;
import android.view.View;

/**
 * Created by jimchen on 2017/10/31.
 */
public abstract class DrawTrackingRect extends View {

    public DrawTrackingRect(Context context) {
        super(context);
    }

    public abstract void removeTrackingRect(int[] objIDs);

    public abstract void removeTrackingRect();

    public abstract int[] processTrackingRect(int width, int height, double data[]);

    public abstract void setLayoutSize(int width, int height);

    public abstract int[] getScopeROI();

    public abstract void setTrackingCalibration(int offsetWidth, int offsetHeight,
            int roiWidth, int roiHeight);

    public abstract void SetBitmap(byte[] bytes, int width, int height);
}
