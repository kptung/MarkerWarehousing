﻿#pragma once
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
#define JNI_DBG 1
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
		cv::Mat origin;
		src.copyTo(origin);
		// (a) define aruco dictionary
		cv::Ptr<cv::aruco::Dictionary> dictionary = cv::aruco::getPredefinedDictionary(cv::aruco::PREDEFINED_DICTIONARY_NAME(cv::aruco::DICT_ARUCO_ORIGINAL));
		
		// (b) detect the markers and estimate pose
		std::vector< int > ids;
		std::vector< std::vector< cv::Point2f > > corners, rejecteds;
		std::vector< cv::Vec3d > rvecs, tvecs;
		
		auto tstart = std::chrono::high_resolution_clock::now();
		cv::aruco::detectMarkers(gray, dictionary, corners, ids, detectparas, rejecteds, intrinsic, distortion);
		// cv::aruco::drawDetectedMarkers(origin, corners, ids);
		
		// (c) estimate the camera pose; the unit is meter
		cv::aruco::estimatePoseSingleMarkers(corners, markerLen, intrinsic, distortion, rvecs, tvecs);
		auto tend = std::chrono::high_resolution_clock::now();
		auto diff = std::chrono::duration_cast<std::chrono::duration<double>>(tend - tstart);
		
		// time estimation
		bool tflag = false;
		if (tflag)
		{
			std::ofstream out1;
			char *ptimefile_name;
			ptimefile_name = "D:/workprojs/III.Projs/out/mtime.txt";
#ifdef ANDROID
			ptimefile_name = "/sdcard/marker/mtime.txt";
#endif
			out1.open(ptimefile_name, std::ios_base::app);
			out1 << "Func: " << (long)(1000 * diff.count()) << ", ";
			out1.close();
		}

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
				_markers[i].setRotationMatrix(rvecs[i]);
				_markers[i].setTransnslationMatrix(tvecs[i]);

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

				// (d3) Calculate the camera pose
				cv::Mat R;
				cv::Rodrigues(rvecs[i], R);
				cv::Mat cameraPose = -R.t() * (cv::Mat)tvecs[i];
				_markers[i].setCameraPos(cameraPose);

				// (d4) draw marker orientation
				//cv::aruco::drawAxis(origin, intrinsic, distortion, rvecs.at(i), tvecs.at(i), markerLen * 0.5f);

				markers.push_back(_markers[i]);
			}
			
			return true;
		}
		else
			return false;
		
	}

	std::vector<cv::Point2f> findInjectPoints(const std::vector<cv::Point3f>& objpts, const cv::Mat &intrinsic, const cv::Mat &distortion, const cv::Mat& rvec, const cv::Mat& tvec, const int &ori, const cv::Point2f &center)
	{
		std::vector<cv::Point2f> pts, outpts;
		cv::projectPoints(objpts, rvec, tvec, intrinsic, distortion, pts);
		cv::Point2f outPoint;
		if (ori > 0)
		{
			for (int i = 0; i < pts.size(); i++)
			{
				cv::Point2f inpoint = pts.at(i) - center;
				// rotate camera in counterclockwise by marker's orientation
				// when the position is rotated in 90/180/270 clockwise, it needs to be invert-rotated in 90/180/270 counterclockwise
				// note that The direction of vector rotation is counterclockwise if θ is positive (e.g. 90°), 
				// and clockwise if θ is negative (e.g. −90°).
				/**************************************************************************************/
				/*           1) clockwise                               2) counterclockwise           */
				/*  | x'|   | cos(θ),  -sin(θ)   | | x |       | x'|   |  cos(θ),  sin(θ)  | | x |    */
				/*  |   | = |                    | |   |       |   | = |                   | |   |    */
				/*  | y'|   | sin(θ),   cos(θ)   | | y |       | y'|   | -sin(θ),  cos(θ)  | | y |    */
				/**************************************************************************************/
				outPoint = cv::Point2f(inpoint.x * cos(ori * PI / 180) + inpoint.y * sin(ori*PI / 180), inpoint.x * -sin(ori*PI / 180) + inpoint.y * cos(ori * PI / 180));

				outPoint += center;
				outpts.push_back(outPoint);
			}
			return outpts;
		}
		else
			return pts;
	}
	
private:


};


#endif
