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
import org.iii.snsi.videotracking.IrArucoMarker;

public class MainActivity extends Activity{

	// debug
	private static final String TAG = "MarkerActivity";
	// marker
	private TextView mMarkerInfoText;
	private boolean mIsAdvanced = true;
	private IrArucoMarker[] fullMarkerSet1;
	private IrArucoMarker[] fullMarkerSet2;
	private SurfaceView surfaceView;
    private FrameLayout cameraLayout;
	private DrawStereoRect2D drawer;
	private DisplayControl bt300Control;
	CameraController cameraController;

	/// camera captured frame
	private byte[] preview;
	private int previewWidth;
	private int previewHeight;

	private static final int REQUEST_CAMERA = 111;
	private static final int REQUEST_WRITE_STORAGE = 112;
	private static final int REQUEST_READ_STORAGE = 113;
	private static final int REQUEST_COARSE_LOCATION = 114;

	@Override
	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		setContentView(R.layout.activity_main);
		mMarkerInfoText = (TextView) findViewById(R.id.marker_id);
		cameraController = new CameraController(this);
		surfaceView = (SurfaceView)findViewById(R.id.camera_view);
        cameraLayout = (FrameLayout)findViewById(R.id.camera_layout);
		if(Build.MODEL.contains("EMBT3C")){
			bt300Control = new DisplayControl(this);
			bt300Control.setMode(DisplayControl.DISPLAY_MODE_3D, false);
		}

		drawer = new DrawStereoRect2D(this);
		drawer.setTrackingCalibration(91,90,92,53);
		drawer.setLayoutSize(1280, 720);
		drawer.setOffsetLR(26);
        cameraLayout.addView(drawer);

		if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.M) {
			requestPermission(Manifest.permission.WRITE_EXTERNAL_STORAGE, REQUEST_WRITE_STORAGE);
			requestPermission(Manifest.permission.READ_EXTERNAL_STORAGE, REQUEST_READ_STORAGE);
			requestPermission(Manifest.permission.CAMERA, REQUEST_CAMERA);
			requestPermission(Manifest.permission.ACCESS_COARSE_LOCATION, REQUEST_COARSE_LOCATION);
		}

		MarkerHelper.initialization();

		final SurfaceHolder surfaceHolder = surfaceView.getHolder();
		surfaceHolder.addCallback(new SurfaceHolder.Callback() {
			@Override
			public void surfaceCreated(SurfaceHolder holder) {
				String[] cameraFormatList = cameraController.getCameraFormatList();
				int index = 0;
				for (int i= 0; i < cameraFormatList.length; ++i)
				{
					if (cameraFormatList[i].contains("640x480")) {
						cameraController.changePreviewFormat(i);
						break;
					}
				}

				cameraController.setSurfaceHolder(holder);
				cameraController.startCamera();
				cameraController.setCallbackFrameListener(
						new CameraController.CallbackFrameListener() {
							@Override
							public void onIncomingCallbackFrame(byte[] bytes,
									int width, int height) {
								sleep(10);
								preview = bytes;


								if (!mIsAdvanced) {
//									basicMarkerSet =
//											IrDetector.findBasicMarkers(
//													bytes, 640, 480);

									runOnUiThread(new Runnable() {
										@Override
										public void run() {
//											IrDetect.printBasicMarkerSet(basicMarkerSet, mMarkerInfoText);
										}
									});
								} else {
									fullMarkerSet1 = MarkerHelper.nFindArucoMarkersWithMarkerSize(
											bytes, width, height, 0.03f, -0.04f);
									fullMarkerSet2 = MarkerHelper.nFindArucoMarkersWithMarkerSize(
											bytes, width, height, 0.03f, -0.065f);

									if(fullMarkerSet1 !=null && fullMarkerSet1.length>0) {
										IrArucoMarker marker0 = fullMarkerSet1[0];
										IrArucoMarker marker1 = fullMarkerSet2[0];
										System.out.println("injectpoints x = " + (int)Math.round(marker0.injectpoints[0].x));
										System.out.println("injectpoints y = " + (int)Math.round(marker1.injectpoints[0].y));
										drawer.processTrackingRect(width, height,
												new int[] {0, (int)Math.round(marker0.injectpoints[0].x -20), (int)Math.round(marker0.injectpoints[0].y), 40, (int)Math.round(marker1.injectpoints[0].y)});
										drawer.postInvalidate();
									}

									runOnUiThread(new Runnable() {
										@Override
										public void run() {
											MarkerHelper.printFullMarkerSet(
													fullMarkerSet1, mMarkerInfoText);
										}
									});
								}

							}
						});
			}

			@Override
			public void surfaceChanged(SurfaceHolder holder, int format,
					int width, int height) {

			}

			@Override
			public void surfaceDestroyed(SurfaceHolder holder) {
				if (cameraController != null)
					cameraController.stopCamera();
			}
		});

	}

	private void requestPermission(String permissionType, int requestCode) {
		boolean hasPermission = (ContextCompat.checkSelfPermission(this,
				permissionType) == PackageManager.PERMISSION_GRANTED);
		if (!hasPermission) {
			ActivityCompat.requestPermissions(MainActivity.this,
					new String[]{permissionType},
					requestCode);
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

		if(Build.MODEL.contains("EMBT3C")){
			bt300Control.setMode(DisplayControl.DISPLAY_MODE_2D, false);
		}
	}

	public void sleep(long time){
		try {
			Thread.sleep(time);
		} catch (InterruptedException e) {
			e.printStackTrace();
		}
	}

}
