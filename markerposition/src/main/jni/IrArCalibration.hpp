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
	IrArCalibration3d() : m_cbHasborder(1), m_cbPattern(7,7), m_cbRealsize(1.0f,1.0f), m_cbLatest(false), 
		m_imgResolution(800,600), m_camIntrinsic(3,3,CV_64F), m_camDistortion(5,1,CV_64F)
	{
	}

	
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

	bool saveCameraParametersToFile( const std::string &filename );
	bool loadCameraParametersFromFile( const std::string &filename );
	bool loadCameraParametersFromYML(const std::string &filename);
	bool loadCameraParametersFromYML(const std::string &filename, cv::Mat &camMatrix, cv::Mat &distCoeffs);
	
	bool runCameraCalibration( const std::string &folder, const std::string &filename );
	bool verifyCameraParamDA( const cv::Mat &src, float &distance, float &angle );
	bool verifyCameraParamXYZ( const cv::Mat &src, cv::Point3f &position );	
	/************************************************************************/
	/* aruco detection parameters definition                                                                     */
	/************************************************************************/
	bool readDetectParameters(const std::string& filename, cv::Ptr<cv::aruco::DetectorParameters> &params);
	bool readDetectParameters(const std::string& filename);

protected:
	const std::vector<cv::Point3f> &getKeypontSequence( void )
	{
		if (!m_cbLatest)
		{
			m_cbLatest = updateKeypointSequence();
		}

		return m_cbKeypoints;
	}
	bool updateKeypointSequence( void );

private:
	int m_cbHasborder;  
	cv::Size2i m_cbPattern;
	cv::Size2f m_cbRealsize;
	// from left to right, from top to bottom
	std::vector<cv::Point3f> m_cbKeypoints;
	bool m_cbLatest;

	cv::Size2i m_imgResolution;
	cv::Mat m_camIntrinsic;
	cv::Mat m_camDistortion;

	/************************************************************************/
	/*  aruco detections params definition                                                  */
	/************************************************************************/
	cv::Ptr<cv::aruco::DetectorParameters> m_detectParams;
};

/************************************************************************/
/*    aruco detection parameters                                        */
/************************************************************************/
inline bool IrArCalibration3d::readDetectParameters(const std::string& filename, cv::Ptr<cv::aruco::DetectorParameters> &params) {
	cv::FileStorage fs(filename, cv::FileStorage::READ);
	if (!fs.isOpened())
		return false;
	fs["adaptiveThreshWinSizeMin"] >> params->adaptiveThreshWinSizeMin;
	fs["adaptiveThreshWinSizeMax"] >> params->adaptiveThreshWinSizeMax;
	fs["adaptiveThreshWinSizeStep"] >> params->adaptiveThreshWinSizeStep;
	fs["adaptiveThreshConstant"] >> params->adaptiveThreshConstant;
	fs["minMarkerPerimeterRate"] >> params->minMarkerPerimeterRate;
	fs["maxMarkerPerimeterRate"] >> params->maxMarkerPerimeterRate;
	fs["polygonalApproxAccuracyRate"] >> params->polygonalApproxAccuracyRate;
	fs["minCornerDistanceRate"] >> params->minCornerDistanceRate;
	fs["minDistanceToBorder"] >> params->minDistanceToBorder;
	fs["minMarkerDistanceRate"] >> params->minMarkerDistanceRate;
	fs["cornerRefinementMethod"] >> params->cornerRefinementMethod;
	fs["cornerRefinementWinSize"] >> params->cornerRefinementWinSize;
	fs["cornerRefinementMaxIterations"] >> params->cornerRefinementMaxIterations;
	fs["cornerRefinementMinAccuracy"] >> params->cornerRefinementMinAccuracy;
	fs["markerBorderBits"] >> params->markerBorderBits;
	fs["perspectiveRemovePixelPerCell"] >> params->perspectiveRemovePixelPerCell;
	fs["perspectiveRemoveIgnoredMarginPerCell"] >> params->perspectiveRemoveIgnoredMarginPerCell;
	fs["maxErroneousBitsInBorderRate"] >> params->maxErroneousBitsInBorderRate;
	fs["minOtsuStdDev"] >> params->minOtsuStdDev;
	fs["errorCorrectionRate"] >> params->errorCorrectionRate;
	return true;
}

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
	m_detectParams->cornerRefinementMethod = cv::aruco::CORNER_REFINE_SUBPIX; // do corner refinement in markers
	return true;
}

inline bool IrArCalibration3d::loadCameraParametersFromYML(const std::string &filename, cv::Mat &camMatrix, cv::Mat &distCoeffs)
{
	cv::FileStorage fs(filename, cv::FileStorage::READ);
	if (!fs.isOpened())
		return false;
	fs["camera_matrix"] >> camMatrix;
	fs["distortion_coefficients"] >> distCoeffs;
	return true;
}

inline bool IrArCalibration3d::loadCameraParametersFromYML(const std::string &filename)
{
	int width, height;
	cv::Mat intrinsic, distortion;

	cv::FileStorage fs(filename, cv::FileStorage::READ);
	if (!fs.isOpened())
		return -1;
	fs["image_width"] >> width;
	fs["image_height"] >> height;
	fs["camera_matrix"] >> intrinsic;
	fs["distortion_coefficients"] >> distortion;
	fs.release();

	intrinsic.copyTo(m_camIntrinsic);
	distortion.copyTo(m_camDistortion);

	if (width > 1 && height > 1 && cv::checkRange(intrinsic) && cv::checkRange(distortion))
	{
		m_imgResolution = cv::Size2i(width, height);
		intrinsic.copyTo(m_camIntrinsic);
		distortion.copyTo(m_camDistortion);
	}
	return true;
}

#endif
