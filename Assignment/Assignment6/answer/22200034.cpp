#include <opencv2/opencv.hpp>
#include <iostream>

using namespace cv;
using namespace std;

int main() {
    Mat compare_img;
    Mat query_image, descriptors1, descriptors2;
    Ptr<ORB> orbF = ORB::create(1000);
    vector<KeyPoint> keypoints1, keypoints2;
    vector<vector<DMatch>> matches[10];
    vector<DMatch> goodMatches[10];
    BFMatcher matcher(NORM_HAMMING);
    Mat imgMatches;

    int i, k;
    int maxMatchSize = 0;
    float nndr;
    // String path("D:\\WWCV_dataset\\WWDBs\\WWHandong*_1.jpg");
    String path("./Assignment6_Datasets/DBs/Handong*_1.jpg");
    vector<String> str;

    string query_name;
    cout << "Enter query image name: ";
    cin >> query_name;

    glob(path, str, false);

    query_image = imread("./Assignment6_Datasets/query_image/" + query_name);

    if (query_image.empty()) {
        cout << "No query image file! \n" << endl;
        return -1;} 
    else {
        resize(query_image, query_image, Size(640, 480));}

    cout << "Sample Image Load Size: " << str.size() << endl;
    if (str.size() == 0) {
        cout << "Image does not exist.\n" << endl;}

    for (int cnt = 0; cnt < str.size(); cnt++) {
        compare_img = imread(str[cnt]);
        resize(compare_img, compare_img, Size(640, 480));

        // Compute ORB Features
        orbF->detectAndCompute(compare_img, noArray(), keypoints1, descriptors1);
        orbF->detectAndCompute(query_image, noArray(), keypoints2, descriptors2);

        // KNN Matching
        k = 2;
        matcher.knnMatch(descriptors1, descriptors2, matches[cnt], k);

        nndr = 0.6f;
        for (i = 0; i < matches[cnt].size(); i++) {
            if (matches[cnt].at(i).size() == 2 &&
                matches[cnt].at(i).at(0).distance <= nndr * matches[cnt].at(i).at(1).distance) {
                goodMatches[cnt].push_back(matches[cnt][i][0]);}}

        if (goodMatches[cnt].size() > maxMatchSize) {
            maxMatchSize = goodMatches[cnt].size();}

        cout << "Image number " << cnt + 1 << " Matching: " << goodMatches[cnt].size() << endl;}

    for (int cnt = 0; cnt < str.size(); cnt++) {
        if (goodMatches[cnt].size() == maxMatchSize) {
            compare_img = imread(str[cnt]);
            resize(compare_img, compare_img, Size(640, 480));

            // Compute ORB Features
            orbF->detectAndCompute(compare_img, noArray(), keypoints1, descriptors1);
            orbF->detectAndCompute(query_image, noArray(), keypoints2, descriptors2);

            // Draw Matching
            drawMatches(compare_img, keypoints1, query_image, keypoints2, goodMatches[cnt], imgMatches, Scalar::all(-1),
                        Scalar(-1), vector<char>(), DrawMatchesFlags::NOT_DRAW_SINGLE_POINTS);

            cout << "Match found!" << endl;
            imshow("Query", query_image);
            imshow("Best_matching", imgMatches);
            waitKey(0);
            break;}}

    return 0;}