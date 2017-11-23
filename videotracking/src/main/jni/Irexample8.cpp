#include "IrArInterface.h"

using namespace std;
using namespace cv;

/****************************************************************************/
/*  Aruco sample works well with the given marker length in meters         */
/*  and draw the injection point without considering the marker orientation */
/*  Test marker @ 1x1, 2x2, 3x3, 4x4, 5x5 are ok                            */
/*  Author: kptung                                                          */
/*  Modified: kptung, 2017/11/07                                            */
/****************************************************************************/
int main123(int argc, char **argv)
{
	std::string infolder("./data/artest7");
	std::string outfolder("./data/arucomarkerout");
	
	/************************************************************************/
	/* the given marker length in meters                                    */
	/************************************************************************/
	float markerLength = 0.03f; // the unit is meter

	//std::string cameraFilename("camera-z2.yml");
	std::string cameraFilename("bt300jj130-camera.yml");
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

	for (unsigned int i = 0; i < files.size(); i++)
	{
		cout << "i: " << i + 1 << "/" << files.size() << endl;
		cout << "file: " << files[i] << endl;

		//CV_LOAD_IMAGE_UNCHANGED, CV_LOAD_IMAGE_GRAYSCALE, CV_LOAD_IMAGE_COLOR
		cv::Mat src = imread(infolder + SEP + files[i], CV_LOAD_IMAGE_COLOR);
		vector<IrArucoMarker> markers;
		if (findArucoMarkers(src, markerLength, markers))
		{
			cv::Mat rvec, tvec;
			int mid = 0, mori = 0, mxzangle = 0, myzangle = 0;
			float mxzdist = 0;
			cv::Point2f mcenter;
			for (unsigned int j = 0; j < markers.size(); j++)
			{
				mid = markers[j].getMarkerId();
				mxzdist = markers[j].getXZCameraDistance() * 100;
				mori = markers[j].getMarkerOri();
				mcenter = markers[j].getMarkerCenter();
				markers[j].getCameraAngle(mxzangle, myzangle);

				cout << "marker id: " << mid << endl;
				cout << "X-Z viewing angle: " << mxzangle << endl;
				cout << "X-Z viewing distance: " << mxzdist << endl;
				cout << "Y-Z viewing angle: " << myzangle << endl;

				rvec = markers[j].getRotationMatrix();
				tvec = markers[j].getTransnslationMatrix();

				// find injection pts
				cv::Point3f Injection(0, -0.05f, 0);
				std::vector<cv::Point3f> Injectionpts = make_vector<cv::Point3f>() << Injection;
				std::vector<cv::Point2f> injpts = findInjection(Injectionpts, rvec, tvec, mori, mcenter);
				for (size_t j = 0; j < injpts.size(); j++)
					cv::circle(src, injpts[j], 15, cv::Scalar(255, 0, 0), -1, 10);

				// add marker center
				cv::circle(src, mcenter, 10, cv::Scalar(0, 255, 0), -1, 10);

				std::ostringstream str1, str2;
				if (mxzangle > 0 & myzangle > 0) str1 << " Camera Angle: Right: " << abs(mxzangle) << ", Top: " << abs(myzangle);
				else if (mxzangle < 0 && myzangle > 0) str1 << " Camera Angle: Left: " << abs(mxzangle) << ", Top: " << abs(myzangle);
				else if (mxzangle > 0 && myzangle < 0) str1 << " Camera Angle: Right: " << abs(mxzangle) << ", Bottom: " << abs(myzangle);
				else if (mxzangle < 0 && myzangle < 0) str1 << " Camera Angle: Left: " << abs(mxzangle) << ", Bottom: " << abs(myzangle);
				else if (mxzangle < 0 && myzangle == 0) str1 << " Camera Angle: Left: " << abs(mxzangle) << ", Top: " << abs(myzangle);
				else if (mxzangle > 0 && myzangle == 0) str1 << " Camera Angle: Right: " << abs(mxzangle) << ", Top: " << abs(myzangle);
				else if (mxzangle == 0 && myzangle == 0) str1 << " Camera Angle: Front: " << abs(mxzangle) << ", Front: " << abs(myzangle);
				else if (mxzangle == 0 && myzangle > 0) str1 << " Camera Angle: Front: " << abs(mxzangle) << ", Top: " << abs(myzangle);
				else if (mxzangle == 0 && myzangle < 0) str1 << " Camera Angle: Front: " << abs(mxzangle) << ", Bottom: " << abs(myzangle);
				putText(src, str1.str(), Point(10, 500), FONT_HERSHEY_SIMPLEX, 1, Scalar(255, 255, 0), 5, 8, false);
				str2 << "Marker ID: " << mid << ", Distance(cm): " << mxzdist;
				putText(src, str2.str(), Point(10, 700), FONT_HERSHEY_SIMPLEX, 1, Scalar(255, 255, 0), 5, 8, false);

				imwrite(outfolder + SEP + files[i], src);
			}
		}
		else
		{
			std::ostringstream str3;
			cout << "No Detected Marker!!!" << endl;
			str3 << "No Detected Marker!!!";
			putText(src, str3.str(), Point(10, 700), FONT_HERSHEY_SIMPLEX, 3, Scalar(255, 255, 0), 5, 8, false);
			imwrite(outfolder + SEP + files[i], src);
		}
	}
	return 1;
}
