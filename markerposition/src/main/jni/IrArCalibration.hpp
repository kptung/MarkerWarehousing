#ifndef __IrArCalibration_hpp__
#define __IrArCalibration_hpp__

#include <vector>
#include <string>
#include <iostream>
#include <opencv2/opencv.hpp>
#include <opencv2/core.hpp>

/**
 * A camera calibraiton class that stores intrinsic matrix
 * and distortion vector, as well as the 3d data of a calibration checkerboard.
 */
class IrArCalibration3d
{  
public:
	IrArCalibration3d() : m_camIntrinsic(cv::Mat::eye(3, 3, CV_32F)*-1), m_camDistortion(cv::Mat(5, 1, CV_64F)*-1)
	{
	}

	// return camera intrinstic & distortion and the detection paras
	const cv::Mat &getIntrinsicMatrix( void )
	{
		return m_camIntrinsic;
	}
	const cv::Mat &getDistortionMatrix( void )
	{
		return m_camDistortion;
	}
	const cv::Ptr<cv::aruco::DetectorParameters> &getDetectparas(void)
	{
		return m_detectParams;
	}
	cv::Ptr<cv::aruco::Dictionary> getDictionary(void)
	{
		return m_dict;
	}
	
	// get RT from double eyes 
	void getRTfromDualEyes(cv::Mat& lrvec, cv::Mat& rrvec, cv::Mat& ltvec, cv::Mat& rtvec)
	{
		cv::Mat(m_Lrvec).copyTo(lrvec);
		cv::Mat(m_Rrvec).copyTo(rrvec);
		cv::Mat(m_Ltvec).copyTo(ltvec);
		cv::Mat(m_Rtvec).copyTo(rtvec);
	}

	// get camera parameters
	bool loadCameraParametersFromYML(const std::string &filename);
	// aruco detection parameters definition          
	bool readDetectParameters(const std::string& filename);
	// get RT parameters
	bool loadRTParametersFromYML(const std::string &filename);
	// get defined dictionary
	bool readDictionary(const std::string& filename);

private:
	
	//cv::Size2i m_imgResolution;
	// camera intrinsic & distortion 
	cv::Mat m_camIntrinsic;
	cv::Mat m_camDistortion;
	// Two Eyes's RT
	cv::Vec3d m_Lrvec, m_Rrvec, m_Ltvec, m_Rtvec;
	//  aruco detections params definition        	
	cv::Ptr<cv::aruco::DetectorParameters> m_detectParams;
	// aruco dictionary
	cv::Ptr<cv::aruco::Dictionary> m_dict;
};

/************************************************************************/
/*    aruco detection parameters                                        */
/************************************************************************/
inline bool IrArCalibration3d::readDictionary(const std::string& filename) {
	
	int mSize, mCBits;
	cv::Mat bits;
	if (filename.empty())
	{
		m_dict = cv::aruco::getPredefinedDictionary(16);
#ifdef ANDROID
		LOGD("No user-defined dictionary, Load the pre-defined aruco dictionary.");
#endif
		return true;
	}
	else
	{
		cv::FileStorage fsr(filename, cv::FileStorage::READ);
		if (!fsr.isOpened())
		{
			// if no dictionary file, load the pre-defined aruco dictionary
			m_dict = cv::aruco::getPredefinedDictionary(16);
#ifdef ANDROID
			LOGD("No user-defined dictionary, Load the pre-defined aruco dictionary.");
#endif
			return true;
		}
		else
		{
#ifdef ANDROID
			LOGD("Load the user-defined aruco dictionary.");
#endif
			fsr["MarkerSize"] >> mSize;
			fsr["MaxCorrectionBits"] >> mCBits;
			fsr["ByteList"] >> bits;
			fsr.release();
			m_dict = cv::makePtr<cv::aruco::Dictionary>(cv::aruco::Dictionary(bits, mSize, mCBits));
			return true;
		}
	}
	
	
}


/************************************************************************/
/*    aruco detection parameters                                        */
/************************************************************************/
inline bool IrArCalibration3d::readDetectParameters(const std::string& filename) {
	m_detectParams = cv::aruco::DetectorParameters::create();
	cv::FileStorage fs(filename, cv::FileStorage::READ);
	if (!fs.isOpened())
		return false;
	fs["adaptiveThreshWinSizeMin"] >> m_detectParams->adaptiveThreshWinSizeMin;
	fs["adaptiveThreshWinSizeMax"] >> m_detectParams->adaptiveThreshWinSizeMax;
	fs["adaptiveThreshWinSizeStep"] >> m_detectParams->adaptiveThreshWinSizeStep;
	fs["adaptiveThreshConstant"] >> m_detectParams->adaptiveThreshConstant;
	fs["minMarkerPerimeterRate"] >> m_detectParams->minMarkerPerimeterRate;
	fs["maxMarkerPerimeterRate"] >> m_detectParams->maxMarkerPerimeterRate;
	fs["polygonalApproxAccuracyRate"] >> m_detectParams->polygonalApproxAccuracyRate;
	fs["minCornerDistanceRate"] >> m_detectParams->minCornerDistanceRate;
	fs["minDistanceToBorder"] >> m_detectParams->minDistanceToBorder;
	fs["minMarkerDistanceRate"] >> m_detectParams->minMarkerDistanceRate;
	fs["cornerRefinementMethod"] >> m_detectParams->cornerRefinementMethod;
	fs["cornerRefinementWinSize"] >> m_detectParams->cornerRefinementWinSize;
	fs["cornerRefinementMaxIterations"] >> m_detectParams->cornerRefinementMaxIterations;
	fs["cornerRefinementMinAccuracy"] >> m_detectParams->cornerRefinementMinAccuracy;
	fs["markerBorderBits"] >> m_detectParams->markerBorderBits;
	fs["perspectiveRemovePixelPerCell"] >> m_detectParams->perspectiveRemovePixelPerCell;
	fs["perspectiveRemoveIgnoredMarginPerCell"] >> m_detectParams->perspectiveRemoveIgnoredMarginPerCell;
	fs["maxErroneousBitsInBorderRate"] >> m_detectParams->maxErroneousBitsInBorderRate;
	fs["minOtsuStdDev"] >> m_detectParams->minOtsuStdDev;
	fs["errorCorrectionRate"] >> m_detectParams->errorCorrectionRate;
	fs.release();
	m_detectParams->cornerRefinementMethod = 1; // do corner refinement in markers
	return true;
}

/************************************************************************/
/* camera parameters                                                    */
/************************************************************************/
inline bool IrArCalibration3d::loadCameraParametersFromYML(const std::string &filename)
{
	int width, height;
	cv::Mat intrinsic, distortion;

	cv::FileStorage fs(filename, cv::FileStorage::READ);
	if (!fs.isOpened())
		return false;
	fs["image_width"] >> width;
	fs["image_height"] >> height;
	fs["camera_matrix"] >> intrinsic;
	fs["distortion_coefficients"] >> distortion;
	fs.release();

	intrinsic.copyTo(m_camIntrinsic);
	distortion.copyTo(m_camDistortion);

	if (width > 1 && height > 1 && cv::checkRange(intrinsic) && cv::checkRange(distortion))
	{
		//m_imgResolution = cv::Size2i(width, height);
		intrinsic.copyTo(m_camIntrinsic);
		distortion.copyTo(m_camDistortion);
	}
	return true;
}

/************************************************************************/
/* RT matrixes                                                          */
/************************************************************************/
inline bool IrArCalibration3d::loadRTParametersFromYML(const std::string &filename)
{
	cv::FileStorage fs(filename, cv::FileStorage::READ);
	if (!fs.isOpened())
		return false;
	
	fs["left_eye_rotate_vec"] >> m_Lrvec;
	fs["right_eye_rotate_vec"] >> m_Rrvec;
	fs["left_eye_translate_vec"] >> m_Ltvec;
	fs["right_eye_translate_vec"] >> m_Rtvec;
	fs.release();

	return true;
}
#endif
