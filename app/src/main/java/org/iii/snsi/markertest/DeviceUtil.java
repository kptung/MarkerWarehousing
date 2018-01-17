package org.iii.snsi.markertest;

import android.app.Activity;
import android.util.DisplayMetrics;

/**
 * Created by liting on 2017/4/14.
 */
public class DeviceUtil {

    public static DisplayMetrics getScreenDimensions(Activity activity) {
        DisplayMetrics metrics = new DisplayMetrics();
        activity.getWindowManager().getDefaultDisplay().getMetrics(metrics);
        return metrics;
    }

    public static int getScreenWidth(Activity activity) {
        return getScreenDimensions(activity).widthPixels;
    }
}
