#include <cv.h>
#include <highgui.h>
#include <iostream>

using namespace cv;
using namespace std;

int main()
{
    int numBoards = 0;
    int numCornersHor;
    int numCornersVer;

	printf("Enter number of corners along width: ");
    scanf("%d", &numCornersHor);
 
    printf("Enter number of corners along height: ");
    scanf("%d", &numCornersVer);
 
    printf("Enter number of boards: ");
    scanf("%d", &numBoards);

	int numSquares = numCornersHor * numCornersVer;
    Size board_sz = Size(numCornersHor, numCornersVer);

	VideoCapture capture = VideoCapture(0);

	vector<vector<Point3f>> object_points;
    vector<vector<Point2f>> image_points;

	ector<Point2f> corners;
    int successes=0;

	Mat image;
    Mat gray_image;
    capture >> image;

	vector<Point3f> obj;
    for(int j=0;j<numSquares;j++)
        obj.push_back(Point3f(j/numCornersHor, j%numCornersHor, 0.0f));

