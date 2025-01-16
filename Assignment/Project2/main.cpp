#include "opencv.hpp"
#include <iostream>

using namespace cv;
using namespace std;

int main() {

    Mat frame, blurgray, blurImg, canny_frame, blurgray2, blurImg2, canny_frame2;
    VideoCapture cap;
    float rho = 0, theta = 0;
    vector<Vec2f> lines, lines2;
    vector<bool> buffer(30, false);
    Rect roi(340, 300, 70, 90), roi2(180, 340, 70, 40);
    Ptr<BackgroundSubtractor> bg_model = createBackgroundSubtractorMOG2();


    // Check file existence
    if (cap.open("Project2_video.mp4") == 0) {
        cout << "No such file!" << endl;
        waitKey(0);
    }

    int fps = cap.get(CAP_PROP_FPS);
    int delay = 100 / fps;
    bool LaneDeparture = false;
    bool LaneDeparture2 = false;
    bool carMoving = false;
    bool frontCarMoving = false;


    while (1) {
        // Read the next frame
        cap >> frame;

        // Check if it's the end of the frame, and if so, exit the while statement.
        if (frame.empty()) {
            cout << "end of the video" << endl;
            break;
        }
        // Alert a warning text ¡°Lane departure!¡± when lane departure of our car is detected.

        // Declare a region frame for lane departure detection
        Mat roi_frame = frame(roi).clone(), HSV;
        Mat vChannel[3];

        // Convert the roi_frame to the HSV color space
        cvtColor(roi_frame, HSV, COLOR_BGR2HSV);

        // Split the HSV frame into individual channels
        split(HSV, vChannel);

        Mat blurredVChannel;

        // Perform blur function with mask size as 5X5
        GaussianBlur(vChannel[2], blurredVChannel, Size(5, 5), 5, 5, BORDER_DEFAULT);
        vChannel[2] = blurredVChannel;

        // Merge the modified channels back into the HSV frame
        merge(vChannel, 3, HSV);

        // Convert the frame back to the BGR color space
        cvtColor(HSV, blurImg, COLOR_HSV2BGR);

        // Convert the blur image to the gray scale image before performing canny edge detection
        cvtColor(blurImg, blurgray, COLOR_BGR2GRAY);

        // Perform canny edge detection
        Canny(blurgray, canny_frame, 90, 110);

        rho = 0;
        theta = 0;

        // Using the Hough Transform in order to find lines in the image
        HoughLines(canny_frame, lines, 1, CV_PI / 180, 20);

        for (int i = 0; i < lines.size(); i++) {
            rho += lines[i][0];
            theta += lines[i][1];
        }

        // Calculate average of rho and theta of filtered right lines
        rho /= lines.size();
        theta /= lines.size();
        cout << "Theta: " << theta << endl;
        // Detect lane departure by thresholding with theta value
        if ((theta >= 2.6 && theta <= 3.2) || (theta >= 0.13 && theta <= 1.2)) {
            LaneDeparture = true;
        }
        else
            LaneDeparture = false;

        // Declare second region frame for lane departure detection (left lane detection)
        Mat roi_frame2 = frame(roi2).clone(), HSV2;
        Mat vChannel2[3];

        // Convert the roi_frame to the HSV color space
        cvtColor(roi_frame2, HSV2, COLOR_BGR2HSV);

        // Split the HSV frame into individual channels
        split(HSV2, vChannel2);

        Mat blurredVChannel2;


        // Perform blur function with mask size as 5X5
        GaussianBlur(vChannel2[2], blurredVChannel2, Size(5, 5), 5, 5, BORDER_DEFAULT);
        vChannel2[2] = blurredVChannel2;

        // Merge the modified channels back into the HSV frame
        merge(vChannel2, 3, HSV2);

        // Convert the frame back to the BGR color space
        cvtColor(HSV2, blurImg2, COLOR_HSV2BGR);

        // Convert the blur image to the gray scale image before performing canny edge detection
        cvtColor(blurImg2, blurgray2, COLOR_BGR2GRAY);

        // Perform canny edge detection
        Canny(blurgray2, canny_frame2, 60, 90);

        // Perform lines filtering
        rho = 0;
        theta = 0;

        // Using the Hough Transform in order to find lines in the image
        HoughLines(canny_frame2, lines2, 1, CV_PI / 180, 25);

        for (int i = 0; i < lines2.size(); i++) {
            rho += lines2[i][0];
            theta += lines2[i][1];
        }

        // Calculate average of rho and theta of filtered right lines
        rho /= lines2.size();
        theta /= lines2.size();

        // Detect lane departure by thresholding with theta value
        if ((theta >= 1.5 && theta <= 1.7) || (theta >= 0.6 && theta <= 1.07)) {
            LaneDeparture2 = false;
        }
        else {
            LaneDeparture2 = true;
        }

        // Ensure the buffer size does not exceed 5 elements
        if (buffer.size() > 5) {
            buffer.erase(buffer.begin());
        }

        // Check if there are at least 13 'true' values in the buffer
        if (count(buffer.begin(), buffer.end(), true) >= 13) {
            // If the condition is met, display "Lane Departure" using putText for prevent flicking
            putText(frame, "Lane Departure", Point(10, 60), FONT_HERSHEY_SIMPLEX, 1, Scalar(0, 0, 255), 2);
        }
        // Add the current LaneDeparture status to the buffer
        buffer.push_back(LaneDeparture);
        // Alert warning text "Start Moving!" when our car is stationary and the car in front starts moving from a stop
        Mat foregroundMask;
        Rect start_roi(330, 260, 50, 50);

        // Determine whether a car is moving or stationary using MoG2, a background extraction algorithm.
        bg_model->apply(frame, foregroundMask);
        GaussianBlur(foregroundMask, foregroundMask, Size(11, 11), 3.5, 3.5);
        threshold(foregroundMask, foregroundMask, 100, 255, THRESH_BINARY);

        // Consider any non-zero value in the foregroundMask to be a moving object.
        // Set the appropriate threshold to determine if the car is moving or not
        int nonZeroCount = 0;
        int carMoving_threshold = 20000;
        int frontCarMoving_threshold = 1050;

        // Loop through each pixel in the foregroundMask image to count non-zero pixels
        for (int y = 0; y < foregroundMask.rows; y++) {
            for (int x = 0; x < foregroundMask.cols; x++) {
                // If pixel value is not zero, increase the count for non-zero pixel
                if (foregroundMask.at<uchar>(y, x) != 0) {
                    nonZeroCount++;
                }
            }
        }
        // Check if the count of non-zero pixels exceeds the carMoving_threshold
        if (nonZeroCount > carMoving_threshold) {
            carMoving = true;
        }
        else {
            carMoving = false;
        }

        // Set the car to detect if the car in front of it is moving and output "Start Moving!" if it does, unless the car is in motion.
        if (!carMoving) {
            Mat frontCarMask = foregroundMask(start_roi);

            int frontCarNonZeroCount = 0;
            for (int y = 0; y < frontCarMask.rows; y++) {
                for (int x = 0; x < frontCarMask.cols; x++) {
                    if (frontCarMask.at<uchar>(y, x) != 0) {
                        frontCarNonZeroCount++;
                    }
                }
            }

            // frontCarNonZeroCount = true if the car in front moves, and throw an exception for the first frame
            if (frontCarNonZeroCount > frontCarMoving_threshold && !frontCarMoving && frontCarNonZeroCount != 2500) {
                frontCarMoving = true;
            }
            else if (frontCarNonZeroCount <= frontCarMoving_threshold && frontCarMoving) {
                frontCarMoving = false;
            }
        }

        // put the texts which "Start Moving!" and "Lane departure!" if conditions are met
        if (!carMoving && frontCarMoving)
            putText(frame, "Start Moving!", Point(10, 60), FONT_HERSHEY_SIMPLEX, 1, Scalar(0, 0, 255), 2);

        if (LaneDeparture && LaneDeparture2 && count(buffer.begin(), buffer.end(), true) >= 13)
            putText(frame, "Lane Departure", Point(10, 60), FONT_HERSHEY_SIMPLEX, 1, Scalar(0, 0, 255), 2);


        // Display window
        imshow("Project 2", frame);

        // Wait for delay and handle user input
        waitKey(delay);
    }

    waitKey(0);
}