package org.iii.snsi.markertest;

import android.Manifest;
import android.app.Activity;
import android.content.pm.PackageManager;
import android.graphics.Bitmap;
import android.graphics.Color;
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
    private CameraController cameraController;
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
    private int markerMode = 2;//0: basic mode, 1:adv mode, 2:app mode
    private int markerSize = 8; // marker size in cm
    private int mid1 = 888, mid2 = 168;
    //private int mid1 = 666, mid2 = 777;
    private int dictMode = 1; //0: pre-defined dictionary, 1: user-defined dictionary
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

        //MarkerHelper.initialization(markerMode);
        MarkerHelper.initialization(markerMode, dictMode);
        //MarkerHelper.initialization(markerMode, 0);

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

        String smode="";
        String sid="";
        String sdis="";
        String smori="";
        String str1="";
        String sdict="";
        if (markerMode == 2) {
            System.out.println("Application Mode");

            double[] markerArea=new double[16];
            markerArea[0]=mid1;markerArea[1]=0;markerArea[2]=-7.5;markerArea[3]=0;
            markerArea[4]=mid1;markerArea[5]=0;markerArea[6]=-10.5;markerArea[7]=0;
            markerArea[8]=mid2;markerArea[9]=8;markerArea[10]=0;markerArea[11]=0;
            markerArea[12]=mid2;markerArea[13]=0;markerArea[14]=8;markerArea[15]=0;

            long t1 = System.currentTimeMillis();
            IrArucoMarker[] appMarkers = MarkerHelper.nFindAppMarkers(bytes, width, height, markerSize, markerArea);
            long t2 = System.currentTimeMillis();
            long diff = t2 - t1;
            System.out.println("time =  " + diff);

            double[] drawRect = new double[5];
            double[] drawCircle = new double[5];
            drawRect[0] = 0; drawRect[1] = -1; drawRect[2] = -1; drawRect[3] = -1; drawRect[4] = -1;
            drawCircle[0] = 1; drawCircle[1] = -1; drawCircle[2] = -1; drawCircle[3] = -1; drawCircle[4] = -1;

            for (int i = 0; i < appMarkers.length; i++)
            {
                if (appMarkers[i].mid == mid1)
                {
                    if (!modeFlag) {
                        drawCircle[3] = Math.abs(appMarkers[i].injectpoints[0].y - appMarkers[i].injectpoints[1].y);
                        drawCircle[4] = Math.abs(appMarkers[i].injectpoints[1].y - appMarkers[i].injectpoints[0].y);
                        drawCircle[1] = Math.abs(appMarkers[i].injectpoints[1].x - drawCircle[3]) + 40;
                        drawCircle[2] = appMarkers[i].injectpoints[0].y;

                        smode="App mode\n";
                        sdict=(dictMode==0)?"pre-defined dictionary\n":"user-defined dictionary\n";
                        sid="ID: "+String.valueOf(appMarkers[i].mid)+"\n";
                        sdis="Distance(cm): "+String.valueOf(appMarkers[i].mxzdistance)+"\n";
                        smori="Marker orientation(degrees): "+String.valueOf(appMarkers[i].mori)+"\n";
                        str1="";
                        if (appMarkers[i].mxzangle > 0 & appMarkers[i].myzangle > 0) str1="Camera Angle(degrees): Right: "+Math.abs(appMarkers[i].mxzangle) + ", Top: " +Math.abs(appMarkers[i].myzangle)+"\n";
                        else if (appMarkers[i].mxzangle < 0 && appMarkers[i].myzangle > 0) str1= "Camera Angle(degrees): Left: " +Math.abs(appMarkers[i].mxzangle)+", Top: " +Math.abs(appMarkers[i].myzangle)+"\n";
                        else if (appMarkers[i].mxzangle > 0 && appMarkers[i].myzangle < 0) str1="Camera Angle(degrees): Right: " +Math.abs(appMarkers[i].mxzangle)+", Bottom: " +Math.abs(appMarkers[i].myzangle)+"\n";
                        else if (appMarkers[i].mxzangle < 0 && appMarkers[i].myzangle < 0) str1="Camera Angle(degrees): Left: " +Math.abs(appMarkers[i].mxzangle)+", Bottom: " +Math.abs(appMarkers[i].myzangle)+"\n";
                        else if (appMarkers[i].mxzangle < 0 && appMarkers[i].myzangle == 0) str1="Camera Angle(degrees): Left: " +Math.abs(appMarkers[i].mxzangle)+", Top: " +Math.abs(appMarkers[i].myzangle)+"\n";
                        else if (appMarkers[i].mxzangle > 0 && appMarkers[i].myzangle == 0) str1="Camera Angle(degrees): Right: " +Math.abs(appMarkers[i].mxzangle)+", Top: " +Math.abs(appMarkers[i].myzangle)+"\n";
                        else if (appMarkers[i].mxzangle == 0 && appMarkers[i].myzangle == 0) str1="Camera Angle(degrees): Front: " +Math.abs(appMarkers[i].mxzangle)+", Front: " +Math.abs(appMarkers[i].myzangle)+"\n";
                        else if (appMarkers[i].mxzangle == 0 && appMarkers[i].myzangle > 0) str1="Camera Angle(degrees): Front: " +Math.abs(appMarkers[i].mxzangle)+", Top: " +Math.abs(appMarkers[i].myzangle)+"\n";
                        else if (appMarkers[i].mxzangle == 0 && appMarkers[i].myzangle < 0) str1="Camera Angle(degrees): Front: " +Math.abs(appMarkers[i].mxzangle)+", Bottom: " +Math.abs(appMarkers[i].myzangle)+"\n";

                        final String sss=smode+sdict+sid+sdis+smori+str1;
                        runOnUiThread(new Runnable() {
                            @Override
                            public void run() {
                                mMarkerInfoText.setText(sss);
                                mMarkerInfoText.setTextColor(Color.WHITE);
                                mMarkerInfoText.setTextSize(15.f);
                            }
                        });

                        drawerStereo.processTrackingCircle(width, height, drawCircle);

                    } else {
                        drawCircle[1] = appMarkers[i].injectpoints[1].x;
                        drawCircle[2] = appMarkers[i].injectpoints[1].y;
                        drawCircle[3] = Math.abs(appMarkers[i].injectpoints[1].y - appMarkers[i].injectpoints[0].y);
                        drawCircle[4] = Math.abs(appMarkers[i].injectpoints[1].y - appMarkers[i].injectpoints[0].y);

                        smode="App mode\n";
                        sdict=(dictMode==0)?"pre-defined dictionary\n":"user-defined dictionary\n";
                        sid="ID: "+String.valueOf(appMarkers[i].mid)+"\n";
                        sdis="Distance(cm): "+String.valueOf(appMarkers[i].mxzdistance)+"\n";
                        smori="Marker orientation(degrees): "+String.valueOf(appMarkers[i].mori)+"\n";
                        str1="";
                        if (appMarkers[i].mxzangle > 0 & appMarkers[i].myzangle > 0) str1="Camera Angle(degrees): Right: "+Math.abs(appMarkers[i].mxzangle) + ", Top: " +Math.abs(appMarkers[i].myzangle)+"\n";
                        else if (appMarkers[i].mxzangle < 0 && appMarkers[i].myzangle > 0) str1= "Camera Angle(degrees): Left: " +Math.abs(appMarkers[i].mxzangle)+", Top: " +Math.abs(appMarkers[i].myzangle)+"\n";
                        else if (appMarkers[i].mxzangle > 0 && appMarkers[i].myzangle < 0) str1="Camera Angle(degrees): Right: " +Math.abs(appMarkers[i].mxzangle)+", Bottom: " +Math.abs(appMarkers[i].myzangle)+"\n";
                        else if (appMarkers[i].mxzangle < 0 && appMarkers[i].myzangle < 0) str1="Camera Angle(degrees): Left: " +Math.abs(appMarkers[i].mxzangle)+", Bottom: " +Math.abs(appMarkers[i].myzangle)+"\n";
                        else if (appMarkers[i].mxzangle < 0 && appMarkers[i].myzangle == 0) str1="Camera Angle(degrees): Left: " +Math.abs(appMarkers[i].mxzangle)+", Top: " +Math.abs(appMarkers[i].myzangle)+"\n";
                        else if (appMarkers[i].mxzangle > 0 && appMarkers[i].myzangle == 0) str1="Camera Angle(degrees): Right: " +Math.abs(appMarkers[i].mxzangle)+", Top: " +Math.abs(appMarkers[i].myzangle)+"\n";
                        else if (appMarkers[i].mxzangle == 0 && appMarkers[i].myzangle == 0) str1="Camera Angle(degrees): Front: " +Math.abs(appMarkers[i].mxzangle)+", Front: " +Math.abs(appMarkers[i].myzangle)+"\n";
                        else if (appMarkers[i].mxzangle == 0 && appMarkers[i].myzangle > 0) str1="Camera Angle(degrees): Front: " +Math.abs(appMarkers[i].mxzangle)+", Top: " +Math.abs(appMarkers[i].myzangle)+"\n";
                        else if (appMarkers[i].mxzangle == 0 && appMarkers[i].myzangle < 0) str1="Camera Angle(degrees): Front: " +Math.abs(appMarkers[i].mxzangle)+", Bottom: " +Math.abs(appMarkers[i].myzangle)+"\n";

                        final String sss=smode+sdict+sid+sdis+smori+str1;
                        runOnUiThread(new Runnable() {
                            @Override
                            public void run() {
                                mMarkerInfoText.setText(sss);
                                mMarkerInfoText.setTextColor(Color.WHITE);
                                mMarkerInfoText.setTextSize(15.f);
                            }
                        });

                        drawerCam.processTrackingCircle(width, height, drawCircle);
                        //drawerCam.postInvalidate();
                    }
                }
                else if (appMarkers[i].mid == mid2) {
                    if (!modeFlag) {
                        drawRect[1] = appMarkers[i].mcenter.x + 40;
                        drawRect[1] = (drawRect[1] > width) ? width : drawRect[1];
                        drawRect[2] = appMarkers[i].injectpoints[1].y;
                        drawRect[3] = 150;
                        drawRect[4] = Math.abs(appMarkers[i].injectpoints[1].y-appMarkers[i].mcenter.y);

                        smode="App mode\n";
                        sdict=(dictMode==0)?"pre-defined dictionary\n":"user-defined dictionary\n";
                        sid="ID: "+String.valueOf(appMarkers[i].mid)+"\n";
                        sdis="Distance(cm): "+String.valueOf(appMarkers[i].mxzdistance)+"\n";
                        smori="Marker orientation(degrees): "+String.valueOf(appMarkers[i].mori)+"\n";
                        str1="";
                        if (appMarkers[i].mxzangle > 0 & appMarkers[i].myzangle > 0) str1="Camera Angle(degrees): Right: "+Math.abs(appMarkers[i].mxzangle) + ", Top: " +Math.abs(appMarkers[i].myzangle)+"\n";
                        else if (appMarkers[i].mxzangle < 0 && appMarkers[i].myzangle > 0) str1= "Camera Angle(degrees): Left: " +Math.abs(appMarkers[i].mxzangle)+", Top: " +Math.abs(appMarkers[i].myzangle)+"\n";
                        else if (appMarkers[i].mxzangle > 0 && appMarkers[i].myzangle < 0) str1="Camera Angle(degrees): Right: " +Math.abs(appMarkers[i].mxzangle)+", Bottom: " +Math.abs(appMarkers[i].myzangle)+"\n";
                        else if (appMarkers[i].mxzangle < 0 && appMarkers[i].myzangle < 0) str1="Camera Angle(degrees): Left: " +Math.abs(appMarkers[i].mxzangle)+", Bottom: " +Math.abs(appMarkers[i].myzangle)+"\n";
                        else if (appMarkers[i].mxzangle < 0 && appMarkers[i].myzangle == 0) str1="Camera Angle(degrees): Left: " +Math.abs(appMarkers[i].mxzangle)+", Top: " +Math.abs(appMarkers[i].myzangle)+"\n";
                        else if (appMarkers[i].mxzangle > 0 && appMarkers[i].myzangle == 0) str1="Camera Angle(degrees): Right: " +Math.abs(appMarkers[i].mxzangle)+", Top: " +Math.abs(appMarkers[i].myzangle)+"\n";
                        else if (appMarkers[i].mxzangle == 0 && appMarkers[i].myzangle == 0) str1="Camera Angle(degrees): Front: " +Math.abs(appMarkers[i].mxzangle)+", Front: " +Math.abs(appMarkers[i].myzangle)+"\n";
                        else if (appMarkers[i].mxzangle == 0 && appMarkers[i].myzangle > 0) str1="Camera Angle(degrees): Front: " +Math.abs(appMarkers[i].mxzangle)+", Top: " +Math.abs(appMarkers[i].myzangle)+"\n";
                        else if (appMarkers[i].mxzangle == 0 && appMarkers[i].myzangle < 0) str1="Camera Angle(degrees): Front: " +Math.abs(appMarkers[i].mxzangle)+", Bottom: " +Math.abs(appMarkers[i].myzangle)+"\n";

                        final String sss=smode+sdict+sid+sdis+smori+str1;
                        runOnUiThread(new Runnable() {
                            @Override
                            public void run() {
                                mMarkerInfoText.setText(sss);
                                mMarkerInfoText.setTextColor(Color.WHITE);
                                mMarkerInfoText.setTextSize(15.f);
                            }
                        });

                        drawerStereo.processTrackingRect(width, height, drawRect);

                    } else {
                        drawRect[1] = appMarkers[i].mcenter.x;
                        drawRect[2] = appMarkers[i].injectpoints[1].y;
                        drawRect[3] = Math.abs(appMarkers[i].injectpoints[1].y-appMarkers[i].mcenter.y);
                        drawRect[4] = Math.abs(appMarkers[i].injectpoints[1].y-appMarkers[i].mcenter.y);

                        smode="App mode\n";
                        sdict=(dictMode==0)?"pre-defined dictionary\n":"user-defined dictionary\n";
                        sid="ID: "+String.valueOf(appMarkers[i].mid)+"\n";
                        sdis="Distance(cm): "+String.valueOf(appMarkers[i].mxzdistance)+"\n";
                        smori="Marker orientation(degrees): "+String.valueOf(appMarkers[i].mori)+"\n";
                        str1="";
                        if (appMarkers[i].mxzangle > 0 & appMarkers[i].myzangle > 0) str1="Camera Angle(degrees): Right: "+Math.abs(appMarkers[i].mxzangle) + ", Top: " +Math.abs(appMarkers[i].myzangle)+"\n";
                        else if (appMarkers[i].mxzangle < 0 && appMarkers[i].myzangle > 0) str1= "Camera Angle(degrees): Left: " +Math.abs(appMarkers[i].mxzangle)+", Top: " +Math.abs(appMarkers[i].myzangle)+"\n";
                        else if (appMarkers[i].mxzangle > 0 && appMarkers[i].myzangle < 0) str1="Camera Angle(degrees): Right: " +Math.abs(appMarkers[i].mxzangle)+", Bottom: " +Math.abs(appMarkers[i].myzangle)+"\n";
                        else if (appMarkers[i].mxzangle < 0 && appMarkers[i].myzangle < 0) str1="Camera Angle(degrees): Left: " +Math.abs(appMarkers[i].mxzangle)+", Bottom: " +Math.abs(appMarkers[i].myzangle)+"\n";
                        else if (appMarkers[i].mxzangle < 0 && appMarkers[i].myzangle == 0) str1="Camera Angle(degrees): Left: " +Math.abs(appMarkers[i].mxzangle)+", Top: " +Math.abs(appMarkers[i].myzangle)+"\n";
                        else if (appMarkers[i].mxzangle > 0 && appMarkers[i].myzangle == 0) str1="Camera Angle(degrees): Right: " +Math.abs(appMarkers[i].mxzangle)+", Top: " +Math.abs(appMarkers[i].myzangle)+"\n";
                        else if (appMarkers[i].mxzangle == 0 && appMarkers[i].myzangle == 0) str1="Camera Angle(degrees): Front: " +Math.abs(appMarkers[i].mxzangle)+", Front: " +Math.abs(appMarkers[i].myzangle)+"\n";
                        else if (appMarkers[i].mxzangle == 0 && appMarkers[i].myzangle > 0) str1="Camera Angle(degrees): Front: " +Math.abs(appMarkers[i].mxzangle)+", Top: " +Math.abs(appMarkers[i].myzangle)+"\n";
                        else if (appMarkers[i].mxzangle == 0 && appMarkers[i].myzangle < 0) str1="Camera Angle(degrees): Front: " +Math.abs(appMarkers[i].mxzangle)+", Bottom: " +Math.abs(appMarkers[i].myzangle)+"\n";

                        final String sss=smode+sdict+sid+sdis+smori+str1;
                        runOnUiThread(new Runnable() {
                            @Override
                            public void run() {
                                mMarkerInfoText.setText(sss);
                                mMarkerInfoText.setTextColor(Color.WHITE);
                                mMarkerInfoText.setTextSize(15.f);
                            }
                        });

                        drawerCam.processTrackingRect(width, height, drawRect);
                        //drawerCam.postInvalidate();
                    }
                }
                else
                {
                    if (!modeFlag) {
                        smode="App mode\n";
                        sdict=(dictMode==0)?"pre-defined dictionary\n":"user-defined dictionary\n";
                        sid="ID: "+String.valueOf(appMarkers[i].mid)+"\n";
                        sdis="Distance(cm): "+String.valueOf(appMarkers[i].mxzdistance)+"\n";
                        smori="Marker orientation(degrees): "+String.valueOf(appMarkers[i].mori)+"\n";
                        str1="";
                        if (appMarkers[i].mxzangle > 0 & appMarkers[i].myzangle > 0) str1="Camera Angle(degrees): Right: "+Math.abs(appMarkers[i].mxzangle) + ", Top: " +Math.abs(appMarkers[i].myzangle)+"\n";
                        else if (appMarkers[i].mxzangle < 0 && appMarkers[i].myzangle > 0) str1= "Camera Angle(degrees): Left: " +Math.abs(appMarkers[i].mxzangle)+", Top: " +Math.abs(appMarkers[i].myzangle)+"\n";
                        else if (appMarkers[i].mxzangle > 0 && appMarkers[i].myzangle < 0) str1="Camera Angle(degrees): Right: " +Math.abs(appMarkers[i].mxzangle)+", Bottom: " +Math.abs(appMarkers[i].myzangle)+"\n";
                        else if (appMarkers[i].mxzangle < 0 && appMarkers[i].myzangle < 0) str1="Camera Angle(degrees): Left: " +Math.abs(appMarkers[i].mxzangle)+", Bottom: " +Math.abs(appMarkers[i].myzangle)+"\n";
                        else if (appMarkers[i].mxzangle < 0 && appMarkers[i].myzangle == 0) str1="Camera Angle(degrees): Left: " +Math.abs(appMarkers[i].mxzangle)+", Top: " +Math.abs(appMarkers[i].myzangle)+"\n";
                        else if (appMarkers[i].mxzangle > 0 && appMarkers[i].myzangle == 0) str1="Camera Angle(degrees): Right: " +Math.abs(appMarkers[i].mxzangle)+", Top: " +Math.abs(appMarkers[i].myzangle)+"\n";
                        else if (appMarkers[i].mxzangle == 0 && appMarkers[i].myzangle == 0) str1="Camera Angle(degrees): Front: " +Math.abs(appMarkers[i].mxzangle)+", Front: " +Math.abs(appMarkers[i].myzangle)+"\n";
                        else if (appMarkers[i].mxzangle == 0 && appMarkers[i].myzangle > 0) str1="Camera Angle(degrees): Front: " +Math.abs(appMarkers[i].mxzangle)+", Top: " +Math.abs(appMarkers[i].myzangle)+"\n";
                        else if (appMarkers[i].mxzangle == 0 && appMarkers[i].myzangle < 0) str1="Camera Angle(degrees): Front: " +Math.abs(appMarkers[i].mxzangle)+", Bottom: " +Math.abs(appMarkers[i].myzangle)+"\n";

                        final String sss=smode+sdict+sid+sdis+smori+str1;
                        runOnUiThread(new Runnable() {
                            @Override
                            public void run() {
                                mMarkerInfoText.setText(sss);
                                mMarkerInfoText.setTextColor(Color.WHITE);
                                mMarkerInfoText.setTextSize(15.f);
                            }
                        });

                        drawerStereo.processTrackingCircle(width, height, drawCircle);

                    } else {
                        smode="App mode\n";
                        sdict=(dictMode==0)?"pre-defined dictionary\n":"user-defined dictionary\n";
                        sid="ID: "+String.valueOf(appMarkers[i].mid)+"\n";
                        sdis="Distance(cm): "+String.valueOf(appMarkers[i].mxzdistance)+"\n";
                        smori="Marker orientation(degrees): "+String.valueOf(appMarkers[i].mori)+"\n";
                        str1="";
                        if (appMarkers[i].mxzangle > 0 & appMarkers[i].myzangle > 0) str1="Camera Angle(degrees): Right: "+Math.abs(appMarkers[i].mxzangle) + ", Top: " +Math.abs(appMarkers[i].myzangle)+"\n";
                        else if (appMarkers[i].mxzangle < 0 && appMarkers[i].myzangle > 0) str1= "Camera Angle(degrees): Left: " +Math.abs(appMarkers[i].mxzangle)+", Top: " +Math.abs(appMarkers[i].myzangle)+"\n";
                        else if (appMarkers[i].mxzangle > 0 && appMarkers[i].myzangle < 0) str1="Camera Angle(degrees): Right: " +Math.abs(appMarkers[i].mxzangle)+", Bottom: " +Math.abs(appMarkers[i].myzangle)+"\n";
                        else if (appMarkers[i].mxzangle < 0 && appMarkers[i].myzangle < 0) str1="Camera Angle(degrees): Left: " +Math.abs(appMarkers[i].mxzangle)+", Bottom: " +Math.abs(appMarkers[i].myzangle)+"\n";
                        else if (appMarkers[i].mxzangle < 0 && appMarkers[i].myzangle == 0) str1="Camera Angle(degrees): Left: " +Math.abs(appMarkers[i].mxzangle)+", Top: " +Math.abs(appMarkers[i].myzangle)+"\n";
                        else if (appMarkers[i].mxzangle > 0 && appMarkers[i].myzangle == 0) str1="Camera Angle(degrees): Right: " +Math.abs(appMarkers[i].mxzangle)+", Top: " +Math.abs(appMarkers[i].myzangle)+"\n";
                        else if (appMarkers[i].mxzangle == 0 && appMarkers[i].myzangle == 0) str1="Camera Angle(degrees): Front: " +Math.abs(appMarkers[i].mxzangle)+", Front: " +Math.abs(appMarkers[i].myzangle)+"\n";
                        else if (appMarkers[i].mxzangle == 0 && appMarkers[i].myzangle > 0) str1="Camera Angle(degrees): Front: " +Math.abs(appMarkers[i].mxzangle)+", Top: " +Math.abs(appMarkers[i].myzangle)+"\n";
                        else if (appMarkers[i].mxzangle == 0 && appMarkers[i].myzangle < 0) str1="Camera Angle(degrees): Front: " +Math.abs(appMarkers[i].mxzangle)+", Bottom: " +Math.abs(appMarkers[i].myzangle)+"\n";

                        final String sss=smode+sdict+sid+sdis+smori+str1;
                        runOnUiThread(new Runnable() {
                            @Override
                            public void run() {
                                mMarkerInfoText.setText(sss);
                                mMarkerInfoText.setTextColor(Color.WHITE);
                                mMarkerInfoText.setTextSize(15.f);
                            }
                        });

                        drawerCam.processTrackingCircle(width, height, drawCircle);
                        //drawerCam.postInvalidate();
                    }
                }
            }

            if(appMarkers.length==0)
            {
                drawerStereo.processTrackingRect(width, height, drawRect);
                drawerStereo.processTrackingCircle(width, height, drawCircle);
                drawerStereo.postInvalidate();
                drawerCam.processTrackingRect(width, height, drawRect);
                drawerCam.processTrackingCircle(width, height, drawCircle);
                drawerCam.postInvalidate();
                sid="";
                str1="";
                sdis="";
                smori="";
                sdict="";
                final String sss=smode+sdict+sid+sdis+smori+str1;
                runOnUiThread(new Runnable() {
                    @Override
                    public void run() {
                        mMarkerInfoText.setText(sss);
                        mMarkerInfoText.setTextColor(Color.WHITE);
                        mMarkerInfoText.setTextSize(15.f);
                    }
                });
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
        else if (markerMode == 0) {
            System.out.println("Basic Mode");
            long t1 = System.currentTimeMillis();
            IrArucoMarker[] basicMarkers = MarkerHelper.nFindBasicMarkers(bytes, width, height);
            long t2 = System.currentTimeMillis();
            long diff = t2 - t1;
            System.out.println("time =  " + diff);
            if (basicMarkers.length > 0)
            {
                System.out.println("MID, Marker length " + basicMarkers.length);
                for (int i = 0; i < basicMarkers.length; i++)
                {
                    System.out.println("Marker " + i + ", MID =  " + basicMarkers[i].mid);

                    smode="Basic mode\n";
                    sdict=(dictMode==0)?"pre-defined dictionary\n":"user-defined dictionary\n";
                    sid="ID: "+String.valueOf(basicMarkers[i].mid)+"\n";
                    smori="Marker orientation(degrees): "+String.valueOf(basicMarkers[i].mori)+"\n";

                    final String sss=smode+sdict+sid+sdis+smori+str1;
                    runOnUiThread(new Runnable() {
                        @Override
                        public void run() {
                            mMarkerInfoText.setText(sss);
                            mMarkerInfoText.setTextColor(Color.WHITE);
                            mMarkerInfoText.setTextSize(15.f);
                        }
                    });


                }
            }

            if(basicMarkers.length==0)
            {
                sid="";
                sdict="";
                str1="";
                sdis="";
                smori="";
                final String sss=smode+sdict+sid+sdis+smori+str1;
                runOnUiThread(new Runnable() {
                    @Override
                    public void run() {
                        mMarkerInfoText.setText(sss);
                        mMarkerInfoText.setTextColor(Color.WHITE);
                        mMarkerInfoText.setTextSize(15.f);
                    }
                });
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

        //    drawerStereo.postInvalidate();
        //    drawerCam.postInvalidate();
        }
        else if(markerMode==1)
        {
            System.out.println("Advanced Mode");
            long t1 = System.currentTimeMillis();
            IrArucoMarker[] advMarkers = MarkerHelper.nFindAdvMarkers(bytes, width, height, markerSize);
            long t2 = System.currentTimeMillis();
            long diff = t2 - t1;
            System.out.println("time =  " + diff);
            if (advMarkers.length > 0) {
                System.out.println("MID, Marker length " + advMarkers.length);
                for (int i = 0; i < advMarkers.length; i++) {
                    System.out.println("Marker " + i + ", MID =  " + advMarkers[i].mid);
                    System.out.println("Marker " + i + ", Distance =  " + advMarkers[i].mdistance);
                    System.out.println("Marker " + i + ", X-Z Distance =  " + advMarkers[i].mxzdistance);

                    smode="Adv mode\n";
                    sid="ID: "+String.valueOf(advMarkers[i].mid)+"\n";
                    sdict=(dictMode==0)?"pre-defined dictionary\n":"user-defined dictionary\n";
                    sdis="Distance(cm): "+String.valueOf(advMarkers[i].mxzdistance)+"\n";
                    smori="Marker orientation(degrees): "+String.valueOf(advMarkers[i].mori)+"\n";
                    str1="";
                    if (advMarkers[i].mxzangle > 0 & advMarkers[i].myzangle > 0) str1="Camera Angle(degrees): Right: "+Math.abs(advMarkers[i].mxzangle) + ", Top: " +Math.abs(advMarkers[i].myzangle)+"\n";
                    else if (advMarkers[i].mxzangle < 0 && advMarkers[i].myzangle > 0) str1= "Camera Angle(degrees): Left: " +Math.abs(advMarkers[i].mxzangle)+", Top: " +Math.abs(advMarkers[i].myzangle)+"\n";
                    else if (advMarkers[i].mxzangle > 0 && advMarkers[i].myzangle < 0) str1="Camera Angle(degrees): Right: " +Math.abs(advMarkers[i].mxzangle)+", Bottom: " +Math.abs(advMarkers[i].myzangle)+"\n";
                    else if (advMarkers[i].mxzangle < 0 && advMarkers[i].myzangle < 0) str1="Camera Angle(degrees): Left: " +Math.abs(advMarkers[i].mxzangle)+", Bottom: " +Math.abs(advMarkers[i].myzangle)+"\n";
                    else if (advMarkers[i].mxzangle < 0 && advMarkers[i].myzangle == 0) str1="Camera Angle(degrees): Left: " +Math.abs(advMarkers[i].mxzangle)+", Top: " +Math.abs(advMarkers[i].myzangle)+"\n";
                    else if (advMarkers[i].mxzangle > 0 && advMarkers[i].myzangle == 0) str1="Camera Angle(degrees): Right: " +Math.abs(advMarkers[i].mxzangle)+", Top: " +Math.abs(advMarkers[i].myzangle)+"\n";
                    else if (advMarkers[i].mxzangle == 0 && advMarkers[i].myzangle == 0) str1="Camera Angle(degrees): Front: " +Math.abs(advMarkers[i].mxzangle)+", Front: " +Math.abs(advMarkers[i].myzangle)+"\n";
                    else if (advMarkers[i].mxzangle == 0 && advMarkers[i].myzangle > 0) str1="Camera Angle(degrees): Front: " +Math.abs(advMarkers[i].mxzangle)+", Top: " +Math.abs(advMarkers[i].myzangle)+"\n";
                    else if (advMarkers[i].mxzangle == 0 && advMarkers[i].myzangle < 0) str1="Camera Angle(degrees): Front: " +Math.abs(advMarkers[i].mxzangle)+", Bottom: " +Math.abs(advMarkers[i].myzangle)+"\n";

                    final String sss=smode+sdict+sid+sdis+smori+str1;
                    runOnUiThread(new Runnable() {
                        @Override
                        public void run() {
                            mMarkerInfoText.setText(sss);
                            mMarkerInfoText.setTextColor(Color.WHITE);
                            mMarkerInfoText.setTextSize(15.f);
                        }
                    });

                }
            }
            if(advMarkers.length==0)
            {
                sid="";
                str1="";
                sdis="";
                smori="";
                sdict="";
                final String sss=smode+sid+sdis+smori+str1;
                runOnUiThread(new Runnable() {
                    @Override
                    public void run() {
                        mMarkerInfoText.setText(sss);
                        mMarkerInfoText.setTextColor(Color.WHITE);
                        mMarkerInfoText.setTextSize(15.f);
                    }
                });
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

           // drawerStereo.postInvalidate();
           // drawerCam.postInvalidate();
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
