#include "opencv2/opencv.hpp"
#include <iostream>
#include <cmath>

using namespace cv;
using namespace std;

int main() {
    Mat image, result_image;
    float gamma = 10.0;

    image = imread("lena.png", 0);
    if (image.empty()) {
        cout << "Could not open or find the image!" << endl;
        return -1;
    }

    transpose(image, result_image);   
    flip(result_image, result_image, 1);  

    unsigned char pix[256];
    for (int i = 0; i < 256; i++)
        pix[i] = saturate_cast<uchar>(pow(i / 255.0, 1.0 / gamma) * 255.0);

    for (int j = 0; j < result_image.rows; j++) {
        for (int i = 0; i < result_image.cols; i++) {
            if (result_image.at<uchar>(j, i) < 127) 
                result_image.at<uchar>(j, i) = 255 - result_image.at<uchar>(j, i);
            else 
                result_image.at<uchar>(j, i) = pix[result_image.at<uchar>(j, i)];
        }
    }

    imshow("gray image", image);
    imshow("result", result_image);
    waitKey(0);

    return 0;}