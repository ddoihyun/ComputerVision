#include <opencv2/opencv.hpp>
#include <iostream>

using namespace cv;
using namespace std;

int main() {
    VideoCapture cap;
    if(cap.open("Road.mp4")==0) return -1;


    Rect left_roi(200, 400, 400, 200); 
    Rect right_roi(600, 400, 400, 200); 

    namedWindow("Left canny");
    moveWindow("Left canny", 200, 0);
    namedWindow("Right canny");
    moveWindow("Right canny", 600, 0);

    Mat frame, temp, LeftCanny, RightCanny;
    double fps = cap.get(CAP_PROP_FPS);
    double time_in_msec = 0;

    while (time_in_msec < 20000) {
        cap >> frame;
        if(frame.empty()) break;
        time_in_msec = cap.get(CAP_PROP_POS_MSEC);

        Mat left_frame = frame(left_roi);
        Mat right_frame = frame(right_roi);

        cvtColor(left_frame, temp, COLOR_BGR2GRAY);
        blur(temp, temp, Size(5, 5)); 
        Canny(temp, LeftCanny, 10, 60); 

        cvtColor(right_frame, temp, COLOR_BGR2GRAY);
        blur(temp, temp, Size(5, 5)); 
        Canny(temp, RightCanny, 10, 60); 

        imshow("Left canny", LeftCanny);
        imshow("Right canny", RightCanny);
        
        waitKey(1000/fps);
    }
    return 0;}