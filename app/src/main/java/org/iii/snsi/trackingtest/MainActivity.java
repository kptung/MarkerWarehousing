package org.iii.snsi.trackingtest;

import android.Manifest;
import android.app.Activity;
import android.content.pm.PackageManager;
import android.os.Build;
import android.os.Bundle;
import android.support.v4.app.ActivityCompat;
import android.support.v4.content.ContextCompat;
import android.view.SurfaceHolder;
import android.view.SurfaceView;
import android.widget.FrameLayout;
import android.widget.TextView;

import com.epson.moverio.btcontrol.DisplayControl;

import org.iii.snsi.drawer.DrawStereoRect2D;
import org.iii.snsi.streamlibrary.CameraController;
import org.iii.snsi.markerposition.IrArucoMarker;

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
    private FrameLayout cameraLayout;
    private DrawStereoRect2D drawer;
    private DisplayControl bt300Control;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        mMarkerInfoText = (TextView) findViewById(R.id.marker_id);
        cameraController = new CameraController(this);
        surfaceView = (SurfaceView) findViewById(R.id.camera_view);

        if (Build.MODEL.contains("EMBT3C")) {
            bt300Control = new DisplayControl(this);
            bt300Control.setMode(DisplayControl.DISPLAY_MODE_3D, false);
        }

        initializeDrawer();

        cameraLayout = (FrameLayout) findViewById(R.id.camera_layout);
        cameraLayout.addView(drawer);

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

                if (ENABLE_PREVIEW) {
                    cameraController.setSurfaceHolder(holder);
                }
                cameraController.startCamera();
                cameraController.setCallbackFrameListener(
                        new CameraController.CallbackFrameListener() {
                            @Override
                            public void onIncomingCallbackFrame(byte[] bytes,
                                    int width, int height) {
                                sleep(10);
                                drawInjectionArea(bytes, width, height);
                            }
                        });
            }

            @Override
            public void surfaceChanged(SurfaceHolder holder, int format,
                    int width, int height) {

            }

            @Override
            public void surfaceDestroyed(SurfaceHolder holder) {
                if (cameraController != null) {
                    cameraController.stopCamera();
                }
            }
        });

    }

    private void drawInjectionArea(byte[] bytes, int width, int height) {
        IrArucoMarker[] find666MarkersPointA;
        IrArucoMarker[] find666MarkersPointB;
        IrArucoMarker[] find777MarkersPointA;
        IrArucoMarker marker666A = null;
        IrArucoMarker marker666B = null;
        IrArucoMarker marker777A = null;
        IrArucoMarker marker777B = null;
        find666MarkersPointA = MarkerHelper.nFindArucoMarkersWithMarkerSize(
                bytes, width, height, 0.03f, -0.04f);
        find666MarkersPointB = MarkerHelper.nFindArucoMarkersWithMarkerSize(
                bytes, width, height, 0.03f, -0.065f);
        find777MarkersPointA = MarkerHelper.nFindArucoMarkersWithMarkerSize(
                bytes, width, height, 0.03f, 0.0f);

        if (find666MarkersPointA.length > 0) {
            for (int i = 0; i < find666MarkersPointA.length; i++) {
                if (find666MarkersPointA[i].id == 666) {
                    marker666A = find666MarkersPointA[i];
                }
            }
        }

        if (find666MarkersPointB.length > 0) {
            for (int i = 0; i < find666MarkersPointB.length; i++) {
                if (find666MarkersPointB[i].id == 666) {
                    marker666B = find666MarkersPointB[i];
                }
            }
        }

        if (find777MarkersPointA.length > 0) {
            for (int i = 0; i < find777MarkersPointA.length; i++) {
                if (find777MarkersPointA[i].id == 777) {
                    marker777A = find777MarkersPointA[i];
                }
            }
        }

        int[] drawInfo = new int[10];
        drawInfo[0] = 0;
        drawInfo[5] = 1;
        if (marker666A != null && marker666B != null) {
            drawInfo[1] = (int) Math.round(marker666A.injectpoints[0].x - 400);
            drawInfo[2] = (int) Math.round(marker666A.injectpoints[0].y);
            drawInfo[3] = 800;
            drawInfo[4] = (int) Math.round(marker666B.injectpoints[0].y
                    - marker666A.injectpoints[0].y);
        } else {
            drawInfo[1] = -1;
            drawInfo[2] = -1;
            drawInfo[3] = 0;
            drawInfo[4] = 0;
        }

        if (marker777A != null) {
            drawInfo[6] = (int) Math.round(marker777A.injectpoints[0].x - 25);
            drawInfo[7] = (int) Math.round(marker777A.injectpoints[0].y - 50);
            drawInfo[8] = 200;
            drawInfo[9] = 50;
        } else {
            drawInfo[6] = -1;
            drawInfo[7] = -1;
            drawInfo[8] = 0;
            drawInfo[9] = 0;
        }

        drawer.processTrackingRect(width, height, drawInfo);
        drawer.postInvalidate();

    }

    private void initializeDrawer() {
        drawer = new DrawStereoRect2D(this);
        drawer.setTrackingCalibration(91, 90, 92, 53);
        drawer.setLayoutSize(1280, 720);
        drawer.setOffsetLR(26);
    }

    private void requestPermission() {
        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.M) {
            requestPermission(Manifest.permission.WRITE_EXTERNAL_STORAGE,
                    REQUEST_WRITE_STORAGE);
            requestPermission(Manifest.permission.READ_EXTERNAL_STORAGE,
                    REQUEST_READ_STORAGE);
            requestPermission(Manifest.permission.CAMERA, REQUEST_CAMERA);
            requestPermission(Manifest.permission.ACCESS_COARSE_LOCATION,
                    REQUEST_COARSE_LOCATION);
        }
    }

    private void requestPermission(String permissionType, int requestCode) {
        boolean hasPermission = (ContextCompat.checkSelfPermission(this,
                permissionType) == PackageManager.PERMISSION_GRANTED);
        if (!hasPermission) {
            ActivityCompat.requestPermissions(MainActivity.this,
                    new String[] {permissionType}, requestCode);
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
