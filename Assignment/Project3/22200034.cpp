#include <opencv2/opencv.hpp>
#include <iostream>

using namespace cv;
using namespace std;

int main() {
    Mat img1 = imread("pano1.JPG");
    Mat img2 = imread("pano2.JPG");
    Mat img3 = imread("pano3.JPG");
    Mat img4 = imread("pano4.JPG");

    if (img1.empty() || img2.empty() || img3.empty() || img4.empty()) {
        cout << "No such image!" << endl;
        return -1;}

    Ptr<ORB> orb = ORB::create(1000);
    BFMatcher matcher(NORM_HAMMING);

    int panoWidth = img1.cols * 4;
    int panoHeight = img1.rows * 2;
    Mat panorama = Mat::zeros(Size(panoWidth, panoHeight), img1.type());

    Point2f offset(panoWidth / 4, panoHeight / 4);
    Mat roi(panorama, Rect(offset.x, offset.y, img1.cols, img1.rows));
    img1.copyTo(roi);

    Mat current_pano = panorama.clone();

    for (int k = 1; k < 4; k++) {
        Mat img = (k == 1) ? img2 : (k == 2) ? img3 : img4;
        Mat descriptors1, descriptors2, H, result;
        vector<KeyPoint> keypoints1, keypoints2;
        vector<vector<DMatch>> matches;
        vector<DMatch> goodMatches;

        orb->detectAndCompute(current_pano, noArray(), keypoints1, descriptors1);
        orb->detectAndCompute(img, noArray(), keypoints2, descriptors2);

        matcher.knnMatch(descriptors1, descriptors2, matches, 2);

        float nndr = 0.75f;
        for (int i = 0; i < matches.size(); i++) {
            if (matches[i][0].distance < nndr * matches[i][1].distance) {
                goodMatches.push_back(matches[i][0]);}}

        vector<Point2f> points1, points2;

        for (size_t i = 0; i < goodMatches.size(); i++) {
            points1.push_back(keypoints1[goodMatches[i].queryIdx].pt);
            points2.push_back(keypoints2[goodMatches[i].trainIdx].pt);}

        H = findHomography(points2, points1, RANSAC);

        warpPerspective(img, result, H, panorama.size());

        Mat mask;
        cvtColor(result, mask, COLOR_BGR2GRAY);
        threshold(mask, mask, 1, 255, THRESH_BINARY);
        result.copyTo(panorama, mask);

        current_pano = panorama.clone();
    }

    Rect crop_region(offset.x, offset.y, panoWidth / 2, panoHeight / 2);
    Mat panorama_img = panorama(crop_region);

    imshow("panorama_img", panorama_img);
    waitKey(0);
    
    return 0;}