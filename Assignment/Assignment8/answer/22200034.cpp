#include <opencv2/opencv.hpp>
#include <iostream>

using namespace std;
using namespace cv;

struct MouseParams {
    int count = 0;
    Mat img;
    Mat background;
    Mat mask;
    Mat crop;
    Mat result;
    Point trapezoid[1][4];
    vector<Point2f> custom, contest;
};

static void onMouse(int event, int x, int y, int, void* param) {
    MouseParams* mp = (MouseParams*)param;

    if (event == EVENT_LBUTTONDOWN && mp->count < 4) { // 왼쪽 버튼 클릭
        // 좌표 삽입 (시계 방향)
        mp->custom.push_back(Point2f(x, y));
        mp->trapezoid[0][mp->count] = Point(x, y);
        mp->count++;}
}

int main() {
    Mat t_frame, c_frame;
    VideoCapture t_cap, c_cap;

    // 비디오 파일 열기
    if (t_cap.open("Timesquare.mp4") == 0 || c_cap.open("contest.mp4") == 0) {
        cout << "no such file!" << endl;
        waitKey(0);
        return -1;}

    int fps = t_cap.get(CAP_PROP_FPS);

    MouseParams mp;
    mp.contest.push_back(Point2f(0, 0));
    mp.contest.push_back(Point2f(c_cap.get(CAP_PROP_FRAME_WIDTH), 0));
    mp.contest.push_back(Point2f(c_cap.get(CAP_PROP_FRAME_WIDTH), c_cap.get(CAP_PROP_FRAME_HEIGHT)));
    mp.contest.push_back(Point2f(0, c_cap.get(CAP_PROP_FRAME_HEIGHT)));

    while (1) {
        c_cap >> c_frame;
        t_cap >> t_frame;

        mp.img = c_frame;
        mp.background = t_frame;

        mp.mask = Mat::zeros(mp.background.size(), mp.background.type());

        if (t_frame.empty()) {
            cout << "end of video" << endl;
            break;}

        imshow("input", mp.img);
        imshow("background", mp.background);

        if (mp.custom.size() == 4) {
            const Point* ppt[1] = { mp.trapezoid[0] };
            int npt[] = { 4 };

            Mat homo_mat = getPerspectiveTransform(mp.contest, mp.custom);
            warpPerspective(mp.img, mp.result, homo_mat, Size(mp.background.size().width, mp.background.size().height));
            fillPoly(mp.mask, ppt, npt, 1, Scalar(255, 255, 255), 8);
            mp.background.copyTo(mp.crop, mp.mask);
            imshow("background", mp.background - mp.crop + mp.result);}

        else {
            for (size_t i = 0; i < mp.custom.size(); i++) {
                circle(mp.background, mp.custom[i], 3, Scalar(0, 0, 255), 5);}
            imshow("background", mp.background);}

        setMouseCallback("background", onMouse, (void*)&mp);
        waitKey(1000 / fps);}

    return 0;}