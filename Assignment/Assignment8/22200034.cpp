#include <opencv2/opencv.hpp>
#include <iostream>

using namespace cv;
using namespace std;

struct MouseParams {
    Mat img;                 
    vector<Point2f> points;};

static void onMouse(int event, int x, int y, int, void* param) {
    MouseParams* mp = (MouseParams*)param;

    if (event == EVENT_LBUTTONDOWN){
        if (mp->points.size() < 4){
            mp->points.push_back(Point2f(x, y));
            circle(mp->img, Point(x, y), 5, Scalar(0, 0, 255), -1);
            imshow("background", mp->img);}}

    if (event == EVENT_RBUTTONDOWN) {
        mp->points.clear();}
}

int main() {
    VideoCapture capBackground("Timesquare.mp4");
    VideoCapture capInput("contest.mp4");

    if (!capBackground.isOpened() || !capInput.isOpened()) {
        cout << "No such File!" << endl;
        return -1;}

    Mat backgroundFrame, inputFrame;
    capBackground >> backgroundFrame;
    capInput >> inputFrame;

    if (backgroundFrame.empty() || inputFrame.empty()) {
        cout << "End of Video" << endl;
        return -1;}

    MouseParams mp;
    mp.img = backgroundFrame.clone();

    namedWindow("background");
    setMouseCallback("background", onMouse, (void*)&mp);

    Size overlaySize = inputFrame.size();
    vector<Point2f> overlayPoints = {
        Point2f(0, 0),
        Point2f(overlaySize.width, 0),
        Point2f(overlaySize.width, overlaySize.height),
        Point2f(0, overlaySize.height)};

    while (true) {
        capBackground >> backgroundFrame;
        if (backgroundFrame.empty()) break;

        mp.img = backgroundFrame.clone();
        for (size_t i = 0; i < mp.points.size(); i++)
            circle(mp.img, mp.points[i], 3, Scalar(0, 0, 255), 5);

        imshow("background", mp.img);

        if (mp.points.size() == 4) {
            Mat homoMat = getPerspectiveTransform(overlayPoints, mp.points);

            Mat blackFrame = Mat::zeros(inputFrame.size(), inputFrame.type());
            Mat blackTransformed;
            warpPerspective(blackFrame, blackTransformed, homoMat, backgroundFrame.size());

            Mat contestResize;
            warpPerspective(inputFrame, contestResize, homoMat, backgroundFrame.size());

            Mat resultFrame = backgroundFrame.clone();
            for (int y = 0; y < blackTransformed.rows; y++) {
                for (int x = 0; x < blackTransformed.cols; x++) {
                    if (blackTransformed.at<Vec3b>(y, x) != Vec3b(0, 0, 0))
                        resultFrame.at<Vec3b>(y, x) = blackTransformed.at<Vec3b>(y, x);}
            }

            for (int y = 0; y < contestResize.rows; y++) {
                for (int x = 0; x < contestResize.cols; x++) {
                    if (contestResize.at<Vec3b>(y, x) != Vec3b(0, 0, 0))
                        resultFrame.at<Vec3b>(y, x) = contestResize.at<Vec3b>(y, x);}
            }

            imshow("background", resultFrame);
        }

        capInput >> inputFrame;
        if (inputFrame.empty()) {
            capInput.set(CAP_PROP_POS_FRAMES, 0);
            capInput >> inputFrame;}
        imshow("input", inputFrame);

        if (waitKey(30) == 27) break;}

    return 0;}