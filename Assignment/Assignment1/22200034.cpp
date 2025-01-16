#include "opencv2/opencv.hpp"
#include <iostream>

using namespace cv; 
using namespace std;

int main() {
    Mat frame;
    int fps;
    int delay;
    VideoCapture cap;

    if (cap.open("background.mp4") == 0) {
        cout << "no such file!" << endl;
        waitKey(0);}

    fps = cap.get(CAP_PROP_FPS);
    delay = 1000 / fps;

    int f_count = 0;
    int total = cap.get(CAP_PROP_FRAME_COUNT);

    while (f_count < 3 * fps) {
        cap >> frame;
        if (frame.empty()) {
            cout << "end of video" << endl;
            break;}

        cout << "frames: " << f_count + 1 << " / " << total << endl;
        imshow("video", frame);
        waitKey(delay);
        
        f_count++;}

    waitKey(0);
    return 0;}