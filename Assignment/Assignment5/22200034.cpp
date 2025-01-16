#include <opencv2/opencv.hpp>
#include <iostream>

using namespace cv;
using namespace std;

int main() {
    Mat finger_print = imread("finger_print.png", 0);
    Mat adaptive_1 = imread("adaptive_1.jpg", 0);
    Mat adaptive = imread("adaptive.png", 0);

    Mat result_finger_print, result_adaptive_1, result_adaptive;
    threshold(finger_print, result_finger_print, 0, 255, THRESH_BINARY | THRESH_OTSU);

    adaptiveThreshold(adaptive_1, result_adaptive_1, 255, ADAPTIVE_THRESH_MEAN_C, THRESH_BINARY, 33, 10);

    adaptiveThreshold(adaptive, result_adaptive, 255, ADAPTIVE_THRESH_GAUSSIAN_C, THRESH_BINARY, 11, 10);

    imshow("finger_print", result_finger_print);
    imshow("adaptive_1", result_adaptive_1);
    imshow("adaptive", result_adaptive);

    waitKey(0); 
    return 0;}