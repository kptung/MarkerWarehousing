#ifndef __IRARINTERFACE_H__
#define __IRARINTERFACE_H__

////////////////////////////////////////////////////////////////////
#include <list>
#include <vector>
#include <string>
#include <opencv2/opencv.hpp>
#include <opencv2/core.hpp>
#include "IrArucoMarker.hpp"

/************************************************************************/
/* aruco                                                                */
/************************************************************************/
#include "aruco/aruco.hpp"

//// //// ************* //// ////
//// //// IrCalibration //// ////
//// //// ************* //// ////
const cv::Mat &getCameraIntrinsicMatrix( void );
const cv::Mat &getCameraDistortionMatrix( void );

/************************************************************************/
/* import parameters                                                                      */
/************************************************************************/
bool importYMLCameraParameters(const std::string &filename);
bool importYMLCameraParameters(const std::string &filename, cv::Mat &camMatrix, cv::Mat &distCoeffs);

/************************************************************************/
/* return injection position                                                                     */
/************************************************************************/
std::vector<cv::Point2f> findInjection(const std::vector<cv::Point3f>& objpoints, const cv::Mat& rvec, const cv::Mat& tvec, const int& ori, const cv::Point2f &center);

/************************************************************************/
/* read aruco detection params                                          */
/************************************************************************/
bool importYMLDetectParameters(const std::string &filename, cv::Ptr<cv::aruco::DetectorParameters> &params);
bool importYMLDetectParameters(const std::string &filename);

/************************************************************************/
/*  find aruco marker                                                   */
/************************************************************************/
bool findArucoMarkers(const cv::Mat &image, const float& markerLen, std::vector<IrArucoMarker> &markers);

//// //// ************ //// ////
//// ////  JNI_Common  //// ////
//// //// ************ //// ////
#ifdef ANDROID
void GetJStringContent(JNIEnv *env, jstring AStr, std::string &ARes);
#endif

//bool getMarkerId( const cv::Mat& img, std::vector<Marker>& markers );
//bool getMarkerLmk( const cv::Mat& image, std::vector<MarkerExt>& markers);
//bool getMarkerPos( const cv::Mat& img, std::vector<MarkerExt>& markers );

#endif
