
#include <iostream>
#include <opencv2/highgui.hpp>
#include "aruco/aruco.hpp"

using namespace cv;
using namespace std;
/************************************************************************/
/*  Aruco marker generator with the given marker length in pixels       */
/*  Author: opencv_contrib                                              */
/*  Modified: kptung, 2017/11/09                                        */
/************************************************************************/
int main12345(int argc, char *argv[]) 
{
	int dictionaryId = 16;
    int markerId = 139;
    int borderBits = 1;
	int markerSize = 114; // Marker size in pixels, 1 cm = 37.7954 pixels, when image DPI is 96;
    bool showImage = 0;
	bool rotflag = false;
	std::ostringstream str1;
	str1 << "D:/workprojs/III.Projs/IrMarker-dev_kp/lib-irglass/src/main/jni/data/aruco/aruco" << markerId << ".jpg";
	//cout << str1.str() << endl;

	cv::Ptr<cv::aruco::Dictionary> dictionary =
        cv::aruco::getPredefinedDictionary(aruco::PREDEFINED_DICTIONARY_NAME(dictionaryId));

    Mat markerImg;
    aruco::drawMarker(dictionary, markerId, markerSize, markerImg, borderBits);

	cv::bitwise_not(markerImg, markerImg);

	int top = (int)(0.15*markerImg.rows), bottom = (int)(0.15*markerImg.rows);
	int left = (int)(0.15*markerImg.cols), right = (int)(0.15*markerImg.cols);
	int borderType = BORDER_CONSTANT;
	copyMakeBorder(markerImg, markerImg, top, bottom, left, right, borderType, Scalar(0, 0, 0));

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

    imwrite(str1.str(), markerImg);

    return 0;
}
