#pragma once
#ifndef __IrArAssistant_hpp__
#define __IrArAssistant_hpp__

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
class IrArAssistant 
{
public:
	IrArAssistant() :m_markerLength(0)
	{
	}
	// set/get marker length in meters
	bool setMarkerLen(const float& mlen)
	{
		m_markerLength = mlen;
		return true;
	}
	float getMarkerLen(void)
	{
		return m_markerLength;
	}
	std::string getCameraParaName(void)
	{
		return cameraFilename;
	}
	std::string getDetectParaName(void)
	{
		return detFilename;
	}
	std::string getRTParaName(void)
	{
		return RTFilename;
	}

	// load camera parameters: intrinsic and distortion and its projection matrix parameters: left-eye &
	bool LoadParasFromID(const int& id)
	{
		detFilename = "detector_params.yml";
		switch (id)
		{
		case 1:
			cameraFilename = "bt300-camera-jim.yml";
			//cameraFilename = "camera-z2.yml";
			RTFilename = "bt300-RTmatrices-jim.yml";
			break;
		default:
			cameraFilename = "camera-z2.yml";
			//cameraFilename = "camera-g4.yml";
			RTFilename = "bt300-RTmatrices-jim.yml";
			break;
		}
		return true;
	}

	//  set/get injection points    
	void setInjectionPoint(const cv::Point3f &injectionpts)
	{
		m_Injectionpoints = make_vector<cv::Point3f>() << injectionpts;
	}
	void addInjectionPoint(const cv::Point3f &injectionpts)
	{
		m_Injectionpoints.push_back(injectionpts);
	}
	void removeInjectionPoint()
	{
		m_Injectionpoints.pop_back();
	}
	const std::vector<cv::Point3f> &getInjectionPoints(void) const
	{
		return m_Injectionpoints;
	}

private:
	// parameters file names
	std::string cameraFilename;
	std::string RTFilename;
	std::string detFilename;
	// marker length in meters
	float m_markerLength;
	//  3D injection points                                                                
	std::vector<cv::Point3f> m_Injectionpoints;
};


#endif
