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
#include <string>

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
	IrArucoMarker() : m_id(-1), m_ori(-1), m_unit(-1), m_cameraPosition(cv::Point3f(-1.0, -1.0, -1.0)), m_rvec(cv::Mat::eye(3, 3, CV_32FC1)*-1), RX(cv::Mat::eye(3, 3, CV_32FC1)), RY(cv::Mat::eye(3, 3, CV_32FC1)), RZ(cv::Mat::eye(3, 3, CV_32FC1)), m_center(cv::Point2f(-1.0, -1.0)), m_markerLength(-1)
	{
	}

	// set/get marker corners
	void setCorners(const std::vector<cv::Point2f> &corners)
	{
		m_corners.clear();
		for (size_t i = 0; i < corners.size(); i++)
		{
			m_corners.push_back(corners.at(i));
		}
	}
	const std::vector<cv::Point2f> &getCorners(void) const
	{
		return m_corners;
	}

	// set/get marker rejected corners
	void setRejecteds(const std::vector<cv::Point2f> &rejects)
	{
		m_rejecteds.clear();
		for (size_t i = 0; i < rejects.size(); i++)
		{
			m_rejecteds.push_back(rejects.at(i));
		}
	}
	const std::vector<cv::Point2f> &getRejecteds(void) const
	{
		return m_rejecteds;
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
	void setMarkerCenter(const std::vector< cv::Point2f >& corners)
	{
		cv::Point2f cent(0, 0);
		for (int p = 0; p < corners.size(); p++)
			cent += corners.at(p);
		m_center = cent / (float)corners.size();
	}
	cv::Point2f getMarkerCenter() const
	{
		return m_center;
	}

	// set/get marker length unit
	void setMarkerUnit(const int& i)
	{
		m_unit = i;
	}
	float getMarkerUnit(void) const
	{
		return m_unit;
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
	float getCameraDistance(void) const
	{
		float x = (float)m_cameraPosition.x;
		float y = (float)m_cameraPosition.y;
		float z = (float)m_cameraPosition.z;
		float dist = sqrt(x*x + y*y + z*z);
		if(m_unit == 0)
			dist *= 100;
		return dist;
		//return z;
	}
	// get X-Z camera distance
	float getXZCameraDistance(void) const
	{
		float x = (float)m_cameraPosition.x;
		float y = (float)m_cameraPosition.y;
		float z = (float)m_cameraPosition.z;
		float dist = sqrt(x * x + z * z);
		if (m_unit == 0)
			dist *= 100;
		return dist;
	}
	
	// get camera angle
	bool getCameraAngle(int& xzangle, int& yzangle) const
	{
		float x = (float)m_cameraPosition.x;
		float y = (float)m_cameraPosition.y;
		float z = (float)m_cameraPosition.z;

		// define XZ Quadrant
		int xzflag = 1;
		if (x < 0 && z >= 0) xzflag = 2;
		else if (x < 0 && z < 0) xzflag = 3;
		else if (x >= 0 && z < 0) xzflag = 4;
		xzangle = atan2(z, x) / (PI / 180);
		if (xzflag == 1 || xzflag ==2) xzangle = 90 - xzangle;
		else xzangle = 90 + xzangle;

		// define YZ Quadrant
		int yzflag = 1;
		if (y < 0 && z >= 0) yzflag = 2;
		else if (y < 0 && z < 0) yzflag = 3;
		else if (y >= 0 && z < 0) yzflag = 4;
		yzangle = atan2(z, y) / (PI / 180);
		if (yzflag == 1 || yzflag == 2) yzangle = 90 - yzangle;
		else yzangle = 90 + yzangle;
	
		return true;
	}

	// rotate rotation_matrix by the marker orientation counterclockwise
	// when the marker is rotated in 90/180/270 clockwise, it needs to rotate in 90/180/270 counterclockwise
	/************************************************************************/
	/*  Rotation matrix when rotate by x-axis                               */
	/*  in clockwise                           in counterclockwise          */
	/*       | 1,   0   ,     0   |         | 1,   0   ,     0   |          */
	/*  Rx = | 0, cos(θ),  -sin(θ)|    Rx = | 0, cos(θ),   sin(θ)|          */
	/*       | 0, sin(θ),  cos(θ) |         | 0, -sin(θ),  cos(θ)|          */
	/*  Rotation matrix when rotate by y-axis                               */
	/*  in clockwise                           in counterclockwise          */
	/*       |  cos(θ), 0,  sin(θ)|         | cos(θ), 0,  -sin(θ)|          */
	/*  Ry = |    0   , 1,    0   |    Ry = |   0   , 1,     0   |          */
	/*       | -sin(θ), 0,  cos(θ)|         | sin(θ), 0,   cos(θ)|          */
	/*  Rotation matrix when rotate by z-axis                               */
	/*  in clockwise                           in counterclockwise          */
	/*       | cos(θ),  -sin(θ),  0 |       |  cos(θ),  sin(θ),   0 |       */
	/*  Rz = | sin(θ),  cos(θ) ,  0 |  Rz = | -sin(θ),  cos(θ) ,  0 |       */
	/*       |  0    ,    0    ,  1 |       |   0    ,    0    ,  1 |       */
	/************************************************************************/
	void rotateZAxis(cv::Mat &rotation)
	{
		cv::Mat R(3, 3, CV_32FC1);
		cv::Rodrigues(rotation, R);
		//create a rotation matrix for z axis
		float angleRad = m_ori*PI / 180;
		/***********************************/
		/*            R = RZ * R           */
		/***********************************/
		RZ.at<float>(0, 0) = cos(angleRad);
		RZ.at<float>(0, 1) = sin(angleRad);
		RZ.at<float>(1, 0) = -sin(angleRad);
		RZ.at<float>(1, 1) = cos(angleRad);
		//now multiply; !! Rz * R != R * Rz since the results will be inverse; Here is Rz * R is correct
		R = RZ * R;
		//finally, the the rodrigues back
		cv::Rodrigues(R, rotation);
	}
	void rotateXAxis(cv::Mat &rotation)
	{
		cv::Mat R(3, 3, CV_32FC1);
		cv::Rodrigues(rotation, R);
		//create a rotation matrix for z axis
		float angleRad = m_ori*PI / 180;
		/***********************************/
		/*            R = RX * R           */
		/***********************************/
		RX.at<float>(1, 1) = cos(angleRad);
		RX.at<float>(1, 2) = sin(angleRad);
		RX.at<float>(2, 1) = -sin(angleRad);
		RX.at<float>(2, 2) = cos(angleRad);
		//now multiply; !! Rx * R != R * Rx since the results will be inverse; Here is Rx * R is correct
		R = RX * R;
		//finally, the the rodrigues back
		cv::Rodrigues(R, rotation);
	}
	void rotateYAxis(cv::Mat &rotation)
	{
		cv::Mat R(3, 3, CV_32FC1);
		cv::Rodrigues(rotation, R);
		//create a rotation matrix for z axis
		float angleRad = m_ori*PI / 180;
		/***********************************/
		/*            R = RY * R           */
		/***********************************/
		RY.at<float>(0, 0) = cos(angleRad);
		RY.at<float>(0, 2) = -sin(angleRad);
		RY.at<float>(2, 0) = sin(angleRad);
		RY.at<float>(2, 2) = cos(angleRad);
		//now multiply; !! Ry * R != R * Ry since the results will be inverse; Here is Ry * R is correct
		R = RY * R;
		//finally, the the rodrigues back
		cv::Rodrigues(R, rotation);
	}

	// set/get rotation matrix
	void setRotationMatrix(const cv::Vec3d& rvec)
	{
		m_rvec.release();
		if (m_ori > 0)
		{
			cv::Mat raux, Rvec;
			raux = cv::Mat(rvec);
			raux.convertTo(Rvec, CV_32FC1);
			rotateZAxis(Rvec);
			m_rvec = Rvec;
		}
		else
			m_rvec = cv::Mat(rvec);
	}
	cv::Mat getRotationMatrix(void) const
	{
		return m_rvec;
	}

	// set/get translation matrix
	void setTransnslationMatrix(const cv::Vec3d& tvec)
	{
		m_tvec.release();
		cv::Mat ttvec = cv::Mat(tvec);
		ttvec.convertTo(ttvec, CV_32FC1);
		m_tvec = ttvec;
	}
	cv::Mat getTransnslationMatrix(void) const
	{
		return m_tvec;
	}

	// get the Z-axis rotation matrix based on the marker's orientation
	cv::Mat getRZ(void) const
	{
		return RZ;
	}
	// get the X-axis rotation matrix based on the marker's orientation
	cv::Mat getRX(void) const
	{
		return RX;
	}
	// get the Y-axis rotation matrix based on the marker's orientation
	cv::Mat getRY(void) const
	{
		return RY;
	}

	template <typename T>
	std::string ToString(const T value)
	{
		std::ostringstream stream;
		stream << value;
		return stream.str();
	}

	// save the marker infomation
	void saveMarkInfo(const std::string filename)
	{
		cv::FileStorage fs(filename, cv::FileStorage::WRITE);
		fs << "marker_id" << m_id;
		fs << "marker_orientation" << m_ori;
		fs << "marker_center" << m_center;
		fs << "marker_length" << m_markerLength;
		fs << "camera_position" << m_cameraPosition;
		fs << "camera_rotation_matrix" << m_rvec;
		fs << "camera_translation_matrix" << m_tvec;
		fs << "marker_corners" << "{";
		for (int i = 0; i < m_corners.size(); i++)
		{
			fs << "Corners_" + ToString(i);
			fs << m_corners.at(i);
		}
		fs << "}";
		fs << "marker_rejecteds" << "{";
		for (int i = 0; i < m_rejecteds.size(); i++)
		{
			fs << "Rejecteds_" + ToString(i);
			fs << m_rejecteds.at(i);
		}
		fs << "}";
		fs.release();
	}
private:

	// Id of the marker
	int m_id;

	// Orientation of the marker
	int m_ori;

	// marker corner
	std::vector<cv::Point2f> m_corners;

	// marker rejected points
	std::vector<cv::Point2f> m_rejecteds;

	// camera position
	cv::Point3f m_cameraPosition;

	// camera rotation, translation matrix, the rotation RX, RY and RZ based on marker's orientation
	// when do the matrix multiplication, the matrix type (e.g. FLOAT32, FlLOAT64) must be the same
	// The default type of cv::Mat is FLOAT64 
	cv::Mat m_rvec, m_tvec, RX, RY, RZ;

	// marker center
	cv::Point2f m_center;

	//  marker length    
	float m_markerLength;

	// marker length unit
	int m_unit;
};


#endif
