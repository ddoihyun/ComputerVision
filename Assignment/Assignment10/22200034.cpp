#include <opencv2/opencv.hpp>
#include <iostream>

using namespace cv;
using namespace std;

double calculatePSNR(const Mat& origin, const Mat& reconstructed) {
    Mat diff;
    absdiff(origin, reconstructed, diff);
    diff.convertTo(diff, CV_32F);
    double mse = 0.0;
    for (int i = 0; i < diff.rows; i++) {
        for (int j = 0; j < diff.cols; j++) {
            float pixel = diff.at<float>(i, j);
            mse += pixel * pixel;}}

    mse /= (double)(origin.total());

    if (mse == 0) return 100;
    double psnr = 10.0 * (log(255 * 255 / mse) / log(10.0));
    return psnr;}

Mat compressAndReconstruct(const Mat& y_channel, const Mat& quantization_mat) {
    Mat result = Mat::zeros(y_channel.size(), CV_8UC1);
    for (int i = 0; i < y_channel.rows; i += 8) {
        for (int j = 0; j < y_channel.cols; j += 8) {
            Rect block_rect(j, i, 8, 8);
            Mat block = y_channel(block_rect);
            Mat block_dct, block_reconstructed;
            block.convertTo(block, CV_32F);
            dct(block, block);
            Mat quantization_mat_float;
            quantization_mat.convertTo(quantization_mat_float, CV_32F);
            for (int x = 0; x < 8; x++) {
                for (int y = 0; y < 8; y++) {
                    block.at<float>(x, y) /= quantization_mat_float.at<float>(x, y);
                }
            }
            block.convertTo(block, CV_32S);
            block.convertTo(block, CV_32F);
            block_reconstructed = Mat::zeros(block.size(), CV_32F);
            for (int x = 0; x < 8; x++) {
                for (int y = 0; y < 8; y++) {
                    block_reconstructed.at<float>(x, y) =
                        block.at<float>(x, y) * quantization_mat_float.at<float>(x, y);
                }
            }
            dct(block_reconstructed, block_reconstructed, DCT_INVERSE);
            block_reconstructed.convertTo(block_reconstructed, CV_8UC1);
            block_reconstructed.copyTo(result(block_rect));
        }
    }
    return result;
}

int main(int argc, char* argv[]) {
    Mat image = imread("lena.png");
    if (image.empty()) return -1;

    Mat image_YCbCr, YCbCr_channels[3], y;

    cvtColor(image, image_YCbCr, COLOR_BGR2YCrCb);
    split(image_YCbCr, YCbCr_channels);

    y = YCbCr_channels[0];

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

    Mat reconstructed1 = compressAndReconstruct(y, quantization_mat1);
    Mat reconstructed2 = compressAndReconstruct(y, quantization_mat2);
    Mat reconstructed3 = compressAndReconstruct(y, quantization_mat3);

    double psnr1 = calculatePSNR(y, reconstructed1);
    double psnr2 = calculatePSNR(y, reconstructed2);
    double psnr3 = calculatePSNR(y, reconstructed3);

    cout << "QM1: psnr = " << psnr1 << endl;
    cout << "QM2: psnr = " << psnr2 << endl;
    cout << "QM3: psnr =  " << psnr3 << endl;

    imshow("origin Y", y);

    imshow("QM1", reconstructed1);
    imshow("QM2", reconstructed2);
    imshow("QM3", reconstructed3);

    waitKey(0);

    return 0;}