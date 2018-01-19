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
	if (objpts.size() == 0)
		return make_vector<cv::Point2f>();
	return mdet.findInjectPoints(objpts, intrinsic, distortion, rvec, tvec, ori, center);
}
/************************************************************************/
/*         Aruco marker detection                                 */
/************************************************************************/
bool findArucoMarkers(const cv::Mat &image, const float& markerLength, std::vector<IrArucoMarker> &markers)
{
	// (0) convert markerLength from centimeter(cm) to meter(m) 
	float markerLen = markerLength;
	if (markerLen - (int)markerLen == 0)
		markerLen = markerLen / 100;

	// (1) processing
	if (image.rows == 0 || image.cols == 0)
		return false;
	// (a) convert bgr -> gray
	cv::Mat gray, origin;
	image.copyTo(gray);
	image.copyTo(origin);
	cvtColor(gray, gray, COLOR_BGR2GRAY);
	cv::bitwise_not(gray, gray);
	
	// (b) camera intrinsic matrices
	const cv::Mat &intrinsic = calib3d.getIntrinsicMatrix();
	const cv::Mat &distortion = calib3d.getDistortionMatrix();
	if (intrinsic.empty() || distortion.empty())
	{
		std::cout << "Lib needs parameters. Please check intrinsic and distortion" << endl;
#ifdef ANDROID
		LOGD("Lib needs parameters. Please check intrinsic and distortion");
#endif
		return false;
	}
	const cv::Ptr<cv::aruco::DetectorParameters> &detectparas = calib3d.getDetectparas();
	// (c) define aruco dictionary
	cv::Ptr<cv::aruco::Dictionary> dictionary =
		cv::aruco::getPredefinedDictionary(cv::aruco::PREDEFINED_DICTIONARY_NAME(cv::aruco::DICT_ARUCO_ORIGINAL));
	// (d) detect marker
	return mdet.findArMarkers(origin, gray, markerLen, markers, intrinsic, distortion, detectparas);

}

bool findArucoMarkers(const cv::Mat &image, std::vector<IrArucoMarker> &markers)
{
	// (1) processing
	if (image.rows == 0 || image.cols == 0)
		return false;
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
	return mdet.findArMarkers(origin, gray, markers, intrinsic, distortion, detectparas);

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

