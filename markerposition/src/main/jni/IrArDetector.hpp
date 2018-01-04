#pragma once
#ifndef __IrArDetector_hpp__
#define __IrArDetector_hpp__

////////////////////////////////////////////////////////////////////
// Standard includes:
#include <vector>
#include <limits>
#include <algorithm>    // std::min
#include <opencv2/opencv.hpp>
#include "IrArGlobalMethods.hpp"
#include "IrArucoMarker.hpp"
#include <chrono>
#include <string>
#include <list>
#include <iostream>
#include <ctime>
#include <fstream>


#ifdef ANDROID
#include <jni.h>
#include <android/log.h>
#define IR_LIB_VERSION 1.0.0
#define JNI_DBG 0
#define LOG_TAG "IrMarkerLib"
#define LOGD(...) ((void)__android_log_print(ANDROID_LOG_DEBUG, LOG_TAG, __VA_ARGS__))
#endif

#ifndef PI
#define PI (3.14159265f)
#endif


///**** **** **** *** **** **** **** ****/
//*** This class represents a marker ***/
///**** **** **** *** **** **** **** ****/
class IrArDetector
{
public:

	/************************************************************************/
	/*         Aruco marker detection                                 */
	/************************************************************************/
	bool findArMarkers(const cv::Mat &src, const cv::Mat &gray, const float& markerLen, std::vector<IrArucoMarker> &markers, const cv::Mat &intrinsic, const cv::Mat &distortion, const cv::Ptr<cv::aruco::DetectorParameters> &detectparas)
	{

#ifdef ANDROID
		LOGD("C lib start");
#endif

		cv::Mat origin;
		src.copyTo(origin);
		// (a) define aruco dictionary
		cv::Ptr<cv::aruco::Dictionary> dictionary = cv::aruco::getPredefinedDictionary(cv::aruco::PREDEFINED_DICTIONARY_NAME(cv::aruco::DICT_ARUCO_ORIGINAL));
#ifdef ANDROID
		LOGD("dict load pass");
#endif
		if (intrinsic.empty() || distortion.empty())
		{
			//std::cout << "Lib needs parameters. Please check intrinsic and distortion" << endl;
#ifdef ANDROID
			LOGD("Lib needs parameters. Please check intrinsic and distortion");
#endif
			return false;
		}
		// (b) detect the markers and estimate pose
		std::vector< int > ids;
		std::vector< std::vector< cv::Point2f > > corners, rejecteds;
		std::vector< cv::Vec3d > rvecs, tvecs;
		
		auto tstart = std::chrono::high_resolution_clock::now();
		cv::aruco::detectMarkers(gray, dictionary, corners, ids, detectparas, rejecteds, intrinsic, distortion);
		// cv::aruco::drawDetectedMarkers(origin, corners, ids);
#ifdef ANDROID
		LOGD("detect pass");
#endif		
		// (c) estimate the camera pose; the unit is meter
		cv::aruco::estimatePoseSingleMarkers(corners, markerLen, intrinsic, distortion, rvecs, tvecs);
		auto tend = std::chrono::high_resolution_clock::now();
		auto diff = std::chrono::duration_cast<std::chrono::duration<double>>(tend - tstart);
#ifdef ANDROID
		LOGD("estimate pass");
#endif		
		// time estimation
// 		bool tflag = false;
// 		if (tflag)
// 		{
// 			std::ofstream out1;
// 			char *ptimefile_name;
// 			ptimefile_name = "D:/workprojs/III.Projs/out/mtime.txt";
// #ifdef ANDROID
// 			ptimefile_name = "/sdcard/marker/mtime.txt";
// #endif
// 			out1.open(ptimefile_name, std::ios_base::app);
// 			out1 << "Func: " << (long)(1000 * diff.count()) << ", ";
// 			out1.close();
// 		}

		// (d) marker information
		if (ids.size() > 0)
		{
			std::vector<IrArucoMarker> _markers(ids.size());
			for (int i = 0; i < ids.size(); i++)
			{
				// (d1) marker id, corners, marker_center, rotation_matrix and translation_matrix
				_markers[i].setMarkerId(ids.at(i));
				_markers[i].setCorners(corners.at(i));
				_markers[i].setRejecteds(rejecteds.at(i));
				_markers[i].setMarkerCenter(corners.at(i));
				_markers[i].setTransnslationMatrix(tvecs[i]);
#ifdef ANDROID
				LOGD("D1 pass");
#endif
				// (d2) get marker orientation
				cv::Point2f cent= _markers[i].getMarkerCenter();
				if (corners.at(i).at(0).x > cent.x && corners.at(i).at(0).y < cent.y)
					_markers[i].setMarkerOri(90);
				else if (corners.at(i).at(0).x > cent.x && corners.at(i).at(0).y > cent.y)
					_markers[i].setMarkerOri(180);
				else if (corners.at(i).at(0).x < cent.x && corners.at(i).at(0).y > cent.y)
					_markers[i].setMarkerOri(270);
				else
					_markers[i].setMarkerOri(0);
#ifdef ANDROID
				LOGD("D2 pass");
#endif
				// (d3) set rotation matrix based on marker orientation
				_markers[i].setRotationMatrix(rvecs[i]);
#ifdef ANDROID
				LOGD("D3 pass");
#endif				
				// (d4) Calculate the camera pose based on marker orientation
				cv::Mat R(3, 3, CV_32FC1);
				cv::Mat tvec = _markers.at(i).getTransnslationMatrix();
				tvec.convertTo(tvec, CV_32FC1);
				cv::Mat rvec = _markers.at(i).getRotationMatrix();
				rvec.convertTo(rvec, CV_32FC1);
				cv::Rodrigues(rvec, R);
				cv::Mat cameraPose = -R.t()*tvec;
				_markers[i].setCameraPos(cameraPose);
#ifdef ANDROID
				LOGD("D4 pass");
#endif
				// (d5) draw marker orientation
				//cv::aruco::drawAxis(origin, intrinsic, distortion, rvecs.at(i), tvecs.at(i), markerLen * 0.5f);

				markers.push_back(_markers[i]);
			}
#ifdef ANDROID
			LOGD("C lib end");
#endif
			return true;
		}
		else
		{
#ifdef ANDROID
			LOGD("C lib end");
#endif
			return false;
		}
	}
	// basic marker
	bool findArMarkers(const cv::Mat &src, const cv::Mat &gray, std::vector<IrArucoMarker> &markers, const cv::Mat &intrinsic, const cv::Mat &distortion, const cv::Ptr<cv::aruco::DetectorParameters> &detectparas)
	{
#ifdef ANDROID
		LOGD("C Basic mode lib start");
#endif
		cv::Mat origin;
		src.copyTo(origin);
		// (a) define aruco dictionary
		cv::Ptr<cv::aruco::Dictionary> dictionary = cv::aruco::getPredefinedDictionary(cv::aruco::PREDEFINED_DICTIONARY_NAME(cv::aruco::DICT_ARUCO_ORIGINAL));
#ifdef ANDROID
		LOGD("dict load pass");
#endif		
		// (b) detect the markers and estimate pose
		std::vector< int > ids;
		std::vector< std::vector< cv::Point2f > > corners, rejecteds;
		cv::aruco::detectMarkers(gray, dictionary, corners, ids, detectparas, rejecteds, intrinsic, distortion);
#ifdef ANDROID
		LOGD("detect pass");
#endif		
		// (d) marker information
		if (ids.size() > 0)
		{
			std::vector<IrArucoMarker> _markers(ids.size());
			for (int i = 0; i < ids.size(); i++)
			{
				// (d1) marker id, corners, rejecteds and marker_center
#ifdef ANDROID
				LOGD("ids.at(i) = %d", ids.at(i));
#endif	
				_markers[i].setMarkerId(ids.at(i));
				_markers[i].setMarkerCenter(corners.at(i));
#ifdef ANDROID
				LOGD("D1 pass");
#endif
				// (d2) marker orientation
				cv::Point2f cent = _markers[i].getMarkerCenter();
				if (corners.at(i).at(0).x > cent.x && corners.at(i).at(0).y < cent.y)
					_markers[i].setMarkerOri(90);
				else if (corners.at(i).at(0).x > cent.x && corners.at(i).at(0).y > cent.y)
					_markers[i].setMarkerOri(180);
				else if (corners.at(i).at(0).x < cent.x && corners.at(i).at(0).y > cent.y)
					_markers[i].setMarkerOri(270);
				else
					_markers[i].setMarkerOri(0);
#ifdef ANDROID
				LOGD("D2 pass");
#endif
				markers.push_back(_markers[i]);
			}
#ifdef ANDROID
			LOGD("C Basic mode lib end");
#endif
			return true;
		}
		else
		{
			return false;
		}
	}

	std::vector<cv::Point2f> findInjectPoints(const std::vector<cv::Point3f>& objpts, const cv::Mat &intrinsic, const cv::Mat &distortion, const cv::Mat& rvec, const cv::Mat& tvec, const int &ori, const cv::Point2f &center)
	{
		std::vector<cv::Point2f> pts, outpts;
		cv::projectPoints(objpts, rvec, tvec, intrinsic, distortion, pts);
		return pts;
	}
	
private:


};


#endif
