#include "IrArInterface.h"
#include "IrArCalibration.hpp"
#include "IrArucoMarker.hpp"
#include "aruco/aruco.hpp"
#include "IrArDetector.hpp"
#include "IrArGlobalMethods.hpp"

using namespace std;
using namespace cv;

static IrArCalibration3d calib3d;
static IrArDetector mdet;

std::vector<cv::Point2f> findInjection(const std::vector<cv::Point3f>& objpts, const cv::Mat& rvec, const cv::Mat& tvec, const int &ori, const cv::Point2f &center)
{
	const cv::Mat &intrinsic = calib3d.getIntrinsicMatrix();
	const cv::Mat &distortion = calib3d.getDistortionMatrix();
	if (intrinsic.empty() || distortion.empty())
	{
#ifdef ANDROID
		LOGD("Lib needs parameters. Please check intrinsic and distortion");
#endif
		return make_vector<cv::Point2f>() << cv::Point2f();
	}
	if (objpts.size() == 0)
		return make_vector<cv::Point2f>();
	return mdet.findInjectPoints(objpts, intrinsic, distortion, rvec, tvec, ori, center);
}

/************************************************************************/
/*         Aruco marker detection                                       */
/*         Advanced and Application Mode                                */
/************************************************************************/
bool findArucoMarkers(const cv::Mat &image, const float& markerLength, std::vector<IrArucoMarker> &markers)
{
	// (0) convert markerLength from centimeter(cm) to meter(m) 
	float markerLen = markerLength;
	markerLen = (markerLen - (int)markerLen == 0) ? markerLen / 100 :  markerLen;

	// (1) processing
	if (image.rows == 0 || image.cols == 0)
	{
#ifdef ANDROID
		LOGD("Image Fail. Please check it!!");
#endif
		return false;
	}
	// (a) convert bgr -> gray
	cv::Mat gray, origin;
	image.copyTo(gray);
#ifndef ANDROID
	image.copyTo(origin);
#endif
	cv::cvtColor(gray, gray, COLOR_BGR2GRAY);

	/************************************************************************/
	/* Important!! convert black content 2 white content since detect algo. */
	/* can only detect the white content in the marker                      */
	/************************************************************************/
	cv::bitwise_not(gray, gray);
	/************************************************************************/

	// (b) camera intrinsic matrices
	const cv::Mat &intrinsic = calib3d.getIntrinsicMatrix();
	const cv::Mat &distortion = calib3d.getDistortionMatrix();
	cv::Ptr<cv::aruco::Dictionary> dictionary = calib3d.getDictionary();
	if (dictionary->bytesList.rows == 0)
		dictionary = cv::aruco::getPredefinedDictionary(16);
	if (intrinsic.empty() || distortion.empty())
	{
#ifdef ANDROID
		LOGD("Lib needs parameters. Please check intrinsic and distortion");
#endif
		return false;
	}
	// (c) detect marker
#ifdef ANDROID
	return mdet.findArMarkers(gray, markerLen, markers, intrinsic, distortion, dictionary);
#endif
	return mdet.findArMarkers(origin, gray, markerLen, markers, intrinsic, distortion, dictionary);

}

/************************************************************************/
/*         Aruco marker detection                                       */
/*         Basic Mode                                                   */
/************************************************************************/
bool findArucoMarkers(const cv::Mat &image, std::vector<IrArucoMarker> &markers)
{
	// (1) processing
	if (image.rows == 0 || image.cols == 0)
	{
#ifdef ANDROID
		LOGD("Image Fail. Please check it!!");
#endif
		return false;
	}
	// (a) convert bgr -> gray
	cv::Mat gray, origin;
	image.copyTo(gray);
#ifndef ANDROID
	image.copyTo(origin);
#endif
	cv::cvtColor(gray, gray, COLOR_BGR2GRAY);
	/************************************************************************/
	/* Important!! convert black content 2 white content since detect algo. */
	/* can only detect the white content in the marker                      */
	/************************************************************************/
	cv::bitwise_not(gray, gray); 
	/************************************************************************/

	// (b) load dictionary
	cv::Ptr<cv::aruco::Dictionary> dictionary = calib3d.getDictionary();
	if(dictionary->bytesList.rows==0)
		dictionary = cv::aruco::getPredefinedDictionary(16);
	
	// (c) detect marker
#ifdef ANDROID
	return mdet.findArMarkers(gray, markers, dictionary);
#endif
	return mdet.findArMarkers(origin, gray, markers, dictionary);

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

/************************************************************************/
/* aruco read dictionary                                           */
/************************************************************************/
bool importYMLDict(const std::string &filename)
{
	return calib3d.readDictionary(filename);
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

