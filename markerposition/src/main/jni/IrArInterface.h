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
/* import parameters                                                    */
/************************************************************************/
bool importYMLCameraParameters(const std::string &filename);

/************************************************************************/
/*  import Rotation and Translation parameters                          */
/************************************************************************/
bool importYMLRTParameters(const std::string &filename);

/************************************************************************/
/*  import Customed Dictionary                                          */
/************************************************************************/
bool importYMLDict(const std::string &filename);

/************************************************************************/
/* return injection position                                            */
/************************************************************************/
std::vector<cv::Point2f> findInjection(const std::vector<cv::Point3f>& objpoints, const cv::Mat& rvec, const cv::Mat& tvec, const int& ori, const cv::Point2f &center);

/************************************************************************/
/* read aruco detection params                                          */
/************************************************************************/
bool importYMLDetectParameters(const std::string &filename);

/************************************************************************/
/*  find aruco marker                                                   */
/************************************************************************/
bool findArucoMarkers(const cv::Mat &image, const float& markerLen, std::vector<IrArucoMarker> &markers);
bool findArucoMarkers(const cv::Mat &image, std::vector<IrArucoMarker> &markers);
//// //// ************ //// ////
//// ////  JNI_Common  //// ////
//// //// ************ //// ////
#ifdef ANDROID
void GetJStringContent(JNIEnv *env, jstring AStr, std::string &ARes);
#endif


#endif
