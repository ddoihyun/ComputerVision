#include <opencv2/opencv.hpp>
#include <iostream>

using namespace cv;
using namespace std;

int main() {
    Mat finger, adaptive0, adaptive1;
    finger = imread("finger_print.png", 0);
    adaptive0 = imread("adaptive.png", 0);
    adaptive1 = imread("adaptive_1.jpg", 0);

    threshold(finger, finger, 0, 255, THRESH_BINARY | THRESH_OTSU);
    adaptiveThreshold(adaptive1, adaptive1, 255, ADAPTIVE_THRESH_MEAN_C, THRESH_BINARY, 85, 10);
    adaptiveThreshold(adaptive0, adaptive0, 255, ADAPTIVE_THRESH_MEAN_C, THRESH_BINARY, 7, 10);

    imshow("finger_print", finger);
    imshow("adaptive", adaptive0);
    imshow("adaptive_1", adaptive1);

    waitKey(0);

    return 0;}