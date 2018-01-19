
#include <opencv2/opencv.hpp>
#include <iostream>
#include <opencv2/highgui.hpp>
#include "aruco/aruco.hpp"
#include <opencv2/opencv.hpp>

using namespace cv;
using namespace std;

/************************************************************************/
/*  Aruco marker generator with the given marker length in pixels       */
/*  Author: opencv_contrib                                              */
/*  Modified: kptung, 2017/11/09                                        */
/************************************************************************/
int main(int argc, char *argv[])
{
	int dictionaryId = 16;
	std::vector<int> markerIdList{ 25,153,275,279,373,433,477,806,919,1013 };
	int markerId = 25;
	int borderBits = 1;
	int markerSize = 240; // Marker size in pixels, 1 cm = 30 pixels
	std::vector<int> markerSizeList{ 30,60,90,120,150,180,210,240,270,300 }; //1, 2, 3, 4, 5, 6, 7, 8, 9, 10
	bool showImage = 0;
	bool rotflag = false;
	bool colorflag = true;

	//cout << str1.str() << endl;

	cv::Ptr<cv::aruco::Dictionary> dictionary =
		cv::aruco::getPredefinedDictionary(aruco::PREDEFINED_DICTIONARY_NAME(dictionaryId));

	cv::Mat markerImg;
	int id = -1;
	std::ostringstream str1;
	//aruco::drawMarker(dictionary, markerId, markerSize, markerImg, borderBits);

	//for (int i = 0; i < 1; i++)
	for (int i = 0; i < markerIdList.size(); i++)
	{
		id = markerIdList.at(i);
		aruco::drawMarker(dictionary, id, markerSize, markerImg, borderBits);
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
						colorMarker.at<Vec3b>(y, x) = cv::Vec3b(0, 0, 255);
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
			imshow("marker", markerImg);
			waitKey(0);
		}

		str1 << "D:/workprojs/III.Projs/out/armarker/" << id << ".jpg";
		imwrite(str1.str(), markerImg);
		str1.str("");
		str1.clear();
	}
	return 0;
}
