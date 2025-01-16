#include <opencv2/opencv.hpp>
#include <iostream>

using namespace cv;
using namespace std;

Mat negative_transformation(Mat img){
    for (int y = 0; y < img.rows; y++)
        for (int x = 0; x < img.cols; x++)
            img.at<uchar>(y, x) = 255 - img.at<uchar>(y, x);
    return img;}

Mat gamma_transformation(Mat img){
    img.convertTo(img, CV_32F);
    float gamma = 2.5;
    normalize(img, img, 0, 1.0, NORM_MINMAX);

    for (int y = 0; y < img.rows; y++)
        for (int x = 0; x < img.cols; x++)
            img.at<float>(y, x) = pow(img.at<float>(y, x), gamma);

    normalize(img, img, 0, 255, NORM_MINMAX);
    convertScaleAbs(img, img, 1.0, 0);
    return img;}

Mat white_space(Mat img) {
    Mat bgr_channels[3];
    split(img, bgr_channels);

    double avg;
    int sum, temp, i, j, c;

    for (c = 0; c < img.channels(); c++) {
        sum = 0;
        avg = 0.0f;
        for (i = 0; i < img.rows; i++)
            for (j = 0; j < img.cols; j++)
                sum += bgr_channels[c].at<uchar>(i, j);

        avg = sum / (img.rows * img.cols);

        for (i = 0; i < img.rows; i++){
            for (j = 0; j < img.cols; j++){
                temp = (128 / avg) * bgr_channels[c].at<uchar>(i, j);
                if (temp > 255)
                    bgr_channels[c].at<uchar>(i, j) = 255;
                else
                    bgr_channels[c].at<uchar>(i, j) = temp;}}
    }

    merge(bgr_channels, 3, img);
    return img;}

int main() {
    Mat frame;
    VideoCapture cap;

    if (cap.open("video.mp4") == 0){
        cout << "no such file!" << endl;
        waitKey(0);}

    double fps = cap.get(CAP_PROP_FPS);
    double wait_time = 1000 / fps;
    int keyboard = -1;
    int status = 82;
    int prev_status = status;
    Mat copy_frame;
    Mat frame_HSV;
    Mat sharpened_HSV;
    Mat sharpened_RGB;
    Mat sharpened;
    Mat blur_V;
    Mat HSV_channels[3];

    while (1){
        cap >> frame;
        if (frame.empty()){
            cout << "end of video" << endl;
            break;}

        keyboard = waitKey(wait_time);
        if (keyboard != -1) {
            status = keyboard;}

        if (status == 78 || status == 110){ // Press "N" or "n"
            cvtColor(frame, frame_HSV, COLOR_BGR2HSV);
            split(frame_HSV, HSV_channels);
            HSV_channels[2] = negative_transformation(HSV_channels[2]);
            merge(HSV_channels, 3, frame);
            cvtColor(frame, frame, COLOR_HSV2BGR);} 

        else if (status == 71 || status == 103){ // Press "G" or "g"
            cvtColor(frame, frame_HSV, COLOR_BGR2HSV);
            split(frame_HSV, HSV_channels);
            HSV_channels[2] = gamma_transformation(HSV_channels[2]);
            merge(HSV_channels, 3, frame);
            cvtColor(frame, frame, COLOR_HSV2BGR);}

        else if (status == 72 || status == 104){ // Press "H" or "h"
            cvtColor(frame, frame_HSV, COLOR_BGR2HSV);
            split(frame_HSV, HSV_channels);
            equalizeHist(HSV_channels[2], HSV_channels[2]);
            merge(HSV_channels, 3, frame);
            cvtColor(frame, frame, COLOR_HSV2BGR);}

        else if (status == 83 || status == 115){ // Press "S" or "s"
            cvtColor(frame, frame_HSV, COLOR_BGR2HSV);
            split(frame_HSV, HSV_channels);
            for (int y = 0; y < HSV_channels[0].rows; y++){
                for (int x = 0; x < HSV_channels[0].cols; x++){
                    if (HSV_channels[0].at<uchar>(y, x) <= 9 || HSV_channels[0].at<uchar>(y, x) >= 23)
                        HSV_channels[1].at<uchar>(y, x) = 0;}}
            merge(HSV_channels, 3, frame);
            cvtColor(frame, frame, COLOR_HSV2BGR);}

        else if (status == 67 || status == 99){ // Press "C" or "c"
            cvtColor(frame, frame_HSV, COLOR_BGR2HSV);
            split(frame_HSV, HSV_channels);
            for (int y = 0; y < HSV_channels[0].rows; y++){
                for (int x = 0; x < HSV_channels[0].cols; x++){
                    if (HSV_channels[0].at<uchar>(y, x) > 129)
                        HSV_channels[0].at<uchar>(y, x) -= 129;
                    else 
                        HSV_channels[0].at<uchar>(y, x) += 50;}}
            merge(HSV_channels, 3, frame);
            cvtColor(frame, frame, COLOR_HSV2BGR);}

        else if (status == 85 || status == 117){ // Press "U" or "u"
            cvtColor(frame, frame_HSV, COLOR_BGR2HSV);
            // Split the HSV frame into channels
            split(frame_HSV, HSV_channels);
            //Apply blur to the V channel
            blur(HSV_channels[2], blur_V, Size(9, 9));
            // Perform unsharp masking on the V channel
            sharpened = HSV_channels[2] + 1.5 * (HSV_channels[2] - blur_V);
            // Update the V channel in the HSV frame
            HSV_channels[2] = sharpened;
            // Merge the modifited HSV channels back into a singel frame
            merge(HSV_channels, 3, sharpened_HSV);
            cvtColor(sharpened_HSV, sharpened_RGB, COLOR_HSV2BGR);
            // Normalize pixel values to the valid range
            normalize(sharpened_RGB, frame, 0, 255, NORM_MINMAX);}

        else if (status == 65 || status == 97){ // Press "A" or "a"
            blur(frame, frame, Size(9, 9));}

        else if (status == 87 || status == 119){ // Press "W" or "w"
            frame = white_space(frame);}

        else if (status == 82 || status == 114){ // Press "R" or "r"
            prev_status = status;
            imshow("video", frame);
            continue;} 

        else if (status == 27) break; // Press "ESC"
         
        else {
            status = prev_status;
            continue;}

        prev_status = status;
        imshow("video", frame);}

    return 0;}