#include "opencv2/opencv.hpp"
#include <iostream>

using namespace cv;
using namespace std;

int main() {
    VideoCapture cap("Project2_video.mp4");
    Mat frame, gray_frame, edge, result, foregroundMask, background;
    vector<Vec2f> lines;
    bool first_frame = true;

    double fps = cap.get(CAP_PROP_FPS);

    while (1) {
        cap >> frame;
        if (frame.empty()) break;

        Rect roi(240, 300, 150, 120); 
        Mat roi_frame = frame(roi);

        Canny(roi_frame, edge, 50, 200, 3);
        HoughLines(edge, lines, 1, CV_PI / 180, 150);

        for (int i = 0; i < lines.size(); i++) {
            float rho = lines[i][0], theta = lines[i][1];
            Point p1, p2;
            double a = cos(theta), b = sin(theta);
            double x0 = a * rho, y0 = b * rho;
            p1 = Point(cvRound(x0 + 1000 * (-b)), cvRound(y0 + 1000 * a));
            p2 = Point(cvRound(x0 - 1000 * (-b)), cvRound(y0 - 1000 * a));
            line(roi_frame, p1, p2, Scalar(0, 0, 255), 2);
        }


        cvtColor(frame, gray_frame, COLOR_BGR2GRAY);
        if (first_frame) {
            background = gray_frame.clone();
            first_frame = false;
        }

        absdiff(background, gray_frame, foregroundMask);
        threshold(foregroundMask, foregroundMask, 50, 255, THRESH_BINARY);

        if (!lines.empty()) {
            putText(frame, "Lane departure!", Point(100, 100), FONT_HERSHEY_SIMPLEX, 1, Scalar(255, 0, 0), 2);
        }

        if (sum(foregroundMask)[0] > 10000) {
            putText(frame, "Start Moving!", Point(100, 200), FONT_HERSHEY_SIMPLEX, 1, Scalar(255, 0, 0), 2);
        }

        imshow("Project2", frame);
        waitKey(1000/fps);
    }
    cap.release();
    return 0;
}
