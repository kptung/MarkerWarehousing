package org.iii.snsi.markertest;

import android.app.Activity;
import android.content.res.Resources;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.util.DisplayMetrics;

import java.io.File;

/**
 * Created by liting on 2017/4/14.
 */
public class ImageUtil {
    public static final int LOAD_IMAGE_RETRY = 2;

    // --------------------------------------------------
    // Scale / InSampleSize
    // --------------------------------------------------

    /**
     * Calculate and round up the scale factor<br>
     * Scale factor is used for down-sampling when loading an image,
     * so the  image will be closest to with the requested h,w
     */
    public static int calculateScaleFactor(int imageHeight, int imageWidth,
            int targetHeight, int targetWidth) {
        int scale = 1;
        if (targetHeight > 0 && targetWidth == 0) {
            if (imageHeight > targetHeight) {
                scale = (int) ((float) imageHeight / (float) targetHeight);
            }
        } else if (targetWidth > 0 && targetHeight == 0) {
            if (imageWidth > targetWidth) {
                scale = (int) ((float) imageWidth / (float) targetWidth);
            }
        } else {
            if (imageHeight > targetHeight || imageWidth > targetWidth) {
                float heightRatio = (float) imageHeight / (float) targetHeight;
                float widthRatio = (float) imageWidth / (float) targetWidth;
                scale = (int) ((heightRatio > widthRatio) ?
                        heightRatio : widthRatio);
            }
        }

        return (scale < 1) ? 1 : scale;
    }

    public static int calculateScaleFactor(String filepath,
            int targetHeight, int targetWidth) {
        int scale = 1;
        boolean fileExists = filepath != null && new File(filepath).exists();
        if (fileExists && targetHeight > 0 && targetWidth > 0) {
            BitmapFactory.Options options = new BitmapFactory.Options();
            options.inJustDecodeBounds = true;
            BitmapFactory.decodeFile(filepath, options);
            scale = calculateScaleFactor(options.outHeight, options.outWidth,
                    targetHeight, targetWidth);
        }
        return scale;
    }

    public static int calculateScaleFactor(Resources res, int resId,
            int targetHeight, int targetWidth) {
        int scale = 1;
        if (res != null && targetHeight > 0 && targetWidth > 0) {
            BitmapFactory.Options options = new BitmapFactory.Options();
            options.inJustDecodeBounds = true;
            BitmapFactory.decodeResource(res, resId, options);
            scale = calculateScaleFactor(options.outHeight, options.outWidth,
                    targetHeight, targetWidth);
        }
        return scale;
    }

    // --------------------------------------------------
    // Load bitmap from res with retry
    // --------------------------------------------------
    public static Bitmap loadBitmap(Resources res, int resId) {
        return loadBitmapMain(res, resId, 1, LOAD_IMAGE_RETRY);
    }

    public static Bitmap loadBitmap(Resources res, int resId, int scale) {
        return loadBitmapMain(res, resId, scale, LOAD_IMAGE_RETRY);
    }

    public static Bitmap loadBitmap(Resources res, int resId, int targetHeight,
            int targetWidth) {
        int scale = calculateScaleFactor(res, resId, targetHeight, targetWidth);
        return loadBitmapMain(res, resId, scale, LOAD_IMAGE_RETRY);
    }

    public static Bitmap loadBitmapMain(Resources res, int resId, int scale,
            int retry) {
        BitmapFactory.Options options = new BitmapFactory.Options();
        options.inSampleSize = scale;
        options.inJustDecodeBounds = false;

        Bitmap b = null;
        int maxTries = retry + 1;
        int tries = 0;
        while (b == null) {
            try {
                b = BitmapFactory.decodeResource(res, resId, options);
            } catch (OutOfMemoryError e) {
                scale *= 2;
                options.inSampleSize = scale;
            }
            tries++;
            if (tries >= maxTries) {
                break;
            }
        }
        return b;
    }

    // --------------------------------------------------
    // Load bitmap from filepath with retry
    // --------------------------------------------------
    public static Bitmap loadBitmap(String filepath) {
        return loadBitmapMain(filepath, 1, LOAD_IMAGE_RETRY);
    }

    public static Bitmap loadBitmap(String filepath, int scale) {
        return loadBitmapMain(filepath, scale, LOAD_IMAGE_RETRY);
    }

    public static Bitmap loadBitmap(String filepath, int targetHeight,
            int targetWidth) {
        int scale = calculateScaleFactor(filepath, targetHeight, targetWidth);
        return loadBitmapMain(filepath, scale, LOAD_IMAGE_RETRY);
    }

    public static Bitmap loadBitmapMain(String filepath, int scale, int retry) {
        boolean fileExists = filepath != null && new File(filepath).exists();
        if (!fileExists) {
            return null;
        }

        BitmapFactory.Options options = new BitmapFactory.Options();
        options.inSampleSize = scale;
        options.inJustDecodeBounds = false;

        Bitmap b = null;
        int maxTries = retry + 1;
        int tries = 0;
        while (b == null) {
            try {
                b = BitmapFactory.decodeFile(filepath, options);
            } catch (OutOfMemoryError e) {
                scale *= 2;
                options.inSampleSize = scale;
            }
            tries++;
            if (tries >= maxTries) {
                break;
            }
        }
        return b;
    }

    // --------------------------------------------------
    // Load bitmap from filepath with retry
    // --------------------------------------------------
    public static Bitmap loadScaledBitmap(String filepath, int targetHeight,
            int targetWidth) {
        if (filepath == null || !new File(filepath).exists()) {
            return null;
        }

        int scaledHeight;
        int scaledWidth;
        BitmapFactory.Options options = new BitmapFactory.Options();
        options.inJustDecodeBounds = true;
        BitmapFactory.decodeFile(filepath, options);
        int imageHeight = options.outHeight;
        int imageWidth = options.outWidth;

        if (imageWidth == imageHeight) { // Square
            if (targetHeight < targetWidth) {
                scaledWidth = targetHeight;
                scaledHeight = targetHeight;
            } else if (targetWidth < targetHeight) {
                scaledWidth = targetWidth;
                scaledHeight = targetWidth;
            } else { // targetWidth == targetHeight
                scaledWidth = targetWidth;
                scaledHeight = targetWidth;
            }
        } else if (imageWidth > imageHeight) { // landscape
            scaledWidth = targetWidth;
            float ratio = (float) imageWidth / (float) targetWidth;
            scaledHeight = (int) (imageHeight / ratio);
        } else { // portrait
            scaledHeight = targetHeight;
            float ratio = (float) imageHeight / (float) targetHeight;
            scaledWidth = (int) (imageWidth / ratio);
        }

        Bitmap bitmap = ImageUtil.loadBitmapMain(filepath, 1,
                LOAD_IMAGE_RETRY);
        Bitmap bitmapScaled = Bitmap.createScaledBitmap(bitmap, scaledWidth,
                scaledHeight, false);

        return bitmapScaled;
    }

    public static Bitmap loadScaledBitmap(Activity activity, String filepath,
            float ratioToScreen) {
        DisplayMetrics dm = DeviceUtil.getScreenDimensions(activity);
        int targetHeight = (int) (dm.heightPixels * ratioToScreen);
        int targetWidth = (int) (dm.widthPixels * ratioToScreen);
        return loadScaledBitmap(filepath, targetHeight, targetWidth);
    }
}
