#include <opencv2/opencv.hpp>
#include <iostream>

using namespace std;
using namespace cv;

int main() {
    Mat moon = imread("moon.png", 0);
    Mat total;
    Mat copied;
    Mat copied2;
    Mat mask = Mat::zeros(moon.size(), moon.type());
    Mat mask2 = Mat::zeros(moon.size(), moon.type());

    Mat blur_filter;
    Mat unsharp_mask;
    Mat sharpening_filter;

    blur(moon, blur_filter, Size(3, 3));
    unsharp_mask = moon - blur_filter;
    sharpening_filter = moon + 2.5 * unsharp_mask;

    Rect rect_left = Rect(0, 0, moon.size().width / 2, moon.size().height);
    rectangle(mask, rect_left, Scalar(255, 255, 255), -1, 8, 0);
    moon.copyTo(copied, mask);

    Rect rect_right = Rect(moon.size().width / 2, 0, moon.size().width / 2, moon.size().height);
    rectangle(mask2, rect_right, Scalar(255, 255, 255), -1, 8, 0);
    sharpening_filter.copyTo(copied2, mask2);

    total = copied + copied2;

    imshow("moon", moon);
    imshow("moon_filtered", total);

    Mat saltpepper = imread("saltpepper.png", 0);
    Mat median_filter;
    mask = Mat::zeros(saltpepper.size(), saltpepper.type());
    mask2 = Mat::zeros(saltpepper.size(), saltpepper.type());

    medianBlur(saltpepper, median_filter, 9);

    rect_left = Rect(0, 0, saltpepper.size().width / 2, saltpepper.size().height);
    rectangle(mask, rect_left, Scalar(255, 255, 255), -1, 8, 0);
    median_filter.copyTo(copied, mask);

    rect_right = Rect(saltpepper.size().width / 2, 0, saltpepper.size().width / 2, saltpepper.size().height);
    rectangle(mask2, rect_right, Scalar(255, 255, 255), -1, 8, 0);
    saltpepper.copyTo(copied2, mask2);

    total = copied + copied2;

    imshow("saltpepper", saltpepper);
    imshow("saltpepper_filtered", total);

    waitKey(0);
    return 0;
}