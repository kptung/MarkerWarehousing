package org.iii.snsi.markertest;

import android.Manifest;
import android.app.Activity;
import android.content.pm.PackageManager;
import android.graphics.Bitmap;
import android.os.Build;
import android.os.Bundle;
import android.os.Environment;
import android.support.v4.app.ActivityCompat;
import android.support.v4.content.ContextCompat;
import android.view.MotionEvent;
import android.view.SurfaceHolder;
import android.view.SurfaceView;
import android.view.View;
import android.view.ViewGroup;
import android.view.WindowManager;
import android.widget.FrameLayout;
import android.widget.TextView;

import com.epson.moverio.btcontrol.DisplayControl;

import org.iii.snsi.drawer.DrawOnCameraFrame;
import org.iii.snsi.drawer.DrawStereoRect2D;
import org.iii.snsi.markerposition.IrArucoMarker;
import org.iii.snsi.streamlibrary.CameraController;

import java.io.*;

import java.util.List;


public class MainActivity extends Activity {
    private static final String TAG = "MainActivity";
    private static final String EMBT3C_RESOLUTION = "640x480";
    private static final int REQUEST_CAMERA = 111;
    private static final int REQUEST_WRITE_STORAGE = 112;
    private static final int REQUEST_READ_STORAGE = 113;
    private static final int REQUEST_COARSE_LOCATION = 114;
    private static final boolean ENABLE_PREVIEW = false;
    CameraController cameraController;
    // marker
    private TextView mMarkerInfoText;
    private SurfaceView surfaceView;
    private ViewGroup.LayoutParams camViewParams;
    private StereoImageView stereoImage;
    private FrameLayout cameraLayout;
    private FrameLayout drawerLayout;
    private DrawStereoRect2D drawerStereo;
    private DrawOnCameraFrame drawerCam;
    private DisplayControl bt300Control;
    private boolean modeFlag = false;//true for preview mode; false for stereo mode
    private int originSurfaceWidth;
    private int originSurfaceHeight;
    // marker mode
    private int markermode = 1;//0: basic mode, 1:adv mode, 2:app mode
    // ini info
    private int roih = 53, offsetw = 91, roiw = 92, offseth = 90, offsetwlr = 26;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        mMarkerInfoText = (TextView) findViewById(R.id.marker_id);
        cameraController = new CameraController(this);
        getWindow().setFlags(WindowManager.LayoutParams.FLAG_FULLSCREEN, WindowManager.LayoutParams.FLAG_FULLSCREEN);
        Bitmap bmp = ImageUtil.loadBitmap(getResources(), R.drawable.stage_border);
        surfaceView = (SurfaceView) findViewById(R.id.camera_view);
        stereoImage = (StereoImageView) findViewById(R.id.stereo_image);
        stereoImage.setBitmap(bmp);
        stereoImage.setOnTouchListener(new View.OnTouchListener()
        {
            @Override
            public boolean onTouch(View v, MotionEvent event) {
                if (event.getAction() != MotionEvent.ACTION_DOWN) {
                    return true;
                }
                toggleMode();
                return true;
            }
        }
        );

        if (Build.MODEL.contains("EMBT3C")) {
            bt300Control = new DisplayControl(this);
            bt300Control.setMode(DisplayControl.DISPLAY_MODE_3D, false);
        }

        stereoImage.set3dMode(true);
        initializeDrawer();

        cameraLayout = (FrameLayout) findViewById(R.id.camera_layout);
        drawerLayout = (FrameLayout) findViewById(R.id.drawer_layout);
        drawerLayout.addView(drawerStereo);

        requestPermission();

        MarkerHelper.initialization();

        final SurfaceHolder surfaceHolder = surfaceView.getHolder();
        surfaceHolder.addCallback(new SurfaceHolder.Callback() {
            @Override
            public void surfaceCreated(SurfaceHolder holder) {
                String[] cameraFormatList =
                        cameraController.getCameraFormatList();
                for (int i = 0; i < cameraFormatList.length; ++i) {
                    if (cameraFormatList[i].contains(EMBT3C_RESOLUTION)) {
                        cameraController.changePreviewFormat(i);
                        break;
                    }
                }

                camViewParams = surfaceView.getLayoutParams();
                originSurfaceWidth = camViewParams.width;
                originSurfaceHeight = camViewParams.height;

                //cameraController.setSurfaceHolder(holder);
                cameraController.startCamera();
                cameraController.setCallbackFrameListener(
                        new CameraController.CallbackFrameListener() {
                            @Override
                            public void onIncomingCallbackFrame(byte[] bytes, int width, int height) {
                                // catch the image
                                drawerCam.SetBitmap(bytes, width, height);

                                // draw injection position
                                drawInjectionArea(bytes, width, height);
                                sleep(5);
                            }
                        });

                camViewParams.width = 0;
                camViewParams.height = 0;
                surfaceView.setLayoutParams(camViewParams);

            }

            @Override
            public void surfaceChanged(SurfaceHolder holder, int format, int width, int height) {

            }

            @Override
            public void surfaceDestroyed(SurfaceHolder holder) {
                if (cameraController != null) {
                    cameraController.stopCamera();
                }
            }
        });

    }

    private void toggleMode() {
        modeFlag = !modeFlag;
        if (modeFlag) {
            setMode("EMBT3C", DisplayControl.DISPLAY_MODE_2D, originSurfaceWidth, originSurfaceHeight, drawerStereo, drawerCam);
        } else {
            setMode("EMBT3C", DisplayControl.DISPLAY_MODE_3D, 0, 0, drawerCam, drawerStereo);
        }
    }

    private void setMode(String modelName, int displayMode, int cameraViewWidth, int cameraViewHeight, View removedView, View addedView) {
        if (Build.MODEL.contains(modelName)) {
            bt300Control.setMode(displayMode,
                    false);
        }
        camViewParams.width = cameraViewWidth;
        camViewParams.height = cameraViewHeight;
        surfaceView.setLayoutParams(camViewParams);
        stereoImage.set3dMode((displayMode == DisplayControl.DISPLAY_MODE_2D) ? false : true);
        drawerLayout.removeView(removedView);
        drawerLayout.addView(addedView);
    }

    private void drawInjectionArea(byte[] bytes, int width, int height) {
        if (markermode == 2) {
            System.out.println("Application Mode");
            long t1 = System.currentTimeMillis();
            IrArucoMarker[] findInjections = MarkerHelper.nFindAppMarkers(bytes, width, height, 3);
            long t2 = System.currentTimeMillis();
            long diff = t2 - t1;
            System.out.println("time =  " + diff);

            double[] drawRect = new double[5];
            double[] drawCircle = new double[5];
            drawRect[0] = 0; drawRect[1] = -1; drawRect[2] = -1; drawRect[3] = -1; drawRect[4] = -1;
            drawCircle[0] = 1; drawCircle[1] = -1; drawCircle[2] = -1; drawCircle[3] = -1; drawCircle[4] = -1;

            for (int i = 0; i < findInjections.length; i++) {
                if (findInjections[i].mid == 666) {
                    if (!modeFlag) {
                        drawCircle[3] = Math.abs(findInjections[i].injectpoints[0].y - findInjections[i].injectpoints[1].y);
                        drawCircle[4] = Math.abs(findInjections[i].injectpoints[1].y - findInjections[i].injectpoints[0].y);
                        drawCircle[1] = Math.abs(findInjections[i].injectpoints[1].x - drawCircle[3]) + 40;
                        drawCircle[2] = findInjections[i].injectpoints[0].y;
                        drawerStereo.processTrackingCircle(width, height, drawCircle);

                    } else {
                        drawCircle[1] = findInjections[i].injectpoints[1].x;
                        drawCircle[2] = findInjections[i].injectpoints[1].y;
                        drawCircle[3] = Math.abs(findInjections[i].injectpoints[1].y - findInjections[i].injectpoints[0].y);
                        drawCircle[4] = Math.abs(findInjections[i].injectpoints[1].y - findInjections[i].injectpoints[0].y);
                        drawerCam.processTrackingCircle(width, height, drawCircle);
                        //drawerCam.postInvalidate();
                    }
                }
                if (findInjections[i].mid == 777) {
                    if (!modeFlag) {
                        drawRect[1] = findInjections[i].mcenter.x + 40;
                        drawRect[1] = (drawRect[1] > width) ? width : drawRect[1];
                        drawRect[2] = findInjections[i].injectpoints[1].y;
                        drawRect[3] = 150;
                        drawRect[4] = Math.abs(findInjections[i].injectpoints[1].y-findInjections[i].mcenter.y);
                        drawerStereo.processTrackingRect(width, height, drawRect);

                    } else {
                        drawRect[1] = findInjections[i].mcenter.x;
                        drawRect[2] = findInjections[i].injectpoints[1].y;
                        drawRect[3] = Math.abs(findInjections[i].injectpoints[1].y-findInjections[i].mcenter.y);;
                        drawRect[4] = Math.abs(findInjections[i].injectpoints[1].y-findInjections[i].mcenter.y);
                        drawerCam.processTrackingRect(width, height, drawRect);
                        //drawerCam.postInvalidate();
                    }
                }
            }
            if(findInjections.length==0)
            {
                drawerStereo.processTrackingRect(width, height, drawRect);
                drawerStereo.processTrackingCircle(width, height, drawCircle);
                drawerStereo.postInvalidate();
                drawerCam.processTrackingRect(width, height, drawRect);
                drawerCam.processTrackingCircle(width, height, drawCircle);
                drawerCam.postInvalidate();
            }
            else
            {
                if (!modeFlag)
                {
                    drawerStereo.postInvalidate();
                }
                else
                {
                    drawerCam.postInvalidate();
                }
            }

        }
        else if (markermode == 0) {
            System.out.println("Basic Mode");
            long t1 = System.currentTimeMillis();
            IrArucoMarker[] BasicMarkers = MarkerHelper.nFindBasicMarkers(bytes, width, height, 3);
            long t2 = System.currentTimeMillis();
            long diff = t2 - t1;
            System.out.println("time =  " + diff);
            if (BasicMarkers.length > 0)
            {
                System.out.println("MID, Marker length " + BasicMarkers.length);
                for (int i = 0; i < BasicMarkers.length; i++)
                {
                    System.out.println("Marker " + i + ", MID =  " + BasicMarkers[i].mid);
                }
            }
            drawerStereo.postInvalidate();
            drawerCam.postInvalidate();
        }
        else if(markermode==1)
        {
            System.out.println("Advanced Mode");
            long t1 = System.currentTimeMillis();
            IrArucoMarker[] AdvMarkers = MarkerHelper.nFindAdvMarkers(bytes, width, height, 3);
            long t2 = System.currentTimeMillis();
            long diff = t2 - t1;
            System.out.println("time =  " + diff);
            if (AdvMarkers.length > 0) {
                System.out.println("MID, Marker length " + AdvMarkers.length);
                for (int i = 0; i < AdvMarkers.length; i++) {
                    System.out.println("Marker " + i + ", MID =  " + AdvMarkers[i].mid);
                    System.out.println("Marker " + i + ", Distance =  " + AdvMarkers[i].mdistance);
                    System.out.println("Marker " + i + ", X-Z Distance =  " + AdvMarkers[i].mxzdistance);
                }
            }
            drawerStereo.postInvalidate();
            drawerCam.postInvalidate();
        }
        else {
            System.out.println(" MID = NULL !! No Detected Marker !!");
        }

    }

    private void initializeDrawer() {
        drawerStereo = new DrawStereoRect2D(this);
        String ini_FILE = "/streamer.ini";
        String filename = Environment.getExternalStorageDirectory().getPath() + ini_FILE;
        File f1 = new File(filename);
        if (!f1.exists()) {
            System.out.println("Error!! No parameters. Please check /sdcard/streamer.ini");
            return;
        }

        // read ini by utf-8 and ansi, if the file encoding is not utf-8 or ansi, it will cause crash.
        IniDocument document = new IniDocument(new File(new File("").getAbsoluteFile(), filename).getAbsolutePath()).parse();

        List<String> roiheight = document.get("ROIHeight");
        List<String> offsetwidth = document.get("OffsetWidth");
        List<String> roiwidth = document.get("ROIWidth");
        List<String> offsetheight = document.get("OffsetHeight");
        List<String> offsetwidthlr = document.get("OffsetWidthLR");
        if (roiheight == null || offsetwidth == null || roiwidth == null || offsetheight == null || offsetwidthlr == null) {
            System.out.println(" INI Read Fail !! Check INI !!");
        }
        if (roiheight != null) {
            roih = Integer.parseInt(roiheight.get(0));
        }
        if (offsetwidth != null) {
            offsetw = Integer.parseInt(offsetwidth.get(0));
        }
        if (roiwidth != null) {
            roiw = Integer.parseInt(roiwidth.get(0));
        }
        if (offsetheight != null) {
            offseth = Integer.parseInt(offsetheight.get(0));
        }
        if (offsetwidthlr != null) {
            offsetwlr = Integer.parseInt(offsetwidthlr.get(0));
        }
        drawerStereo.setTrackingCalibration(offsetw, offseth, roiw, roih);
        drawerStereo.setLayoutSize(1280, 720);
        drawerStereo.setOffsetLR(offsetwlr);
        //drawerStereo.setOffsetLR(40);
        drawerCam = new DrawOnCameraFrame(this);
        drawerCam.setLayoutSize(1280, 720);
    }

    private void requestPermission() {
        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.M) {
            requestPermission(Manifest.permission.WRITE_EXTERNAL_STORAGE, REQUEST_WRITE_STORAGE);
            requestPermission(Manifest.permission.READ_EXTERNAL_STORAGE, REQUEST_READ_STORAGE);
            requestPermission(Manifest.permission.CAMERA, REQUEST_CAMERA);
            requestPermission(Manifest.permission.ACCESS_COARSE_LOCATION, REQUEST_COARSE_LOCATION);
        }
    }

    private void requestPermission(String permissionType, int requestCode) {
        boolean hasPermission = (ContextCompat.checkSelfPermission(this, permissionType) == PackageManager.PERMISSION_GRANTED);
        if (!hasPermission) {
            ActivityCompat.requestPermissions(MainActivity.this, new String[]{permissionType}, requestCode);
        }
    }

    @Override
    public void onPause() {
        super.onPause();
        if (cameraController != null) {
            cameraController.stopCamera();
        }
    }

    @Override
    protected void onResume() {
        super.onResume();
    }

    public void onDestroy() {
        super.onDestroy();
        if (cameraController != null) {
            cameraController.stopCamera();
        }
        if (Build.MODEL.contains("EMBT3C")) {
            bt300Control.setMode(DisplayControl.DISPLAY_MODE_2D, false);
        }
    }

    public void sleep(long time) {
        try {
            Thread.sleep(time);
        } catch (InterruptedException e) {
            e.printStackTrace();
        }
    }
}
