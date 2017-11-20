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
	bool findArMarkers(const cv::Mat &gray, const float& markerLen, std::vector<IrArucoMarker> &markers, const cv::Mat &intrinsic, const cv::Mat &distortion, const cv::Ptr<cv::aruco::DetectorParameters> &detectparas)
	{
		cv::Mat origin;
		gray.copyTo(origin);
		// (a) define aruco dictionary
		cv::Ptr<cv::aruco::Dictionary> dictionary =
			cv::aruco::getPredefinedDictionary(cv::aruco::PREDEFINED_DICTIONARY_NAME(cv::aruco::DICT_ARUCO_ORIGINAL));
		// (b) detect the markers
		std::vector< int > ids;
		std::vector< std::vector< cv::Point2f > > corners, rejected;
		std::vector< cv::Vec3d > rvecs, tvecs;
		// detect markers and estimate pose
		cv::aruco::detectMarkers(gray, dictionary, corners, ids, detectparas, rejected, intrinsic, distortion);
		cv::aruco::drawDetectedMarkers(origin, corners, ids);
		// estimate the camera pose; the unit is meter
		if (ids.size() > 0)
		{
			std::vector<IrArucoMarker> _markers(ids.size());
			for (int i = 0; i < ids.size(); i++)
			{
				// (c) get marker orientation
				cv::Point2f cent(0, 0);
				for (int p = 0; p < 4; p++)
					cent += corners.at(i).at(p);
				cent = cent / 4.;
				if (corners.at(i).at(0).x > cent.x && corners.at(i).at(0).y < cent.y)
					_markers[i].setMarkerOri(90);
				else if (corners.at(i).at(0).x > cent.x && corners.at(i).at(0).y > cent.y)
					_markers[i].setMarkerOri(180);
				else if (corners.at(i).at(0).x < cent.x && corners.at(i).at(0).y > cent.y)
					_markers[i].setMarkerOri(270);
				else
					_markers[i].setMarkerOri(0);

				// (d) estimate the camera pose; the unit is meter
				_markers[i].setMarkerId(ids.at(i));
				_markers[i].setCorners(corners.at(i));
				_markers[i].setMarkerCenter(cent);
				cv::aruco::estimatePoseSingleMarkers(corners, markerLen, intrinsic, distortion, rvecs, tvecs);
				// (e) Calculate the camera pose
				cv::Mat R;
				cv::Rodrigues(rvecs[i], R);
				cv::Mat cameraPose = -R.t() * (cv::Mat)tvecs[i];
				_markers[i].setCameraPos(cameraPose);

				_markers[i].setRotationMatrix(rvecs[i]);
				_markers[i].setTransnslationMatrix(tvecs[i]);

				//cv::aruco::drawAxis(origin, intrinsic, distortion, rvecs[i], tvecs[i], markerLen * 0.5f);

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
				// rotate point by clockwise
				if (ori == 90)
					outPoint = cv::Point2f(inpoint.y, -inpoint.x);
				else if (ori == 180)
					outPoint = cv::Point2f(-inpoint.x, -inpoint.y);
				else if (ori == 270)
					outPoint = cv::Point2f(-inpoint.y, inpoint.x);
				outPoint += center;
			}
			outpts.push_back(outPoint);
			return outpts;
		}
		else
			return pts;
	}

	std::vector<cv::Point2f> findInjectPoints(const std::vector<cv::Point3f>& objpts, const cv::Mat &intrinsic, const cv::Mat &distortion, const cv::Mat& lrvec, const cv::Mat& ltvec, const cv::Mat& rrvec, const cv::Mat& rtvec, const int &ori, const cv::Point2f &center)
	{
		std::vector<cv::Point2f> lpts, rpts, outpts;
		cv::projectPoints(objpts, lrvec, ltvec, intrinsic, distortion, lpts);
		cv::projectPoints(objpts, rrvec, rtvec, intrinsic, distortion, rpts);

		int numPts = objpts.size();
		if (ori > 0)
		{
			cv::Point2f outPoint;
			for (int i = 0; i < numPts; i++)
			{
				cv::Point2f inpoint = lpts.at(i) - center;
				// rotate point by clockwise
				if (ori == 90)
					outPoint = cv::Point2f(inpoint.y, -inpoint.x);
				else if (ori == 180)
					outPoint = cv::Point2f(-inpoint.x, -inpoint.y);
				else if (ori == 270)
					outPoint = cv::Point2f(-inpoint.y, inpoint.x);
				outPoint += center;
				outpts.push_back(outPoint);
			}

			for (int i = 0; i < numPts; i++)
			{
				cv::Point2f inpoint = rpts.at(i) - center;
				// rotate point by clockwise
				if (ori == 90)
					outPoint = cv::Point2f(inpoint.y, -inpoint.x);
				else if (ori == 180)
					outPoint = cv::Point2f(-inpoint.x, -inpoint.y);
				else if (ori == 270)
					outPoint = cv::Point2f(-inpoint.y, inpoint.x);
				outPoint += center;
				outpts.push_back(outPoint);
			}
			return outpts;
		}
		else
		{
			for (int i = 0; i < numPts; i++)
				outpts.push_back(lpts.at(i));
			for (int i = 0; i < numPts; i++)
				outpts.push_back(rpts.at(i));
			return outpts;
		}
	}

private:


};


#endif
