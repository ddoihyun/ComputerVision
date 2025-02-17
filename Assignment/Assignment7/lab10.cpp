// #include <opencv2/opencv.hpp>
// #include <iostream>

// using namespace cv;
// using namespace std;

// int main() {
//     CascadeClassifier face_classifier;
//     Mat frame, grayframe;
//     vector<Rect> faces;
//     int i;

//     // Open the webcam
//     VideoCapture cap(0);

//     // Check if we succeeded
//     if (!cap.isOpened()) {
//         cout << "Could not open camera" << endl;
//         return -1;
//     }

//     // Face detection configuration
//     face_classifier.load("haarcascade_frontalface_alt.xml");

//     while (true) {
//         // Get a new frame from webcam
//         cap >> frame;
        
//         // Convert captured frame to grayscale
//         cvtColor(frame, grayframe, COLOR_BGR2GRAY);

//         // Detect faces in the grayscale frame
//         face_classifier.detectMultiScale(
//             grayframe,
//             faces,
//             1.1, // Increase search scale by 10% each pass
//             3,   // Merge groups of three detections
//             0,   // Not used for a new cascade
//             Size(30, 30) // Minimum size for detection
//         );

//         // Draw the results
//         for (i = 0; i < faces.size(); i++) {
//             Point lb(faces[i].x + faces[i].width, faces[i].y + faces[i].height);
//             Point tr(faces[i].x, faces[i].y);
//             rectangle(frame, lb, tr, Scalar(0, 255, 0), 3, 4, 0);
//         }

//         // Print the output
//         imshow("Face Detection", frame);
//         if (waitKey(33) == 27) break; // ESC to exit
//     }

//     return 0;
// }

