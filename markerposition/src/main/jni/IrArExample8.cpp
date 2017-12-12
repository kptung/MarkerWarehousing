#include "IrArInterface.h"
#include <chrono>
#include <ctime>
using namespace std;
using namespace cv;

/****************************************************************************/
/*  Aruco sample works well                                                 */
/*  and draw the injection point without considering the marker orientation */
/*  Test marker @ 1x1, 2x2, 3x3, 4x4, 5x5 are ok                            */
/*  Author: kptung                                                          */
/*  Modified: kptung, 2017/11/07                                            */
/****************************************************************************/
int main(int argc, char **argv)
{
	std::string infolder("./data/artest");
	std::string outfolder("./data/arout");
	
	/************************************************************************/
	/* the given marker length in meters                                    */
	/************************************************************************/
	float markerLength = 0.03f; // the unit is meter

	std::string cameraFilename("camera-z2.yml");
	//std::string cameraFilename("bt300jj130-camera.yml");
	bool camflag = importYMLCameraParameters(cameraFilename);
	if (!camflag)
		return 0;

	std::string detFilename("detector_params.yml");
	bool detflag = importYMLDetectParameters(detFilename);
	if (!detflag) {
		cerr << "Invalid detector parameters file" << endl;
		return 0;
	}

	std::vector<std::string> files;
	get_files_in_directory(infolder, files);

	// Set real inject-position
	cv::Point3f Injection(0, -0.05f, 0);
	std::vector<cv::Point3f> Injectionpts = make_vector<cv::Point3f>() << Injection;

	for (unsigned int i = 0; i < files.size(); i++)
	{
		cout << "i: " << i + 1 << "/" << files.size() << endl;
		cout << "file: " << files[i] << endl;

		//CV_LOAD_IMAGE_UNCHANGED, CV_LOAD_IMAGE_GRAYSCALE, CV_LOAD_IMAGE_COLOR
		cv::Mat src = imread(infolder + SEP + files[i], CV_LOAD_IMAGE_COLOR);
		vector<IrArucoMarker> markers;
		auto tstart = std::chrono::high_resolution_clock::now();
		bool flag = findArucoMarkers(src, markerLength, markers);
		auto tend = std::chrono::high_resolution_clock::now();
		auto diff = std::chrono::duration_cast<std::chrono::duration<double>>(tend - tstart);
		// time estimation
		bool tflag = false;
		if (tflag)
		{
			ofstream out1;
			char *ptimefile_name;
			ptimefile_name = "D:/workprojs/III.Projs/out/mtime.txt";
			out1.open(ptimefile_name, ios::app);
			out1 << "Call: " << (long)(1000 * diff.count()) << std::endl;
			out1.close();
		}
		if (flag)
		{
			for (unsigned int j = 0; j < markers.size(); j++)
			{
				int mid = markers.at(j).getMarkerId();
				float mxzdist = markers.at(j).getXZCameraDistance() * 100;
				int mori = markers.at(j).getMarkerOri();
				cv::Point2f mcenter = markers.at(j).getMarkerCenter();
				int mxzangle = 0, myzangle = 0;
				markers.at(j).getCameraAngle(mxzangle, myzangle);

				cout << "marker id: " << mid << endl;
				cout << "X-Z viewing angle: " << mxzangle << endl;
				cout << "Y-Z viewing angle: " << myzangle << endl;
				cout << "X-Z viewing distance: " << mxzdist << endl;

				cv::Mat rvec = markers.at(j).getRotationMatrix();
				cv::Mat tvec = markers.at(j).getTransnslationMatrix();

				// find injection pts
				std::vector<cv::Point2f> injpts = findInjection(Injectionpts, rvec, tvec, mori, mcenter);
				for (int k = 0; k < injpts.size(); k++)
					cv::circle(src, injpts.at(k), 15, cv::Scalar(0, 255, 255), -1, 10);

				// add marker center
				cv::circle(src, mcenter, 10, cv::Scalar(0, 255, 0), -1, 10);

				// the angle left/right depends on the camera's position (Z-axis 0 degree)
				std::ostringstream str1, str2, str3;
				if (mxzangle > 0 & myzangle > 0) str1 << " Camera Angle: Right: " << abs(mxzangle) << ", Top: " << abs(myzangle);
				else if (mxzangle < 0 && myzangle > 0) str1 << " Camera Angle: Left: " << abs(mxzangle) << ", Top: " << abs(myzangle);
				else if (mxzangle > 0 && myzangle < 0) str1 << " Camera Angle: Right: " << abs(mxzangle) << ", Bottom: " << abs(myzangle);
				else if (mxzangle < 0 && myzangle < 0) str1 << " Camera Angle: Left: " << abs(mxzangle) << ", Bottom: " << abs(myzangle);
				else if (mxzangle < 0 && myzangle == 0) str1 << " Camera Angle: Left: " << abs(mxzangle) << ", Top: " << abs(myzangle);
				else if (mxzangle > 0 && myzangle == 0) str1 << " Camera Angle: Right: " << abs(mxzangle) << ", Top: " << abs(myzangle);
				else if (mxzangle == 0 && myzangle == 0) str1 << " Camera Angle: Front: " << abs(mxzangle) << ", Front: " << abs(myzangle);
				else if (mxzangle == 0 && myzangle > 0) str1 << " Camera Angle: Front: " << abs(mxzangle) << ", Top: " << abs(myzangle);
				else if (mxzangle == 0 && myzangle < 0) str1 << " Camera Angle: Front: " << abs(mxzangle) << ", Bottom: " << abs(myzangle);
				
				int r1 = 4, r2 = 7;
				int r = (mid == 777) ? r1 : r2;
				int x = (mid == 777) ? (mcenter.x - 200) : (mcenter.x);

				putText(src, str1.str(), Point(x, 500), FONT_HERSHEY_SIMPLEX, 2, Scalar(255, 0, 0), 5, 8, false);
				str2 << "Marker ID: " << mid << ", Distance(cm): " << mxzdist;
				putText(src, str2.str(), Point(x, 600), FONT_HERSHEY_SIMPLEX, 2, Scalar(255, 0, 0), 5, 8, false);
				str3 << "Marker Orientation: " << mori << " degrees ";
				putText(src, str3.str(), Point(x, 700), FONT_HERSHEY_SIMPLEX, 2, Scalar(255, 0, 0), 5, 8, false);

				imwrite(outfolder + SEP + files[i], src);
			}
		}
		else
		{
			std::ostringstream str;
			cout << "No Detected Marker!!!" << endl;
			str << "No Detected Marker!!!";
			putText(src, str.str(), Point(src.cols / 2, 700), FONT_HERSHEY_SIMPLEX, 2, Scalar(255, 255, 0), 5, 8, false);
			imwrite(outfolder + SEP + files[i], src);
		}
	}
	return 1;
}
