#include <opencv2/opencv.hpp>
#include <iostream>

using namespace cv;
using namespace std;

int main() {
    CascadeClassifier face_classifier;
    Mat frame, grayframe;
    vector<Rect> faces;

    VideoCapture cap("Faces.mp4");
    double fps = cap.get(CAP_PROP_FPS);

    if (!cap.isOpened()) {
        cout << "Not such file" << endl;
        return -1;}

    face_classifier.load("haarcascade_frontalface_alt.xml");

    char mode = ' ';
    bool tracking = false;         
    bool face_detected = false;    
    bool detect_warning = false;     
    Size min_size, max_size;
    Rect face_roi;

    while (true) {
        cap >> frame;
        if (frame.empty()) break;
        cvtColor(frame, grayframe, COLOR_BGR2GRAY);

        if (mode == 'n') {
            min_size = Size(75, 75);
            max_size = Size(80, 80);}

        else if (mode == 'm') { 
            min_size = Size(45, 45);
            max_size = Size(55, 55);}

        else if (mode == 'f') { 
            min_size = Size(35, 35);
            max_size = Size(40, 40);} 

        else {
            faces = vector<Rect>();
            face_detected = false;}

        if (mode == 'n' || mode == 'm' || mode == 'f') {
            faces.clear();
            face_classifier.detectMultiScale(grayframe, faces, 1.1, 3, 0, min_size, max_size);

            if (!faces.empty()) {
                face_roi = faces[0]; 
                face_detected = true; 
                detect_warning = false;
                
                Point lb(face_roi.x + face_roi.width, face_roi.y + face_roi.height);
                Point tr(face_roi.x, face_roi.y);
                rectangle(frame, lb, tr, Scalar(0, 255, 0), 3, 4, 0);}
        }

        if (tracking && face_detected) {
            Mat Tracking = Mat::zeros(frame.size(), CV_8UC3);
            Tracking.setTo(Scalar(255, 0, 0));
            
            Mat face_crop = frame(face_roi);
            face_crop.copyTo(Tracking(face_roi));

            imshow("Tracking", Tracking);}

        if (detect_warning)
            putText(frame, "Detect before Tracking", Point(50, 50), FONT_HERSHEY_SIMPLEX, 1, Scalar(0, 0, 255), 2);

        char key = (char)waitKey(1000 / fps);

        if (key == 27) break;
        else if (key == 'n'){ 
            mode = 'n'; 
            face_detected = false; 
            detect_warning = false;}

        else if (key == 'm'){ 
            mode = 'm'; 
            face_detected = false; 
            detect_warning = false;}

        else if (key == 'f') { 
            mode = 'f'; 
            face_detected = false; 
            detect_warning = false;}

        else if (key == 'r') { 
            mode = ' '; 
            faces = vector<Rect>();
            face_detected = false;
            tracking = false;
            detect_warning = false;
            destroyWindow("Tracking");} 

        else if (key == 't') {
            if (!face_detected) detect_warning = true;
            else {
                tracking = !tracking;
                if (!tracking)
                    destroyWindow("Tracking");}
        }

        imshow("Faces", frame);}

    return 0;}