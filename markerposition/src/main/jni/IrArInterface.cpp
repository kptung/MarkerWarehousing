#include "stdafx.h"
#include "IrArInterface.h"
#include "IrArCalibration.hpp"
#include "IrArucoMarker.hpp"
#include "ippe.h"
#include "aruco/aruco.hpp"
#include "IrArDetector.hpp"

using namespace std;
using namespace cv;

static IrArCalibration3d calib3d;
static IrArDetector mdet;

std::vector<cv::Point2f> findInjection(const std::vector<cv::Point3f>& objpts, const cv::Mat& rvec, const cv::Mat& tvec, const int &ori, const cv::Point2f &center)
{
	const cv::Mat &intrinsic = calib3d.getIntrinsicMatrix();
	const cv::Mat &distortion = calib3d.getDistortionMatrix();
	return mdet.findInjectPoints(objpts, intrinsic, distortion, rvec, tvec, ori, center);
}

std::vector<cv::Point2f> findInjection(const std::vector<cv::Point3f>& objpts, const int &ori, const cv::Point2f &center)
{
	const cv::Mat &intrinsic = calib3d.getIntrinsicMatrix();
	const cv::Mat &distortion = calib3d.getDistortionMatrix();
	cv::Mat lrvec, rrvec, ltvec, rtvec;
	calib3d.getRTfromDualEyes(lrvec, rrvec, ltvec, rtvec);
	return mdet.findInjectPoints(objpts, intrinsic, distortion, lrvec, ltvec, rrvec, rtvec, ori, center);
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
	// (d) detect marker
	return mdet.findArMarkers(gray, markerLen, markers, intrinsic, distortion, detectparas);

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

/************************************************************************/
/* read RT params from YML                                          */
/************************************************************************/
bool importYMLRTParameters(const std::string &filename)
{
	return calib3d.loadRTParametersFromYML(filename);
}

/************************************************************************/
/* aruco read detection params                                          */
/************************************************************************/
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

