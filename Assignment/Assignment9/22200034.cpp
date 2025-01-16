#include <opencv2/opencv.hpp>
#include <opencv2/highgui.hpp>
#include <iostream>

using namespace cv;
using namespace std;

int main() {
    // Open video webcam
    VideoCapture cap = VideoCapture(0);
    if (!cap.isOpened()) {
        cerr << "Error: Could not open webcam!" << endl;
        return -1;
    }
    int successes = 0;

    int numBoards = 70; // Total number of corners
    int numCornersHor = 10; // Number of horizontal corners
    int numCornersVer = 7;  // Number of vertical corners
    int Rect_size = 20;     // Length of one side of the rectangle

    int numSquares = (numCornersHor - 1) * (numCornersVer - 1);
    Size board_sz = Size(numCornersHor, numCornersVer);

    // Containers
    vector<vector<Point3f>> object_points;
    vector<vector<Point2f>> image_points;
    vector<Point2f> corners;
    vector<Point3f> obj;

    for (int i = 0; i < numCornersHor; i++) {
        for (int j = 0; j < numCornersVer; j++) {
            obj.push_back(Point3f(i * Rect_size, j * Rect_size, 0.0f));
        }
    }

    Mat img, gray;
    cap >> img;
    if (img.empty()) {
        cerr << "Error: Could not read frame from webcam!" << endl;
        return -1;
    }
    cout << "Image size: " << img.size() << endl;

    while (successes < numBoards) {
        cap >> img;
        if (img.empty()) break; // End of video stream

        cvtColor(img, gray, COLOR_BGR2GRAY);
        if (waitKey(1) == 27) break; // Stop capturing by pressing ESC

        // Finds the positions of internal corners of the chessboard
        bool found = findChessboardCorners(
            gray,         // Source chessboard view. Must be grayscale.
            board_sz,     // Number of inner corners per chessboard row and column.
            corners,      // Output array of detected corners.
            CALIB_CB_ADAPTIVE_THRESH // Use adaptive thresholding for better accuracy
        );

        if (found) {
            // Refines the corner locations
            cornerSubPix(
                gray,       // Input single-channel, 8-bit or float image.
                corners,    // Initial coordinates of corners to refine
                Size(11, 11), // Size of search window at each pyramid level
                Size(-1, -1), // Zero zone for filtering; (-1, -1) disables it
                // TermCriteria(TermCriteria::EPS | TermCriteria::COUNT, 30, 0.1)
                TermCriteria(TermCriteria::EPS | TermCriteria::MAX_ITER, 30, 0.1)
            );

            // Renders the detected chessboard corners
            drawChessboardCorners(
                img,        // Destination image
                board_sz,   // Number of corners per chessboard
                corners,    // Output array of detected corners
                found       // Whether the complete board was found
            );

            image_points.push_back(corners);
            object_points.push_back(obj);
            printf("Snap stored\n");
            successes++;
        }

        imshow("win1", img);
        imshow("win2", gray);
        waitKey(1000);
    }

    cout << "Complete!" << endl;

    Mat intrinsic = Mat(3, 3, CV_32FC1);
    Mat distCoeffs;
    vector<Mat> rvecs;
    vector<Mat> tvecs;

    intrinsic.ptr<float>(0)[0] = 1;
    intrinsic.ptr<float>(1)[1] = 1;

    // Finds the camera intrinsic and extrinsic parameters
    calibrateCamera(
        object_points, // 3D world coordinates
        image_points,  // Corresponding 2D image points
        img.size(),    // Size of the image
        intrinsic,     // Intrinsic camera matrix
        distCoeffs,    // Distortion coefficients
        rvecs,         // Rotation vectors
        tvecs          // Translation vectors
    );

    Mat imageUndistorted;

    // Print intrinsic parameters
    cout << "================Intrinsic Parameters================" << endl;
    for (int i = 0; i < intrinsic.rows; i++) {
        for (int j = 0; j < intrinsic.cols; j++) {
            cout << intrinsic.at<float>(i, j) << "\t";
        }
        cout << endl;
    }
    cout << "===================================================" << endl;

    cap.release();
    waitKey();
    return 0;
}
