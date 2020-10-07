#ifndef _CV_BARREL_DISTORTION_H_
#define _CV_BARREL_DISTORTION_H_

#include <cstdint>
#include <cmath>

// OpenCV includes
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/calib3d.hpp>



// https://www.mathworks.com/help/vision/ug/camera-calibration.html
// http://marcodiiga.github.io/radial-lens-undistortion-filtering

void distortion(cv::Mat src,
    cv::Mat& dst,
    int xc,
    int yc,
    float kx1,
    float kx2,
    float ky1,
    float ky2)
{
    dst = cv::Mat(src.rows, src.cols, CV_8U, cv::Scalar::all(255));

    int nr = src.rows;
    int nc = src.cols;

    float r;
    int xd, yd;
    float xn, yn, xd_f, yd_f;

    for (int x = 0; x < src.cols; x++)
    {
        for (int y = 0; y < src.rows; y++)
        {
            xn = (float)(2 * x - nc) / (float)nc;
            yn = (float)(2 * y - nr) / (float)nr;

            r = (x - xc) * (x - xc) + (y - yc) * (y - yc);
            r = sqrt(r);

            xd_f = xn * (1 + kx1 * r + kx2 * r * r);
            yd_f = yn * (1 + ky1 * r + ky2 * r * r);

            xd = (uint64_t)((xd_f + 1.0) * nc / 2);
            yd = (uint64_t)((yd_f + 1.0) * nr / 2);

            if (xd < src.cols && xd >= 0 && yd < src.rows && yd >= 0)
            {
                cv::Vec3b pixel = src.at<cv::Vec3b>(yd, xd);
                dst.at<uchar>(y, x) = pixel[0];
            }

        } // end of inner for loop
    } // end of for loop

} // end of distortion


void distortion(cv::Mat src,
    cv::Mat& dst)
{
    int nr = src.rows;
    int nc = src.cols;

    // center of distortion
    int xc = nc / 2;
    int yc = nr / 2;

    // distortion coefficients
    float k1 = 0.0008;
    float k2 = 0.0;

    distortion(src, dst, xc, yc, k1, k2, k1, k2);
}


cv::Mat img, dst;

struct cv_distortion_coeffs
{
    int xc = 0;
    int yc = 0;
    int kx1 = 0;
    int kx2 = 0;
    int ky1 = 0;
    int ky2 = 0;
};


void trackbar_callback(int, void* user_data)
{
    cv_distortion_coeffs params = *((cv_distortion_coeffs*)user_data);

    float kx1 = (float)((params.kx1 + 1) / 100000.0);
    float kx2 = (float)((params.kx2 + 1) / 1000000.0);
    float ky1 = (float)((params.ky1 + 1) / 100000.0);
    float ky2 = (float)((params.ky2 + 1) / 1000000.0);

    std::cout << "center of distortion (" << params.xc << ", " << params.yc << ")" << std::endl;
    std::cout << "distortion coefficients (kx1, ky1) (" << kx1 << ", " << ky1 << ")" <<
        " -- (kx2, ky2) (" << kx2 << ", " << ky2 << ")" << std::endl << std::endl;

    distortion(img, dst, (int)params.xc, (int)params.yc, kx1, kx2, ky1, ky2);
    cv::imshow("Distorted Image", dst);
}


void load_gui(std::string file_path)
{
    img = cv::imread(file_path, cv::IMREAD_COLOR);

    int coefficient_max = 100;
    int xc_max = img.cols;
    int yc_max = img.rows;

    cv_distortion_coeffs user_data;
    user_data.xc = img.cols / 2;
    user_data.yc = img.rows / 2;

    std::string parameter_window = "Parameter Options";
    cv::namedWindow(parameter_window, cv::WINDOW_AUTOSIZE); // Create Window

    // xc and yc trackbar
    std::string trackbar_name = "Xc - " + std::to_string(xc_max);
    cv::createTrackbar(trackbar_name, parameter_window, &user_data.xc, xc_max, trackbar_callback, &user_data);
    trackbar_name = "Yc - " + std::to_string(yc_max);
    cv::createTrackbar(trackbar_name, parameter_window, &user_data.yc, yc_max, trackbar_callback, &user_data);

    // kx1 and kx2 trackbar
    trackbar_name = "Kx1 - " + std::to_string(coefficient_max);
    cv::createTrackbar(trackbar_name, parameter_window, &user_data.kx1, coefficient_max, trackbar_callback, &user_data);
    trackbar_name = "Kx2 - " + std::to_string(coefficient_max);
    cv::createTrackbar(trackbar_name, parameter_window, &user_data.kx2, coefficient_max, trackbar_callback, &user_data);

    // ky1 and ky2 trackbar
    trackbar_name = "Ky1 - " + std::to_string(coefficient_max);
    cv::createTrackbar(trackbar_name, parameter_window, &user_data.ky1, coefficient_max, trackbar_callback, &user_data);
    trackbar_name = "Ky2 - " + std::to_string(coefficient_max);
    cv::createTrackbar(trackbar_name, parameter_window, &user_data.ky2, coefficient_max, trackbar_callback, &user_data);

    std::string main_window = "Distorted Image";
    cv::namedWindow(main_window, cv::WINDOW_AUTOSIZE); // Create Window
    trackbar_callback(0, &user_data); // initialize window
    cv::waitKey(0);

    // save new image
    cv::imwrite("../images/checkerboard_pincushion.jpg", dst);
}



#endif  // _CV_BARREL_DISTORTION_H_
