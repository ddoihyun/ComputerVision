#include <opencv2/opencv.hpp>
#include <iostream>

using namespace cv;
using namespace std;

void Negative(Mat& frame) {
    Mat hsv_image, hsv_channels[3];
    cvtColor(frame, hsv_image, COLOR_BGR2HSV);

    split(hsv_image, hsv_channels);

    for (int j = 0; j < hsv_channels[2].rows; j++)
        for (int i = 0; i < hsv_channels[2].cols; i++)
            hsv_channels[2].at<uchar>(j, i) = 255 - hsv_channels[2].at<uchar>(j, i);

    Mat negative_hsv_image;
    merge(hsv_channels, 3, negative_hsv_image);

    cvtColor(negative_hsv_image, frame, COLOR_HSV2BGR);
}

void Gamma(Mat& frame) {
    Mat image = frame.clone();
    float gamma = 2.5;
    unsigned char pix[256];

    for (int i = 0; i < 256; i++) 
        pix[i] = saturate_cast<uchar>(pow(i / 255.0, gamma) * 255.0f);

    Mat hsv_image;
    cvtColor(image, hsv_image, COLOR_BGR2HSV);

    Mat hsv_channels[3];
    split(hsv_image, hsv_channels);


    for (int j = 0; j < hsv_channels[2].rows; j++) {
        for (int i = 0; i < hsv_channels[2].cols; i++) {
            hsv_channels[2].at<uchar>(j, i) = pix[hsv_channels[2].at<uchar>(j, i)];}}

    merge(hsv_channels, 3, hsv_image);

    cvtColor(hsv_image, frame, COLOR_HSV2BGR);
}

void HistogramEqualization(Mat& frame) {
    Mat hsv_image;
    cvtColor(frame, hsv_image, COLOR_BGR2HSV);

    Mat hsv_channels[3];
    split(hsv_image, hsv_channels);

    equalizeHist(hsv_channels[2], hsv_channels[2]);

    Mat hist_equalized_hsv;
    merge(hsv_channels, 3, hist_equalized_hsv);

    cvtColor(hist_equalized_hsv, frame, COLOR_HSV2BGR);
}

void ColorSlicing(Mat& frame) {
    Mat hsv, mask, grayFrame;
    cvtColor(frame, hsv, COLOR_BGR2HSV);

    inRange(hsv, Scalar(9, 100, 100), Scalar(23, 255, 255), mask);
    
    cvtColor(frame, grayFrame, COLOR_BGR2GRAY);
    cvtColor(grayFrame, grayFrame, COLOR_GRAY2BGR);
    
    frame.copyTo(grayFrame, mask);
    
    frame = grayFrame.clone();
}

void ColorConversion(Mat& frame) {
    Mat hsv;
    cvtColor(frame, hsv, COLOR_BGR2HSV);

    for (int i = 0; i < hsv.rows; i++) {
        for (int j = 0; j < hsv.cols; j++) {
            int hue = hsv.at<Vec3b>(i, j)[0];
            if (hue > 129) hue -= 129;
            else hue += 50;
            hsv.at<Vec3b>(i, j)[0] = hue;}}
    cvtColor(hsv, frame, COLOR_HSV2BGR);
}

void AverageFiltering(Mat& frame) {
    blur(frame, frame, Size(9, 9));
}

void Sharpening(Mat& frame) {
    Mat blurred;
    blur(frame, blurred, Size(9, 9));
    addWeighted(frame, 1.5, blurred, -0.5, 0, frame);
}

void WhiteBalancing(Mat& frame) {
    Mat bgr_channels[3];
    split(frame, bgr_channels);

    double avg;
    int sum, temp;

    for (int c = 0; c < frame.channels(); c++) {
        sum = 0;
        avg = 0.0f;
        for (int i = 0; i < frame.rows; i++) {
            for (int j = 0; j < frame.cols; j++) {
                sum += bgr_channels[c].at<uchar>(i, j);}}

        avg = sum / (frame.rows * frame.cols);
        for (int i = 0; i < frame.rows; i++) {
            for (int j = 0; j < frame.cols; j++) {
                temp = (128 / avg) * bgr_channels[c].at<uchar>(i, j);
                if (temp > 255) bgr_channels[c].at<uchar>(i, j) = 255;
                else bgr_channels[c].at<uchar>(i, j) = temp;}}}

    merge(bgr_channels, 3, frame);
}

int main() {
    VideoCapture cap;

    if(cap.open("video.mp4")==0){
        cout << "no such file!" << endl;
        waitKey(0);}

    Mat frame, Origin;
    int Current = 114; // 'r' 

    while (true) {
        cap >> frame;
        if (frame.empty())
            break;

        Origin = frame.clone();

        switch (Current) {
            case 110: // 'n'
                Negative(frame);
                break;
            case 103: // 'g'
                Gamma(frame);
                break;
            case 104: // 'h'
                HistogramEqualization(frame);
                break;
            case 115: // 's'
                ColorSlicing(frame);
                break;
            case 99:  // 'c'
                ColorConversion(frame);
                break;
            case 97:  // 'a'
                AverageFiltering(frame);
                break;
            case 117: // 'u'
                Sharpening(frame);
                break;
            case 119: // 'w'
                WhiteBalancing(frame);
                break;
            case 114: // 'r'
                frame = Origin.clone();
                break;
        }

        imshow("video", frame);

        char key = waitKey(30);
        if (key == 27) break;
        else if (key != -1)
            Current = key;
    }

    return 0;}