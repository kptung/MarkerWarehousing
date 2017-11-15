#include "stdafx.h"
#include "IrArInterface.h"

using namespace std;
using namespace cv;

static Scalar blue(255, 0, 0);
static Scalar red(0, 0, 255);
static Scalar green(0, 255, 0);
static Scalar white(255, 255, 255);

/****************************************************************************/
/*  Aruco sample works well with the given marker length in meters         */
/*  and draw the injection point without considering the marker orientation */
/*  Test marker @ 1x1, 2x2, 3x3, 4x4, 5x5 are ok                            */
/*  Author: kptung                                                          */
/*  Modified: kptung, 2017/11/07                                            */
/****************************************************************************/
int main(int argc, char **argv)
{
	std::string infolder("./data/ar_3x3");
	std::string outfolder("./data/arucomarkerout");
	
	/************************************************************************/
	/* the given marker length in meters                                    */
	/************************************************************************/
	float markerLength = 0.03f; // the unit is meter

	std::string cameraFilename("camera-z2.yml");
	//cv::Mat cameraMatrix, distCoeffs;
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
			for (unsigned int j = 0; j < markers.size(); j++)
			{
				int id = markers[j].getMarkerId();
				float xzdist = markers[j].getXZCameraDistance() * 100; 
				
				int ori = markers[j].getMarkerOri();
				cv::Point2f center = markers[j].getMarkerCenter();
				int xzangle = 0, yzangle = 0;
				//markers[j].getCameraAngle(xzangle, yzangle);
				markers[j].getCameraAngle(xzangle, yzangle, ori);

				cout << "marker id: " << id << endl;
				cout << "X-Z viewing angle: " << xzangle << endl;
				cout << "X-Z viewing distance: " << xzdist << endl;
				cout << "Y-Z viewing angle: " << yzangle << endl;

				cv::Mat rvec = markers[j].getRotationMatrix();
				cv::Mat tvec = markers[j].getTransnslationMatrix();

				markers[j].setInjectionPoint(Point3f(0, -0.05f, 0));
				std::vector<cv::Point2f> injpts = findInjection(markers[j].getInjectionPoints(), rvec, tvec, ori, center);
				for (size_t j = 0; j < injpts.size(); j++)
				{
					cv::circle(src, injpts[j], 15, cv::Scalar(255, 0, 0), -1, 10);
				}
				
				markers[j].removeInjectionPoint();
				// add marker center
				cv::circle(src, center, 10, cv::Scalar(0, 255, 0), -1, 10);

				std::ostringstream str1, str2;
				if (xzangle > 0 & yzangle > 0) str1 << " Camera Angle: Right: " << abs(xzangle) << ", Top: " << abs(yzangle);
				else if (xzangle < 0 && yzangle > 0) str1 << " Camera Angle: Left: " << abs(xzangle) << ", Top: " << abs(yzangle);
				else if (xzangle > 0 && yzangle < 0) str1 << " Camera Angle: Right: " << abs(xzangle) << ", Bottom: " << abs(yzangle);
				else if (xzangle < 0 && yzangle < 0) str1 << " Camera Angle: Left: " << abs(xzangle) << ", Bottom: " << abs(yzangle);
				else if (xzangle < 0 && yzangle == 0) str1 << " Camera Angle: Left: " << abs(xzangle) << ", Top: " << abs(yzangle);
				else if (xzangle > 0 && yzangle == 0) str1 << " Camera Angle: Right: " << abs(xzangle) << ", Top: " << abs(yzangle);
				else if (xzangle == 0 && yzangle == 0) str1 << " Camera Angle: Front: " << abs(xzangle) << ", Front: " << abs(yzangle);
				else if (xzangle == 0 && yzangle > 0) str1 << " Camera Angle: Front: " << abs(xzangle) << ", Top: " << abs(yzangle);
				else if (xzangle == 0 && yzangle < 0) str1 << " Camera Angle: Front: " << abs(xzangle) << ", Bottom: " << abs(yzangle);
				putText(src, str1.str(), Point(10, 500), FONT_HERSHEY_SIMPLEX, 3, Scalar(255, 255, 0), 10, 8, false);
				str2 << "Marker ID: " << id << ", Distance(cm): " << xzdist;
				putText(src, str2.str(), Point(10, 700), FONT_HERSHEY_SIMPLEX, 3, Scalar(255, 255, 0), 10, 8, false);

				

				imwrite(outfolder + SEP + files[i], src);
			}
		}
	}
		//save3dMarkerCorners( filename );
	return 1;
	
}
