#include <opencv2/opencv.hpp>
#include <iostream>

using namespace cv;
using namespace std;

int main(int argc, char* argv[]){
    Mat y(512, 512, CV_8UC1);
    
    Mat qm1(512, 512, CV_8UC1);
    Mat qm2(512, 512, CV_8UC1);
    Mat qm3(512, 512, CV_8UC1);
    Mat image;
    Mat image_ycbcr;
    Mat Ycbcr_channels[3];

    // 3 Quantization matrices given.
    Mat quantization_mat1 = (Mat_<double>(8, 8) <<
        16, 11, 10, 16, 24, 40, 51, 61,
        12, 12, 14, 19, 26, 58, 60, 55,
        14, 13, 16, 24, 40, 57, 69, 56,
        14, 17, 22, 29, 51, 87, 80, 62,
        18, 22, 37, 56, 68, 109, 103, 77,
        24, 35, 55, 64, 81, 104, 113, 92,
        49, 64, 78, 87, 103, 121, 120, 101,
        72, 92, 95, 98, 112, 100, 103, 99
    );

    Mat quantization_mat2 = (Mat_<double>(8, 8) <<
        1, 1, 1, 1, 1, 1, 1, 1,
        1, 1, 1, 1, 1, 1, 1, 1,
        1, 1, 1, 1, 1, 1, 1, 1,
        1, 1, 1, 1, 1, 1, 1, 1,
        1, 1, 1, 1, 1, 1, 1, 1,
        1, 1, 1, 1, 1, 1, 1, 1,
        1, 1, 1, 1, 1, 1, 1, 1,
        1, 1, 1, 1, 1, 1, 1, 1
    );

    Mat quantization_mat3 = (Mat_<double>(8, 8) <<
        100, 100, 100, 100, 100, 100, 100, 100,
        100, 100, 100, 100, 100, 100, 100, 100,
        100, 100, 100, 100, 100, 100, 100, 100,
        100, 100, 100, 100, 100, 100, 100, 100,
        100, 100, 100, 100, 100, 100, 100, 100,
        100, 100, 100, 100, 100, 100, 100, 100,
        100, 100, 100, 100, 100, 100, 100, 100,
        100, 100, 100, 100, 100, 100, 100, 100
    );

    // Extract Y channel from Lena.png
    image = imread("Lena.png", IMREAD_COLOR);
    cvtColor(image, image_ycbcr, COLOR_BGR2YCrCb);
    split(image_ycbcr, Ycbcr_channels);
    for (int j = 0; j < 512; j++) {
        for (int i = 0; i < 512; i++) {
            y.at<uchar>(j, i) = Ycbcr_channels[0].at<uchar>(j, i);}}

    imshow("Original Y", y);

    qm1 = y.clone();
    qm1.convertTo(qm1, CV_32F);

    qm2 = y.clone();
    qm2.convertTo(qm2, CV_32F);

    qm3 = y.clone();
    qm3.convertTo(qm3, CV_32F);

    Mat roi;

    // Process QM1
    for (int i = 0; i < 512; i += 8) {
        for (int j = 0; j < 512; j += 8) {
            Rect rect(i, j, 8, 8);
            roi = qm1(rect);

            // DCT
            dct(roi, roi);

            // Quantization
            for (int k = 0; k < 8; k++) {
                for (int l = 0; l < 8; l++) {
                    roi.at<float>(l, k) = round(roi.at<float>(l, k) / quantization_mat1.at<double>(l, k));}}

            // Inverse Quantization
            for (int k = 0; k < 8; k++) {
                for (int l = 0; l < 8; l++) {
                    roi.at<float>(l, k) = roi.at<float>(l, k) * quantization_mat1.at<double>(l, k);}}

            // Inverse DCT
            dct(roi, roi, DCT_INVERSE);
        }
    }

    qm1.convertTo(qm1, CV_8U);
    imshow("QM1", qm1);

    // Process QM2
    for (int i = 0; i < 512; i += 8) {
        for (int j = 0; j < 512; j += 8) {
            Rect rect(i, j, 8, 8);
            roi = qm2(rect);

            // DCT
            dct(roi, roi);

            // Quantization
            for (int k = 0; k < 8; k++) {
                for (int l = 0; l < 8; l++) {
                    roi.at<float>(l, k) = round(roi.at<float>(l, k) / quantization_mat2.at<double>(l, k));}}

            // Inverse Quantization
            for (int k = 0; k < 8; k++) {
                for (int l = 0; l < 8; l++) {
                    roi.at<float>(l, k) = roi.at<float>(l, k) * quantization_mat2.at<double>(l, k);}}

            // Inverse DCT
            dct(roi, roi, DCT_INVERSE);
        }
    }

    qm2.convertTo(qm2, CV_8U);
    imshow("QM2", qm2);

    // Process QM3
    for (int i = 0; i < 512; i += 8) {
        for (int j = 0; j < 512; j += 8) {
            Rect rect(i, j, 8, 8);
            roi = qm3(rect);

            // DCT
            dct(roi, roi);

            // Quantization
            for (int k = 0; k < 8; k++) {
                for (int l = 0; l < 8; l++) {
                    roi.at<float>(l, k) = round(roi.at<float>(l, k) / quantization_mat3.at<double>(l, k));}}

            // Inverse Quantization
            for (int k = 0; k < 8; k++) {
                for (int l = 0; l < 8; l++) {
                    roi.at<float>(l, k) = roi.at<float>(l, k) * quantization_mat3.at<double>(l, k);}}

            // Inverse DCT
            dct(roi, roi, DCT_INVERSE);
        }
    }

    qm3.convertTo(qm3, CV_8U);
    imshow("QM3", qm3);

    // PSNR Calculation for QM1
    int error;
    double sum = 0;
    double psnr = 0;

    for (int i = 0; i < 512; i++) {
        for (int j = 0; j < 512; j++) {
            error = qm1.at<uchar>(j, i) - y.at<uchar>(j, i);
            error = error * error;
            sum += error;}}

    sum = sum / 262144;
    psnr = 20 * log10(255) - 10 * log10(sum);

    printf("QM1: psnr = %f\n", psnr);

    // PSNR Calculation for QM2
    sum = 0;
    psnr = 0;

    for (int i = 0; i < 512; i++) {
        for (int j = 0; j < 512; j++) {
            error = qm2.at<uchar>(j, i) - y.at<uchar>(j, i);
            error = error * error;
            sum += error;}}

    sum = sum / 262144;
    psnr = 20 * log10(255) - 10 * log10(sum);

    printf("QM2: psnr = %f\n", psnr);

    // PSNR Calculation for QM3
    sum = 0;
    psnr = 0;

    for (int i = 0; i < 512; i++) {
        for (int j = 0; j < 512; j++) {
            error = qm3.at<uchar>(j, i) - y.at<uchar>(j, i);
            error = error * error;
            sum += error;}}

    sum = sum / 262144;
    psnr = 20 * log10(255) - 10 * log10(sum);

    printf("QM3: psnr = %f\n", psnr);

    waitKey(0);
    return 0;}