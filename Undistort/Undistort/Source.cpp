#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include <opencv2/objdetect/objdetect.hpp>
#include <iostream>
#include <stdio.h>

using namespace cv;
using namespace std;

/// Global variables
Mat frame, frameCopy, image;
char* source_window = "Source video";
char* warp_window = "Warped video";
int corners_cnt = 0;
double zoom = 1;
Scalar pointColor(0,128,255);
Scalar lineColor(0, 128, 255);

bool sortX(Point2f p1, Point2f p2) { return (p1.x < p2.x); }

void sortCorners(Point2f * corners)
{
	vector<Point2f> cornersVector(corners, corners + 4);
	Point2f tmp;

	sort(cornersVector.begin(), cornersVector.end(), sortX);
	cout << "sort 1 ";
	if (cornersVector[0].y > cornersVector[1].y) {
		tmp = cornersVector[0];
		cornersVector[0] = cornersVector[1];
		cornersVector[1] = tmp;
	}
	cout << "sort 2 ";

	if (cornersVector[2].y > cornersVector[3].y) {
		tmp = cornersVector[2];
		cornersVector[2] = cornersVector[3];
		cornersVector[3] = tmp;
	}
	cout << "sort 3 ";

	for (int i = 0; i < 4; i++) {
		corners[i] = cornersVector[i];
	}
	cout << "sort 4 ";
}

int outputWidth(Point2f * corners)
{
	int leftMost, rightMost;
	leftMost = rightMost = corners[0].x;

	for (int i = 1; i < 4; i++) {
		if (corners[i].x < leftMost) {
			leftMost = corners[i].x;
		}
		if (corners[i].x > rightMost) {
			rightMost = corners[i].x;
		}
	}

	return zoom*(rightMost - leftMost);
}

int outputHeight(Point2f * corners)
{
	int topMost, bottomMost;
	topMost = bottomMost = corners[0].y;

	for (int i = 1; i < 4; i++) {
		if (corners[i].y < topMost) {
			topMost = corners[i].y;
		}
		if (corners[i].y > bottomMost) {
			bottomMost = corners[i].y;
		}
	}

	return zoom*(bottomMost - topMost);
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

/// @function main 
int main(int argc, char** argv)
{
	Point2f corners[4];

	CvCapture* capture = 0;

	///capture from default cam
	capture = cvCaptureFromCAM(CV_CAP_ANY);
	if (!capture)
	{
		cout << "No camera detected" << endl;
	}

	bool sort = false;

	/// Show what you got
	namedWindow(source_window, CV_WINDOW_AUTOSIZE);
	setMouseCallback(source_window, CallBackFunc, corners);

	if (capture)
	{
		cout << "In capture ..." << endl;
		for (;;)
		{
			///reading frame
			IplImage* iplImg = cvQueryFrame(capture);
			///assigning current frame to MAT 
			frame = iplImg;

			if (frame.empty())
				break;
			if (iplImg->origin == IPL_ORIGIN_TL)
				frame.copyTo(frameCopy);
			else
				flip(frame, frameCopy, 0);

			Mat input = frame.clone();

			for (int i = 0; i < corners_cnt; i++) {
				circle(input, corners[i], 3, pointColor, -1);
			}

			if (corners_cnt >= 4 && sort == false)
			{
				sortCorners(corners);
				sort = true;
				continue;
			}

			if (corners_cnt >= 4)
			{
				/// Create an empty image
				int height = outputHeight(corners);
				int width = (int)(0.7 * height);
				int midX = frame.cols / 2;
				int midY = frame.rows / 2;

				Mat output = Mat(frame.cols, frame.rows, frame.type());

				/// Drawing box around selected area
				line(input, corners[0], corners[1], lineColor);
				line(input, corners[1], corners[3], lineColor);
				line(input, corners[3], corners[2], lineColor);
				line(input, corners[2], corners[0], lineColor);

				Mat mmat(3, 3, CV_32FC1);
				Point2f c1[4] = {
					corners[0], corners[1], corners[2], corners[3]
				};
				Point2f c2[4] = {
					Point2f(midX - width / 2, midY - height / 2), Point2f(midX - width / 2, midY + height / 2), Point2f(midX + width / 2, midY - height / 2), Point2f(midX + width / 2, midY + height / 2)
				};

				warpPerspective(frame, output, getPerspectiveTransform(c1, c2), frame.size());

				namedWindow(warp_window, CV_WINDOW_AUTOSIZE);
				imshow(warp_window, output);
			}

			imshow(source_window, input);
			
			switch (waitKey(10)) {
				case 32:
					corners_cnt = 0;
					sort = false;
					break;
				case 27:
					return 0;
					break;
				default:
					break;
			}
		}
	}

	return 0;
}