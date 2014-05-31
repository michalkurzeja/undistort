#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include <iostream>
#include <stdio.h>

using namespace cv;
using namespace std;

/// Global variables
char* source_window = "Source image";
char* warp_window = "Warp";
int corners_cnt = 0;

bool sortX(Point2f p1, Point2f p2) { return (p1.x < p2.x); }
bool sortY(Point2f p1, Point2f p2) { return (p1.y < p2.y); }

void sortCorners(Point2f * corners)
{
	vector<Point2f> cornersVector(corners, corners+4);
	Point2f tmp;

	sort(cornersVector.begin(), cornersVector.end(), sortX);
	
	if (cornersVector[0].y > cornersVector[1].y) {
		tmp = cornersVector[0];
		cornersVector[0] = cornersVector[1];
		cornersVector[1] = tmp;
	}

	if (cornersVector[2].y > cornersVector[3].y) {
		tmp = cornersVector[2];
		cornersVector[2] = cornersVector[3];
		cornersVector[3] = tmp;
	}

	for (int i = 0; i < 4; i++) {
		corners[i] = cornersVector[i];
	}
}

void CallBackFunc(int event, int x, int y, int flags, void* userdata)
{
	if (event != EVENT_LBUTTONDOWN) {
		return;
	}

	if (corners_cnt >= 4) {
		return;
	}

	Point2f * corners = (Point2f *) userdata;
	
	corners[corners_cnt++] = Point2f(x, y);

	cout << "Corner #" << corners_cnt << "(" << x << "," << y << ") added" << endl;
}

/** @function main */
int main(int argc, char** argv)
{
	string filename;

	Point2f corners[4];

	Mat warp_mat(2, 3, CV_32FC1);
	Mat input;

	/// Load the image
	input = imread(argv[1], 1);

	/// Show what you got
	namedWindow(source_window, CV_WINDOW_AUTOSIZE);

	setMouseCallback(source_window, CallBackFunc, corners);

	imshow(source_window, input);

	while (1) {
		if (waitKey(100) == 27) {
			if (corners_cnt >= 4) {
				sortCorners(corners);
				break;
			}
		}
	}

	/// Create an empty image
	Mat output;
	output = Mat::zeros(842, 595, input.type());

	Mat mmat(3, 3, CV_32FC1);
	Point2f c1[4] = {
		corners[0], corners[1], corners[2], corners[3]
	};
	Point2f c2[4] = {
		Point2f(0, 0), Point2f(0, output.cols - 1), Point2f(output.rows - 1, 0), Point2f(output.rows - 1, output.cols - 1)
	};
	mmat = getPerspectiveTransform(c1, c2);

	warpPerspective(input, output, mmat, output.size());

	namedWindow(warp_window, CV_WINDOW_AUTOSIZE);
	imshow(warp_window, output);

	/// Wait until user exits the program
	waitKey(0);

	return 0;
}