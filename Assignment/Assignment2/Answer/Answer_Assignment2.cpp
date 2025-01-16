#include "opencv2/opencv.hpp"
#include <iostream>
#include <cmath>

using namespace cv;
using namespace std;

int main(){
    Mat image, rotated_image, result_image;
    float gamma = 10;
    unsigned char pix[256];

    // 0 in the 2nd parameter means read img in grayscale
    image = imread("lena.png", 0);
    rotated_image = Mat::zeros(image.cols, image.rows, image.type());

    // Geberate a 90-degree rotated image
    for(int j=0; j<image.cols; j++)
        for(int i=0; i<image.rows; i++)
            rotated_image.at<uchar>(j,i) = image.at<uchar>(i, image.cols-j-1);

    result_image = rotated_image.clone();

    for(int i=0; i<256; i++)
        pix[i] = saturate_cast<uchar>(pow((float)(i/255.0), gamma) * 255.0f);
    
    // pixel value is smaller than 127 -> negative transformation, otherwise-> gamma transformation
    for(int j=0; j<rotated_image.rows; j++){
        for(int i=0; i<rotated_image.cols; i++){
            if(rotated_image.at<uchar>(j,i) < 127)
                // negative transformation
                result_image.at<uchar>(j,i) = 255 - rotated_image.at<uchar>(j,i);
            else
                // gamma transformation
                result_image.at<uchar>(j,i) = pix[rotated_image.at<uchar>(j,i)];}}
    
    imshow("gray image", image);
    imshow("result", result_image);
    waitKey(0);
    
    return 0;}