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

	float alpha = 0.8f, alpha2=0.2f;

	cv::Mat src2 = imread("data/stage_border.png");
	cv::resize(src2, src2, cv::Size(1280, 720), 1);

	std::vector<std::string> files;
	get_files_in_directory(infolder, files);

	for (unsigned int i = 0; i < files.size(); i++)
	{
		cout << "i: " << i + 1 << "/" << files.size() << endl;
		cout << "file: " << files[i] << endl;

		//CV_LOAD_IMAGE_UNCHANGED, CV_LOAD_IMAGE_GRAYSCALE, CV_LOAD_IMAGE_COLOR
		cv::Mat src = imread(infolder + SEP + files[i], CV_LOAD_IMAGE_COLOR);
		cv::Mat imcopy;
		src.copyTo(imcopy);
		cv::resize(src, src, cv::Size(1280, 720), 1);
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

				std::vector<cv::Point3f> Injectionpts;
				if (mid == 666)
				{
					// find injection pts
					cv::Point3f Injection(0, -0.04f, 0);
					Injectionpts = make_vector<cv::Point3f>() << Injection;
					Injectionpts.push_back(cv::Point3f(0, -0.065f, 0));
				}
				else if (mid == 777)
				{
					// find injection pts
					cv::Point3f Injection(0.05f, 0, 0);
					Injectionpts = make_vector<cv::Point3f>() << Injection;
					Injectionpts.push_back(cv::Point3f(0, 0.05f, 0));
				}
				std::vector<cv::Point2f> injpts = findInjection(Injectionpts, rvec, tvec, mori, mcenter);
				cv::Mat overlay;
				src.copyTo(overlay);
				if (mid == 666)
				{
					cv::rectangle(overlay, cv::Point2f(injpts[0].x-100, injpts[0].y), cv::Point2f(injpts[1].x+100, injpts[1].y), cv::Scalar(0, 255, 255), -1, 8, 0);
					cv::addWeighted(src2, alpha, src, 1 - alpha, 0, src);
					cv::addWeighted(overlay, alpha2, src, 1 - alpha2, 0, src);
				}
				if (mid == 777)
				{
					cv::rectangle(overlay, cv::Point2f(mcenter.x, injpts[1].y), cv::Point2f(injpts[0].x, mcenter.y), cv::Scalar(0, 255, 255), -1, 8, 0);
					cv::addWeighted(src2, alpha, src, 1 - alpha, 0, src);
					cv::addWeighted(overlay, alpha2, src, 1 - alpha2, 0, src);
				}
				
			}
			src.convertTo(src, -1, 2.5, 20);
			imwrite(outfolder + SEP + files[i], src);
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
