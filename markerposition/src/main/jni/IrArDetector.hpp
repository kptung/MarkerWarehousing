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

#ifndef PI
#define PI (3.14159265f)
#endif


///**** **** **** *** **** **** **** ****/
//*** This class represents marker detection ***/
//*** The unit of the detected marker size is in meter compulsory ***//
//*** Thus the unit of the camera position and the other 3D positions is in meter ***//
//*** Since the returned position needs to be represented as centimeter, the value shoul be multiplied 100  ***//
///**** **** **** *** **** **** **** ****/
class IrArDetector
{
public:

	/************************************************************************/
	/*         Aruco marker detection    Adv + App mode                     */
	/************************************************************************/
	// adv/app marker - for pc
	bool findArMarkers(const cv::Mat &src, const cv::Mat &gray, const float& markerLen, std::vector<IrArucoMarker> &markers, const cv::Mat &intrinsic, const cv::Mat &distortion, const cv::Ptr<cv::aruco::Dictionary> &dictionary)
	{
		if (intrinsic.empty() || distortion.empty() || dictionary->bytesList.empty())
			return false;

		// (a) detect the markers and estimate pose
		std::vector< int > ids;
		std::vector< std::vector< cv::Point2f > > corners, rejecteds;
		std::vector< cv::Vec3d > rvecs, tvecs;
		cv::Ptr<cv::aruco::DetectorParameters> paras = cv::aruco::DetectorParameters::create();
		//auto tstart = std::chrono::high_resolution_clock::now();
		cv::aruco::detectMarkers(gray, dictionary, corners, ids, paras, rejecteds);
		//cv::aruco::drawDetectedMarkers(origin, rejecteds, ids);
	
		// (b) marker information
		if (ids.size() > 0)
		{
			// (c) estimate the camera pose; the unit is meter; check the val is integer or float
			cv::aruco::estimatePoseSingleMarkers(corners, markerLen, intrinsic, distortion, rvecs, tvecs);
			//auto tend = std::chrono::high_resolution_clock::now();
			//auto diff = std::chrono::duration_cast<std::chrono::duration<double>>(tend - tstart);
	
			//if (rejecteds.size() > 0)
			//	cv::aruco::drawDetectedMarkers(origin, rejecteds, cv::noArray(), cv::Scalar(100, 0, 255));

			std::vector<IrArucoMarker> _markers(ids.size());
			for (int i = 0; i < ids.size(); i++)
			{
				// (d1) marker id, corners, marker_center, rotation_matrix, translation_matrix
				_markers[i].setMarkerId(ids.at(i));
				_markers[i].setCorners(corners.at(i));
				_markers[i].setRejecteds(rejecteds.at(i));
				_markers[i].setMarkerCenter(corners.at(i));
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
				// (d3) set rotation matrix based on marker orientation
				_markers[i].setRotationMatrix(rvecs[i]);
			
				// (d4) Calculate the camera pose based on marker orientation
				cv::Mat R(3, 3, CV_32FC1);
				cv::Mat tvec = _markers.at(i).getTransnslationMatrix();
				tvec.convertTo(tvec, CV_32FC1);
				cv::Mat rvec = _markers.at(i).getRotationMatrix();
				rvec.convertTo(rvec, CV_32FC1);
				cv::Rodrigues(rvec, R);
				cv::Mat cameraPose = -R.t()*tvec;
				_markers[i].setCameraPos(cameraPose);

				// (d5) draw marker orientation
				//cv::aruco::drawAxis(origin, intrinsic, distortion, rvecs.at(i), tvecs.at(i), markerLen * 0.5f);
				markers.push_back(_markers[i]);
			}
			return true;
		}
		else
		{
			return false;
		}
	}

	// adv/app marker - for android
	bool findArMarkers(const cv::Mat &gray, const float& markerLen, std::vector<IrArucoMarker> &markers, const cv::Mat &intrinsic, const cv::Mat &distortion, const cv::Ptr<cv::aruco::Dictionary> &dictionary)
	{
#ifdef ANDROID
		LOGD("C Adv/App lib start");
#endif
		if (intrinsic.empty() || distortion.empty() || dictionary->bytesList.empty())
		{
			//std::cout << "Lib needs parameters. Please check intrinsic, distortion and dictionary" << endl;
#ifdef ANDROID
			LOGD("Lib needs parameters. Please check intrinsic, distortion and dictionary");
#endif
			return false;
		}
		// (a) detect the markers and estimate pose
		std::vector< int > ids;
		std::vector< std::vector< cv::Point2f > > corners, rejecteds;
		std::vector< cv::Vec3d > rvecs, tvecs;
		cv::Ptr<cv::aruco::DetectorParameters> paras = cv::aruco::DetectorParameters::create();
		//auto tstart = std::chrono::high_resolution_clock::now();
		cv::aruco::detectMarkers(gray, dictionary, corners, ids, paras, rejecteds);
		//cv::aruco::drawDetectedMarkers(origin, rejecteds, ids);
#ifdef ANDROID
		LOGD("detect pass");
#endif		
		// (b) marker information
		if (ids.size() > 0)
		{
			// (c) estimate the camera pose; the unit is meter; check the val is integer or float
			cv::aruco::estimatePoseSingleMarkers(corners, markerLen, intrinsic, distortion, rvecs, tvecs);
			//auto tend = std::chrono::high_resolution_clock::now();
			//auto diff = std::chrono::duration_cast<std::chrono::duration<double>>(tend - tstart);
#ifdef ANDROID
			LOGD("estimate pass");
#endif		
			//if (rejecteds.size() > 0)
			//	cv::aruco::drawDetectedMarkers(origin, rejecteds, cv::noArray(), cv::Scalar(100, 0, 255));

			std::vector<IrArucoMarker> _markers(ids.size());
			for (int i = 0; i < ids.size(); i++)
			{
				// (d1) marker id, corners, marker_center, rotation_matrix, translation_matrix
				_markers[i].setMarkerId(ids.at(i));
				_markers[i].setCorners(corners.at(i));
				_markers[i].setRejecteds(rejecteds.at(i));
				_markers[i].setMarkerCenter(corners.at(i));
				_markers[i].setTransnslationMatrix(tvecs[i]);
#ifdef ANDROID
				LOGD("D1 - ID + Corners pass");
#endif
				// (d2) get marker orientation
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
				LOGD("D2 - orientation pass");
#endif
				// (d3) set rotation matrix based on marker orientation
				_markers[i].setRotationMatrix(rvecs[i]);
#ifdef ANDROID
				LOGD("D3 - rotation matrix pass");
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
				LOGD("D4 - camera position pass");
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

	// basic marker for pc-debug
	bool findArMarkers(const cv::Mat &src, const cv::Mat &gray, std::vector<IrArucoMarker> &markers, const cv::Ptr<cv::aruco::Dictionary> &dictionary)
	{
		cv::Mat origin;
		src.copyTo(origin);
		// (b) detect the markers
		std::vector< int > ids;
		std::vector< std::vector< cv::Point2f > > corners, rejecteds;
		cv::Ptr<cv::aruco::DetectorParameters> paras = cv::aruco::DetectorParameters::create();
		cv::aruco::detectMarkers(gray, dictionary, corners, ids, paras, rejecteds);
		// (d) marker information
		if (ids.size() > 0)
		{
			std::vector<IrArucoMarker> _markers(ids.size());
			for (int i = 0; i < ids.size(); i++)
			{
				// (d1) marker id and marker_center
				_markers[i].setMarkerId(ids.at(i));
				_markers[i].setMarkerCenter(corners.at(i));
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
				markers.push_back(_markers[i]);
			}
			return true;
		}
		else
		{
			return false;
		}
	}

	// basic marker - for android
	bool findArMarkers(const cv::Mat &gray, std::vector<IrArucoMarker> &markers, const cv::Ptr<cv::aruco::Dictionary> &dictionary)
	{
#ifdef ANDROID
		LOGD("C Basic mode lib start");
#endif
		// (b) detect the markers
		std::vector< int > ids;
		std::vector< std::vector< cv::Point2f > > corners, rejecteds;
		cv::Ptr<cv::aruco::DetectorParameters> paras = cv::aruco::DetectorParameters::create();
		cv::aruco::detectMarkers(gray, dictionary, corners, ids, paras, rejecteds);
#ifdef ANDROID
		LOGD("detect pass");
#endif		
		// (d) marker information
		if (ids.size() > 0)
		{
			std::vector<IrArucoMarker> _markers(ids.size());
			for (int i = 0; i < ids.size(); i++)
			{
				// (d1) marker id and marker_center
				_markers[i].setMarkerId(ids.at(i));
				_markers[i].setMarkerCenter(corners.at(i));
#ifdef ANDROID
				LOGD("D1 - ID + Corners pass");
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
				LOGD("D2 - orientation pass");
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
