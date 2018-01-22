#include "IrArInterface.h"
#include <chrono>
#include <ctime>
using namespace std;
using namespace cv;

/****************************************************************************/
/*  Aruco basic sample only for ID, orientation and center                  */
/*  Test marker @ 1x1, 2x2, 3x3, 4x4, 5x5 are ok                            */
/*  Author: kptung                                                          */
/*  Modified: kptung, 2017/12/20                                            */
/****************************************************************************/
int main87645864564868(int argc, char **argv)
{
	std::string infolder("./data/artest");
	std::string outfolder("./data/arout");
	
	/************************************************************************/
	/* the given marker length in meters                                    */
	/************************************************************************/
	//float markerLength = 0.03f; // the unit is meter
	float markerLength = 3; // the unit is meter

	std::string cameraFilename("camera-z2.yml");
	//std::string cameraFilename("a6k_mr/bt300jj130-camera.yml");
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
		auto tstart = std::chrono::high_resolution_clock::now();
		bool flag = findArucoMarkers(src, markers);
		auto tend = std::chrono::high_resolution_clock::now();
		auto diff = std::chrono::duration_cast<std::chrono::duration<double>>(tend - tstart);
		// time estimation
		bool tflag = false;
		if (tflag)
		{
			std::ofstream out1;
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
				int mori = markers.at(j).getMarkerOri();
				cv::Point2f mcenter = markers.at(j).getMarkerCenter();

				cout << "marker id: " << mid << endl;
				cout << "marker ori: " << mori << endl;

				cv::circle(src, mcenter, 10, cv::Scalar(0, 255, 0), -1, 10);
				std::ostringstream str2;
				str2 << "Marker ID: " << mid;
				putText(src, str2.str(), Point(10, 200), FONT_HERSHEY_SIMPLEX, 1, Scalar(255, 0, 0), 5, 1, false);
				imwrite(outfolder + SEP + files[i], src);
			}
		}
		else
		{
			std::ostringstream str;
			cout << "No Detected Marker!!!" << endl;
			str << "No Detected Marker!!!";
			putText(src, str.str(), Point(10, 200), FONT_HERSHEY_SIMPLEX, 1, Scalar(255, 255, 0), 5, 1, false);
			imwrite(outfolder + SEP + files[i], src);
		}
	}
	return 1;
}
