
#include <opencv2/opencv.hpp>
#include <iostream>
#include <opencv2/highgui.hpp>
#include "aruco/aruco.hpp"
#include <opencv2/opencv.hpp>
#include <time.h>


using namespace cv;
using namespace std;

/************************************************************************/
/*  Aruco marker generator with the given marker length in pixels       */
/*  Author: opencv_contrib                                              */
/*  Modified: kptung, 2017/11/09                                        */
/************************************************************************/
void useage()
{
	cerr << "Usage: IrArMarkerCreator [outFolder] [num_of_ID] [size_cm] <options>" << endl;
	cerr << "where <options> are one or more of the following:\n";
	cerr << "\t<-r|g|b|c [b] [g] [r]> marker in color Red/Green/Blue/custom" << endl;
	cerr << "\t<-exclude [exnum] [ID1] [ID2] ....> exclude IDs" << endl;
	cerr << "\t<-input [innum] [ID1] [ID2] ....> input IDs" << endl;
	cerr << "\t<-DPI [dpi]> Dots Per Inch (72:screen, 300:printer)" << endl;
	exit(1);
}

int main(int argc, char *argv[])
{
	bool colorflag = false;
	bool showImage = false;
	bool rotflag = false;
	bool inflag = false;
	bool exflag = false;

	cv::Vec3b color;
	std::vector<int> excludeIdList;
	std::vector<int> markerIdList;
	int mnum = -1;	// marke_num
	int msize = -1; // marker_size in cm
	int exnum = -1; // exclude marker number
	int innum = -1; // input marker number
	float DPI = 74.2; // Dot Per Inch (defalt:74)
	std::ostringstream outFolder; // output_folder
	cv::Mat markerImg; // output_image
	int dictionaryId = 16; // pre-defined dictionary
	int borderBits = 1; // marker border
	std::ostringstream output;

	if (argc < 3) {
		useage();
	}

	outFolder << argv[1] <<"/";
	argc--;
	argv++;
	mnum = atoi(argv[1]);
	argc--;
	argv++;
	msize = atoi(argv[1]);
	argc--;
	argv++;

	int ok;
	while (argc > 1) {
		ok = false;
		if (strcmp(argv[1], "-r") == 0) {
			colorflag = true;
			color[0] = 0;
			color[1] = 0;
			color[2] = 255;
			argc--;
			argv++;
			ok = true;
		}
		else if (strcmp(argv[1], "-g") == 0) {
			colorflag = true;
			color[0] = 0;
			color[1] = 255;
			color[2] = 0;
			argc--;
			argv++;
			ok = true;
		}
		else if (strcmp(argv[1], "-b") == 0) {
			colorflag = true;
			color[0] = 255;
			color[1] = 0;
			color[2] = 0;
			argc--;
			argv++;
			ok = true;
		}
		else if (strcmp(argv[1], "-c") == 0) {
			colorflag = true;
			argc--;
			argv++;
			color[0] = (atoi(argv[1])<0)? (atoi(argv[1])+256):atoi(argv[1]);
			argc--;
			argv++;
			color[1] = (atoi(argv[1]) < 0) ? (atoi(argv[1]) + 256) : atoi(argv[1]);
			argc--;
			argv++;
			color[2] = (atoi(argv[1]) < 0) ? (atoi(argv[1]) + 256) : atoi(argv[1]);
			argc--;
			argv++;
			ok = true;
		}
		else if (strcmp(argv[1], "-DPI") == 0) {
			argc--;
			argv++;
			DPI = atoi(argv[1]);
			argc--;
			argv++;
			ok = true;
		}
		else if (strcmp(argv[1], "-exclude") == 0) {
			exflag = true;
			argc--;
			argv++;
			exnum = atoi(argv[1]);
			argc--;
			argv++;
			for(int i = 0; i < exnum; i++)
			{
				excludeIdList.push_back(atoi(argv[1]));
				argc--;
				argv++;
			}
			ok = true;
		}
		else if (strcmp(argv[1], "-input") == 0) {
			inflag = true;
			argc--;
			argv++;
			innum = atoi(argv[1]);
			argc--;
			argv++;
			for (int i = 0; i < innum; i++)
			{
				markerIdList.push_back(atoi(argv[1]));
				argc--;
				argv++;
			}
			ok = true;
		}
	}

	// marker size generator
	int markerSize = round(msize * DPI / 2.54);

	// random marker generator
	if (!inflag)
	{
		//initialize random seed
		srand(time(NULL));
		for (int i = 0; i < mnum; i++)
		{
			int id = round(rand() % 1024);
			if (!exflag)
			{
				bool flag = false;
				if (markerIdList.size() == 0)
				{
					markerIdList.push_back(id);
					flag = false;
				}
				else
					for (int j = 0; j < markerIdList.size(); j++)
					{
						while (flag)
						{
							if (id != markerIdList.at(j))
							{
								markerIdList.push_back(id);
								flag = false;
							}
							else
							{
								flag = true;
								id = round(rand() % 1024);
							}
						}
					}
					
			}	
			else 
			{
				bool flag = false;
				for (int j = 0; j < exnum; j++)
				{
					while (flag)
					{
						if (id != excludeIdList.at(j))
						{
							markerIdList.push_back(id);
							flag = false;
						}
						else
						{
							flag = true;
							id = round(rand() % 1024);
						}
					}
				}
			}
		}
	}
	
	cv::Ptr<cv::aruco::Dictionary> dictionary =
		cv::aruco::getPredefinedDictionary(aruco::PREDEFINED_DICTIONARY_NAME(dictionaryId));

	for (int i = 0; i < markerIdList.size(); i++)
	{
		aruco::drawMarker(dictionary, markerIdList.at(i), markerSize, markerImg, borderBits);
		// convert bit-color: black->white, white->black
		cv::bitwise_not(markerImg, markerImg);
		// add border
		int top = (int)(0.15*markerImg.rows), bottom = (int)(0.15*markerImg.rows);
		int left = (int)(0.15*markerImg.cols), right = (int)(0.15*markerImg.cols);
		int borderType = BORDER_CONSTANT;
		copyMakeBorder(markerImg, markerImg, top, bottom, left, right, borderType, Scalar(0, 0, 0));
		// make a color marker
		cv::Mat colorMarker(markerImg.size(), CV_8UC3);
		if (colorflag)
		{
			for (int y = 0; y < markerImg.rows; y++)
				for (int x = 0; x < markerImg.cols; x++)
				{
					cv:Scalar intensity = markerImg.at<uchar>(y, x);
					if (intensity.val[0] == 255)
						colorMarker.at<Vec3b>(y, x) = cv::Vec3b(255, 255, 255);
					if (intensity.val[0] == 0)
						colorMarker.at<Vec3b>(y, x) = color;
				}
			colorMarker.copyTo(markerImg);
		}

		// RotateFlags 
		// ROTATE_90_CLOCKWISE = 0, //Rotate 90 degrees clockwise
		// ROTATE_180 = 1, //Rotate 180 degrees clockwise
		// ROTATE_90_COUNTERCLOCKWISE = 2, //Rotate 270 degrees clockwise
		if (rotflag)
		{
			cv::Mat tmp;
			markerImg.copyTo(tmp);
			cv::rotate(markerImg, tmp, ROTATE_90_CLOCKWISE);
			cv::rotate(markerImg, tmp, ROTATE_180);
			cv::rotate(markerImg, tmp, ROTATE_90_COUNTERCLOCKWISE);
		}

		if (showImage) {
			cv::imshow("marker", markerImg);
			waitKey(0);
		}

		output << outFolder.str() << markerIdList.at(i) << ".jpg";
		cv::imwrite(output.str(), markerImg);
		output.str("");
		output.clear();
	}
	return 0;
}
