#include <opencv2/opencv.hpp>
#include <iostream>

using namespace std;
using namespace cv;

// Tracking할 때 속도가 너무 느려서 develop 해야 할 것으로 보임.

int main() {
    CascadeClassifier face_classifier;
    Mat frame, grayframe;
    vector<Rect> faces;

    Mat result, bgModel, fgModel, foreground;
    Mat tracking;

    // Rectangle containing foreground
    Rect rect_foreground;

    Mat m_backproj, hsv;
    Mat m_model3d;
    Rect m_rc;

    float hrange[] = { 0, 180 }; // Hue
    float srange[] = { 0, 255 }; // Saturation
    float vrange[] = { 0, 255 }; // Brightness

    const float* ranges[] = { hrange, srange, vrange }; // hue, saturation, brightness
    int channels[] = { 0, 1, 2 };
    int hist_sizes[] = { 16, 16, 16 };

    bool key_far = false;
    bool key_mid = false;
    bool key_near = false;
    bool key_tracking = false;
    bool key_detecting = false;
    bool flag = false, flag_err = false;

    // open the video
    VideoCapture cap ("Faces.mp4");

    // check if we succeded
    if(!cap.isOpened())
        cout << "Can't open the video" << endl;
    
    // face detecting configuration
    face_classifier.load("haarcascade_frontalface_alt.xml");
    int offset = 0;

    while (true){
        // get a new frame
        cap >> frame;
        Mat origin;
        frame.copyTo(origin);
        tracking = Mat(origin.size(), CV_8UC3, cv::Scalar(255, 0, 0));
        
        if(frame.empty()){
            cout << "empty image" << endl;
            return 0;}

        // convert captured frame from RGB to GRAY scale 
        cvtColor(frame, grayframe, COLOR_BGR2GRAY);

        int size_max = 0;
        int size_min = 0;
        string text;
        if(key_far){
            size_max = 45;
            size_min = 35;
            offset = 3;
            text = "f";}
        
        else if(key_mid){
            size_max = 60;
            size_min = 45;
            offset = 5;
            text = "m";}

        else if(key_near){
            size_max = 80;
            size_min = 65;
            offset = 10;
            text = "n";}
        
        // face detecting
        if( (key_far || key_mid || key_near) && key_detecting == false ){
            flag_err = false;
            face_classifier.detectMultiScale( grayframe, // detect faces using grayframe
                                            faces,
                                            1.2,    // increase search scale by 20% each pass 3
                                            3,      // merge groups of three detections
                                            0,      // not used for a new cascade
                                            Size(size_min, size_min),   // min size
                                            Size(size_max, size_max) ); // max size
            
            Point lb(faces[0].x + faces[0].width, faces[0].y + faces[0].height);
            Point tr(faces[0].x + faces[0].y);

            // point for printing how far faces is
            Point ptext(faces[0].x, faces[0].y + faces[0].height - 5);
            rectangle(frame, lb, tr, Scalar(0, 255 ,0), 3, 4, 0);
            //text to show where it is
            putText(frame, text, ptext, 2, 1.2, Scalar(255, 255, 255));
            key_detecting = true;}
        
        // face tracking
        if(key_detecting == true){
            // convert image from RGB to HSV
            cvtColor(frame, hsv, COLOR_BGR2HSV);

            if(flag == false){
                Rect rc(faces[0].x + offset, faces[0].y + offset , faces[0].width - offset * 2 , faces[0].height - offset * 2);
                Mat mask = Mat::zeros(rc.height, rc.width, CV_8U);

                ellipse(mask, Point(rc.width / 2, rc.height / 2), Size(rc.width / 2, rc.height / 2), 0, 0, 360, 255, FILLED);
                Mat roi(hsv, rc);

                // histogram calculation
                calcHist(&roi, // The source array(s)
                        1,    // The number of source arrays
                        channels, // The channel (dim) to be measured.
                        mask, // A mask to be used on the source array (zeros indicating pixels to be ignored)
                        m_model3d, // The Mat object where the histogram will be stored
                        3,    // The histogram dimensionality.
                        hist_sizes, // The number of bins per each used dimension
                        ranges ); // The range of values to be measured per each dimension

                m_rc = rc;
                flag = true;}

            cap >> frame;
            if(flag == true){
                // histogram backprojection
                // all the arguments are known (the same as used to calculate the histogram),
                // only we add the backproj matrix,
                // which will store the backprojection of the source image (&hue)
                calcBackProject(&hsv, 1, channels, m_model3d, m_backproj, ranges);

                // tracking
                // obtain a window with maximum pixel distribution
                meanShift(m_backproj, // dst
                        m_rc, // initial location of window
                        TermCriteria(TermCriteria::EPS | TermCriteria::COUNT, 20, 1) ); // termination criteria
                rectangle(frame, m_rc, Scalar(0, 255, 0), 3);
                rect_foreground = Rect(m_rc.x - offset*5 , m_rc.y - offset*7 , m_rc.width + offset*10 , m_rc.height + offset*10); }
        }

            if (key_tracking){
                if ( (key_far || key_mid || key_near) && key_detecting==1 ){

                    // grabCut
                    grabCut(origin, // input image
                            result, // segmentation result
                            rect_foreground, //rectangle containing foreground
                            bgModel, fgModel, // models
                            10, // number of iterations
                            GC_INIT_WITH_RECT );// use rectangle
                
                    // Get the pixels marked as likely foreground
                    compare(result, GC_PR_FGD, result, CMP_EQ);

                    // Generate output image
                    foreground = Mat(origin.size(), CV_8UC3, cv::Scalar(255, 0, 0));
                    origin.copyTo(foreground, result);
                    foreground(Rect(rect_foreground.x, rect_foreground.y, rect_foreground.width, rect_foreground.height)).copyTo(tracking
                                (Rect(rect_foreground.x, rect_foreground.y, rect_foreground.width, rect_foreground.height)));
                    imshow("Tracking", tracking);}

                // (warning) You should detect(f, m, n) before click tracking(t)
                else flag_err = true; }
        
        if(flag_err == true)
            putText(frame, "Detect before Tracking", Point(0, 50), 2, 1.2, Scalar(0, 0, 255), 2);
        
        // print the output
        imshow("Faces", frame);

        // user input
        int keyval = waitKey(1000 / cap.get(CAP_PROP_FPS));

        if(keyval == 27) break; // ESC

        else if(keyval == 'f'){ // far
            key_mid = false;
            key_near = false;
            flag = false;
            flag_err = false;
            if(key_detecting == true) key_detecting = false;
            key_far = !(key_far);}
        
        else if(keyval == 'm'){ // middle
            key_far = false;
            key_near = false;
            flag = false;
            flag_err = false;
            if(key_detecting == true) key_detecting = false;
            key_mid = !(key_mid);}

        else if(keyval == 'n'){  // near
            key_far = false;
            key_mid = false;
            flag = false;
            flag_err = false;
            if(key_detecting == true) key_detecting = false;
            key_near = !(key_near);}

        // tracking window
        else if(keyval == 't'){
            if(key_tracking == true)
                destroyWindow("Tracking");
            else flag_err = false;
            key_tracking = !(key_tracking);}

        // reset
        else if(keyval == 'r'){
            key_far = false;
            key_mid = false;
            key_near = false;
            if(key_tracking == true)
                destroyWindow("Tracking");
            key_tracking = false;
            key_detecting = false;
            flag = false;
            flag_err = false;}
    
        }
    return 0;}