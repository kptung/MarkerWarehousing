#pragma once
#ifndef __IrArucoMarker_hpp__
#define __IrArucoMarker_hpp__

////////////////////////////////////////////////////////////////////
// Standard includes:
#include <vector>
#include <limits>
#include <algorithm>    // std::min
#include <opencv2/opencv.hpp>
#include "IrArGlobalMethods.hpp"
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
class IrArucoMarker
{
public:

	/// constructor
	IrArucoMarker() : m_id(-1), m_ori(-1), m_cameraPosition(cv::Point3f(-1.0,-1.0,-1.0)), m_rvec(cv::Mat::eye(3, 3, CV_32F)*-1), y_axis_angle(-1), x_axis_angle(-1), m_center(cv::Point2f(-1.0, -1.0)), m_markerLength(-1)
	{
	}

	// set/get marker corners
	void setCorners(const std::vector<cv::Point2f> &corners)
	{
		m_corners.clear();
		for (size_t i = 0; i < corners.size(); i++)
		{
			m_corners.push_back(corners[i]);
		}
	}
	const std::vector<cv::Point2f> &getCorners(void) const
	{
		return m_corners;
	}
	
	// set/get marker ID
	void setMarkerId(int id)
	{
		m_id = id;
	}
	int getMarkerId(void) const
	{
		return m_id;
	}

	// set/get marker orientation
	void setMarkerOri(int ori)
	{
		m_ori = ori;
	}
	int getMarkerOri(void) const
	{
		return m_ori;
	}

	// set/get camera pose
	void setCameraPos(const cv::Mat& cameraPose)
	{
		m_cameraPosition = cv::Point3f(cameraPose);
	}
	cv::Point3f getCameraPosition(void) const
	{
		return m_cameraPosition;
	}

	// set/get marker center
	void setMarkerCenter(const cv::Point2f& cen)
	{
		m_center = cen;
	}
	cv::Point2f getMarkerCenter() const
	{
		return m_center;
	}

	// set/get marker length in meters
	void setMarkerLen(const float& i)
	{
		m_markerLength = i;
	}
	float getMarkerLen(void) const
	{
		return m_markerLength;
	}

	// get camera distance
	float getXZCameraDistance(void) const
	{
		float x = (float)m_cameraPosition.x;
		float y = (float)m_cameraPosition.y;
		float z = (float)m_cameraPosition.z;
		cv::Point3f XYZ(x, y, z);
		if (m_ori == 90)
			XYZ = cv::Point3f(y, -x, z);
		else if (m_ori == 180)
			XYZ = cv::Point3f(-x, -y, z);
		else if (m_ori == 270)
			XYZ = cv::Point3f(-y, x, z);
		return sqrt(XYZ.x*XYZ.x+XYZ.z*XYZ.z);
		//return z;
	}
	float getCameraDistance(void) const
	{
		float x = (float)m_cameraPosition.x;
		float y = (float)m_cameraPosition.y;
		float z = (float)m_cameraPosition.z;
		return sqrt(x*x + y*y + z*z);
		//return z;
	}

	// get camera angle
	bool getCameraAngle(int& xzangle, int& yzangle) const
	{
		float x = (float)m_cameraPosition.x;
		float y = (float)m_cameraPosition.y;
		float z = (float)m_cameraPosition.z;

		// rotate camera in counterclockwise by marker's orientation
		/**********************************************************************************************/
		/*           1) clockwise                                    2) counterclockwise              */
		/*  | x'|   | cos(theta), -sin(theta) | | x |       | x'|   | cos(theta),  sin(theta)  | | x | */
		/*  |   | = |                         | |   |       |   | = |                          | |   | */
		/*  | y'|   | sin(theta), cos(theta)  | | y |       | y'|   | -sin(theta), cos(theta)  | | y | */
		/**********************************************************************************************/
		cv::Point3f XYZ(x, y, z);
		XYZ = cv::Point3f(x*cos(m_ori)+y*sin(m_ori), x*-sin(m_ori) + y*cos(m_ori), z);

		// define XZ Quadrant
		int xzflag = 1;
		if (XYZ.x < 0 && XYZ.z >= 0) xzflag = 2;
		else if (XYZ.x < 0 && XYZ.z < 0) xzflag = 3;
		else if (XYZ.x >= 0 && XYZ.z < 0) xzflag = 4;
		xzangle = atan2(XYZ.z, XYZ.x) / (PI / 180);
		if (xzflag == 1 || xzflag ==2) xzangle = 90 - xzangle;
		else xzangle = 90 + xzangle;

		// define YZ Quadrant
		int yzflag = 1;
		if (XYZ.y < 0 && XYZ.z >= 0) yzflag = 2;
		else if (XYZ.y < 0 && XYZ.z < 0) yzflag = 3;
		else if (XYZ.y >= 0 && XYZ.z < 0) yzflag = 4;
		yzangle = atan2(XYZ.z, XYZ.y) / (PI / 180);
		if (yzflag == 1 || yzflag == 2) yzangle = 90 - yzangle;
		else yzangle = 90 + yzangle;
		
		
		return true;
	}

	// set/get rotation matrix
	void setRotationMatrix(const cv::Vec3d& rvec)
	{
		m_rvec = cv::Mat(rvec);
	}
	cv::Mat getRotationMatrix(void) const
	{
		return m_rvec;
	}

	// set/get translation matrix
	void setTransnslationMatrix(const cv::Vec3d& tvec)
	{
		m_tvec = cv::Mat(tvec);
	}
	cv::Mat getTransnslationMatrix(void) const
	{
		return m_tvec;
	}

private:

	// Id of the marker
	int m_id;

	// Orientation of the marker
	int m_ori;

	// marker corner
	std::vector<cv::Point2f> m_corners;

	// camera position
	cv::Point3f m_cameraPosition;

	// camera rotation and translation matrix
	cv::Mat m_rvec, m_tvec;

	/******************************************************************************************************/
	/* angle definition: the coordinate system is shown as X(Right), Y(Up) and Z(Front)                   */
	/******************************************************************************************************/
	float y_axis_angle;
	float x_axis_angle;

	// marker center
	cv::Point2f m_center;

	/************************************************************************/
	/*  marker length                                                       */
	/************************************************************************/
	float m_markerLength;

};


#endif
