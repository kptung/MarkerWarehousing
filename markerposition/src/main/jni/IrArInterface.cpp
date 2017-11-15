#include "stdafx.h"
#include "IrArInterface.h"
#include "IrArCalibration.hpp"
#include "IrArucoMarker.hpp"
#include "ippe.h"
#include "aruco/aruco.hpp"


using namespace std;
using namespace cv;

static IrArCalibration3d calib3d;

std::vector<cv::Point2f> findInjection(const std::vector<cv::Point3f>& objpts, const cv::Mat& rvec, const cv::Mat& tvec, const int &ori, const cv::Point2f &center)
{
	const Mat &intrinsic = calib3d.getIntrinsicMatrix();
	const Mat &distortion = calib3d.getDistortionMatrix();
	std::vector<cv::Point2f> pts, outpts;
	cv::projectPoints(objpts, rvec, tvec, intrinsic, distortion, pts);
	cv::Point2f outPoint;
	if (ori > 0)
	{
		for (int i = 0; i < pts.size(); i++)
		{
			cv::Point2f inpoint = pts.at(i) - center; 
			// rotate point by clockwise
			if (ori == 90)
				outPoint = cv::Point2f(inpoint.y, -inpoint.x);
			else if (ori == 180)
				outPoint = cv::Point2f(-inpoint.x, -inpoint.y);
			else if (ori == 270)
				outPoint = cv::Point2f(-inpoint.y, inpoint.x);
			outPoint += center;
		}	
		outpts.push_back(outPoint);
		return outpts;
		//return pts;
	}
	else
		return pts;
}

/************************************************************************/
/*         Aruco marker detection                                 */
/************************************************************************/
bool findArucoMarkers(const cv::Mat &image, const float& markerLen, std::vector<IrArucoMarker> &markers)
{
	// (a) convert bgr -> gray
	cv::Mat gray, origin;
	image.copyTo(gray);
	image.copyTo(origin);
	cvtColor(gray, gray, COLOR_BGR2GRAY);
	cv::bitwise_not(gray, gray);
	
	// (b) camera intrinsic matrices
	const Mat &intrinsic = calib3d.getIntrinsicMatrix();
	const Mat &distortion = calib3d.getDistortionMatrix();
	const cv::Ptr<cv::aruco::DetectorParameters> &detectparas = calib3d.getDetectparas();
	// (c) define aruco dictionary
	cv::Ptr<cv::aruco::Dictionary> dictionary =
		cv::aruco::getPredefinedDictionary(cv::aruco::PREDEFINED_DICTIONARY_NAME(cv::aruco::DICT_ARUCO_ORIGINAL));
	// (d) detect the markers
	std::vector< int > ids;
	std::vector< std::vector< cv::Point2f > > corners, rejected;
	std::vector< cv::Vec3d > rvecs, tvecs;
	// detect markers and estimate pose
	cv::aruco::detectMarkers(gray, dictionary, corners, ids, detectparas, rejected, intrinsic, distortion);
	//cv::aruco::drawDetectedMarkers(origin, corners, ids);
	// estimate the camera pose; the unit is meter
	if (ids.size() > 0)
	{
		std::vector<IrArucoMarker> _markers(ids.size());
		for (int i = 0; i < ids.size(); i++)
		{
			// (e) get marker orientation
			cv::Point2f cent(0, 0);
			for (int p = 0; p < 4; p++)
				cent += corners.at(i).at(p);
			cent = cent / 4.;
			if (corners.at(i).at(0).x > cent.x && corners.at(i).at(0).y < cent.y)
				_markers[i].setMarkerOri(90);
			else if (corners.at(i).at(0).x > cent.x && corners.at(i).at(0).y > cent.y)
				_markers[i].setMarkerOri(180);
			else if (corners.at(i).at(0).x < cent.x && corners.at(i).at(0).y > cent.y)
				_markers[i].setMarkerOri(270);
			else
				_markers[i].setMarkerOri(0);

			// (f) estimate the camera pose; the unit is meter
			//_markers[i].setDetMethod(1);
			_markers[i].setMarkerId(ids.at(i));
			_markers[i].setMarkerLen(markerLen);
			_markers[i].setCorners(corners.at(i));
			_markers[i].setMarkerCenter(cent);
			cv::aruco::estimatePoseSingleMarkers(corners, markerLen, intrinsic, distortion, rvecs, tvecs);
			// Calculate the camera pose
			cv::Mat R;
			cv::Rodrigues(rvecs[i], R);
			cv::Mat cameraPose = -R.t() * (Mat)tvecs[i];
			_markers[i].setCameraPos(cameraPose);
			_markers[i].setRotationMatrix(rvecs[i]);
			_markers[i].setTransnslationMatrix(tvecs[i]);
			
			//cv::aruco::drawAxis(origin, intrinsic, distortion, rvecs[i], tvecs[i], markerLen * 0.5f);
			
			markers.push_back(_markers[i]);
		}
	}

	return true;
}

const cv::Mat &getCameraIntrinsicMatrix( void )
{
	return calib3d.getIntrinsicMatrix();
}
const cv::Mat &getCameraDistortionMatrix( void )
{
	return calib3d.getDistortionMatrix();
}

/************************************************************************/
/*  read camera parameters from YML                                     */
/************************************************************************/
bool importYMLCameraParameters(const std::string &filename)
{
	return calib3d.loadCameraParametersFromYML(filename);
}
bool importYMLCameraParameters(const std::string &filename, cv::Mat &camMatrix, cv::Mat &distCoeffs)
{
	return calib3d.loadCameraParametersFromYML(filename, camMatrix, distCoeffs);
}

/************************************************************************/
/* aruco read detection params                                          */
/************************************************************************/
bool importYMLDetectParameters(const std::string &filename, cv::Ptr<cv::aruco::DetectorParameters> &params)
{
	return calib3d.readDetectParameters(filename, params);
}
bool importYMLDetectParameters(const std::string &filename)
{
	return calib3d.readDetectParameters(filename);
}
//////////////////////////////////////////////////////////////////////////


//// //// ************ //// ////
//// ////  JNI_Common  //// ////
//// //// ************ //// ////
#ifdef ANDROID
void GetJStringContent(JNIEnv *env, jstring AStr, std::string &ARes) {
    if (!AStr) {
        ARes.clear();
        return;
    }
    const char *str = env->GetStringUTFChars(AStr, NULL);
    ARes = str;
    env->ReleaseStringUTFChars(AStr, str);
}
#endif

