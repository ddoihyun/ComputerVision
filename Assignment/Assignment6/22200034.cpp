#include <opencv2/opencv.hpp>
#include <iostream>

using namespace cv;
using namespace std;

int main() {
    string FileName;
    cout << "Enter query image name: ";
    cin >> FileName;

    Mat query = imread("Assignment6_Datasets/query_image/" + FileName);
    if (query.empty()) {
        cout << "No file!" << endl;
        return -1;}
    resize(query, query, Size(640, 480));

    Ptr<ORB> orbF = ORB::create(1000);
    vector<KeyPoint> keypointsQuery;
    Mat descriptorsQuery;

    orbF->detectAndCompute(query, noArray(), keypointsQuery, descriptorsQuery);

    vector<String> dbImagePaths;
    glob("Assignment6_Datasets/DBs/*.jpg", dbImagePaths);

    cout << "Sample Image Load Size : " << dbImagePaths.size() << endl;

    BFMatcher matcher(NORM_HAMMING);
    vector<DMatch> bestMatches;
    Mat bestMatchingImage;
    vector<KeyPoint> bestKeypointsDB;
    int bestMatchCount = 0;

    for (size_t i = 0; i < dbImagePaths.size(); i++) {
        Mat dbImage = imread(dbImagePaths[i]);

        resize(dbImage, dbImage, Size(640, 480));

        vector<KeyPoint> keypointsDB;
        Mat descriptorsDB;
        orbF->detectAndCompute(dbImage, noArray(), keypointsDB, descriptorsDB);

        vector<vector<DMatch>> Matches;
        // matcher.knnMatch(descriptorsQuery, descriptorsDB, Matches, 2);
        matcher.knnMatch(descriptorsDB, descriptorsQuery, Matches, 2);

        vector<DMatch> goodMatches;
        float nndr = 0.6f;
        for (int i = 0; i < Matches.size(); i++) {
            if (Matches.at(i).size() == 2 && Matches.at(i).at(0).distance <= nndr * Matches.at(i).at(1).distance) {
                goodMatches.push_back(Matches[i][0]);}}


        cout << "Image number " << i + 1 << " Matching: " << goodMatches.size() << endl;

        if (goodMatches.size() > bestMatchCount) {
            bestMatchCount = goodMatches.size();
            bestMatches = goodMatches;
            bestMatchingImage = dbImage;
            bestKeypointsDB = keypointsDB;
        }
    }

    if (bestMatchCount > 0) {
        cout << "Match found!" << endl;

        Mat imgMatches;

        drawMatches(bestMatchingImage, bestKeypointsDB, query, keypointsQuery, bestMatches, imgMatches, Scalar::all(-1), Scalar::all(-1), vector<char>(), DrawMatchesFlags::NOT_DRAW_SINGLE_POINTS);
        imshow("Query", query);
        imshow("Best_matching", imgMatches);
        waitKey(0);} 
    else
        cout << "Matching failed" << endl;

    return 0;}