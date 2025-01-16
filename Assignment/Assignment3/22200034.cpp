#include <opencv2/opencv.hpp>
#include <iostream>

using namespace cv;
using namespace std;

int main() {

    Mat moon = imread("moon.png", 0);

    Mat moon_blurred, laplacian, abs_laplacian, moon_sharpened;
    GaussianBlur(moon, moon_blurred, Size(3, 3), 0, 0, BORDER_DEFAULT);
    Laplacian(moon_blurred, laplacian, CV_16S, 1, 1, 0);
    convertScaleAbs(laplacian, abs_laplacian);
    moon_sharpened = moon + abs_laplacian;

    Mat mask = Mat::zeros(moon.size(), moon.type());
    mask(Rect(0, 0, moon.cols / 2, moon.rows))=255;
    moon.copyTo(moon_sharpened, mask);

    Mat unsharp_masked;
    addWeighted(moon, 1.5, moon_sharpened, -0.5, 0, unsharp_masked);

    imshow("moon", moon);
    imshow("moon_filtered", moon_sharpened);


    Mat saltnpepper = imread("saltnpepper.png", 0);
    Mat saltnpepper_filtered;
    medianBlur(saltnpepper, saltnpepper_filtered, 9);

    mask = Mat::zeros(saltnpepper.size(), saltnpepper.type());
    mask(Rect(saltnpepper.cols / 2, 0, saltnpepper.cols / 2, saltnpepper.rows))=255;

    saltnpepper.copyTo(saltnpepper_filtered, mask);

    imshow("saltnpepper", saltnpepper);
    imshow("saltnpepper_filtered", saltnpepper_filtered);

    waitKey(0);
    return 0;}
