#define _CRT_SECURE_NO_WARNINGS

#if defined(_WIN32) | defined(__WIN32__) | defined(__WIN32) | defined(_WIN64) | defined(__WIN64)
#include <windows.h>
//#include "win_network_fcns.h"
//#include <winsock2.h>
//#include <iphlpapi.h>
//
//#pragma comment(lib, "IPHLPAPI.lib")    // Link with Iphlpapi.lib

#endif


// C/C++ includes
#include <cmath>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <fstream>
#include <iomanip>
#include <chrono>
#include <string>
#include <vector>
#include <array>
#include <algorithm>
#include <type_traits>
#include <list>


// OpenCV includes
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/video.hpp>
#include <opencv2/imgcodecs.hpp>

// custom includes


// global variables //
int const threshold = 120;
int const max_binary_value = 1;



void replace_method(cv::Mat src_img, cv::Mat src_blur, cv::Mat &dst, cv::Mat mask) 
{
    std::cout << "Inside mask_method" << std::endl;
    
    dst.create(src_blur.rows, src_img.cols, CV_8UC3);
    
    cv::Mat mask_inv;
    cv::bitwise_not(mask, mask_inv);
    cv::subtract(mask_inv, 254, mask_inv);
    
    // masked images
    cv::Mat original_img_mask_0;
    cv::copyTo(src_img, original_img_mask_0, mask_inv);
    
    cv::Mat blurred_img_mask_1;
    cv::copyTo(src_blur, blurred_img_mask_1, mask);

    // combine both masked images
    cv::add(original_img_mask_0, blurred_img_mask_1, dst);
}

void matrix_mult_method(cv::Mat src_img, cv::Mat src_blur, cv::Mat &dst, cv::Mat mask) 
{
    std::cout << "Inside linear_alg_method" << std::endl;

    cv::Mat prod1, prod2, mask_inv;
    dst.create(src_blur.rows, src_img.cols, CV_8UC3);

    cv::Mat mask_3C;
    cv::cvtColor(mask, mask_3C, cv::COLOR_GRAY2BGR);  //3 channel mask

    cv::bitwise_not(mask_3C, mask_inv);
    cv::subtract(mask_inv, 254, mask_inv);

    cv::multiply(src_img, mask_inv, prod1);
    cv::multiply(src_blur, mask_3C, prod2);

    cv::add(prod1, prod2, dst);
}

// ----------------------------------------------------------------------------------------
int main(int argc, char** argv)
{

    // initialize parameters for cv::circle
    cv::Point point = cv::Point(600, 200);
    int raduis = rand() % 50 + 55;
    cv::Scalar color = cv::Scalar(0.45, 0.65, 0.34);
    int thickness = -1;

    // this will have to be adjusted based on where/how you are running the code... It should work for VS debugging
    std::string checker_file = "C:/Users/Javier/Documents/Projects/playground/images/checkerboard_10x10.png";

    if (argc > 1)
        checker_file = argv[1];

    std::cout << "Path to image " << checker_file << std::endl;

    // do work here
    try
    {
        cv::Mat checker_img = cv::imread(checker_file, cv::IMREAD_COLOR);

        // convert data type to 16 bit float
        cv::Mat checkboard_img(600, 800, CV_32FC3);
        checker_img.convertTo(checkboard_img, CV_32FC3, 1/255.0); // CV_32F are from 0.0 to 1.0

        // apply 5x5 convolution
        cv::Mat checkboard_blur;
        cv::boxFilter(checkboard_img, checkboard_blur, -1, cv::Size(5, 5), cv::Point(-1, -1), true); 

        // clone blurred image 
        cv::Mat checkboard_blur_copy = checkboard_blur.clone();


        cv::Scalar colors[] = { cv::Scalar(0.45, 0.65, 0.34), cv::Scalar(0.32, 0.13, 0.69), cv::Scalar(0.79, 0.48, 0.25) };
        cv::Size filter_dimensions[] = { cv::Size(5, 5), cv::Size(7, 7), cv::Size(3, 3) };
        cv::Point center[] = { cv::Point(600, 200), cv::Point(300, 500), cv::Point(0, 450) };

        cv::Mat masks[3];
        cv::Mat blurred_imgs[3];

        cv::Mat tmp, mask_blurred;
        for (int i = 0; i < 3; i++) {
            
            cv::circle(checkboard_blur, center[i], raduis, colors[i], thickness);

            cv::inRange(checkboard_blur, colors[i], colors[i], masks[i]);

            cv::boxFilter(checkboard_blur, tmp, -1, filter_dimensions[i], cv::Point(-1, -1), true);

            // convert to CV_32FC1
            masks[i].convertTo(mask_blurred, CV_32FC1, 1 / 255.0);
            
            cv::boxFilter(mask_blurred, mask_blurred, -1, filter_dimensions[i], cv::Point(-1, -1), true);

            // merge for 3 channels
            cv::Mat in_bg[] = { mask_blurred,mask_blurred, mask_blurred };
            cv::merge(in_bg, 3, masks[i]);

            cv::multiply(tmp, masks[i], blurred_imgs[i]);
        }


        cv::Mat foreground_mask(600, 800, CV_32FC3, cv::Scalar(0,0,0));
        for (int i = 0; i < 3; i++) {
            cv::add(foreground_mask, masks[i], foreground_mask);
        }

        cv::Mat background_mask, background;
        cv::subtract(cv::Scalar(1, 1, 1), foreground_mask, background_mask);
        cv::multiply(background_mask, checkboard_blur_copy, background);


        cv::Mat foreground(600, 800, CV_32FC3, cv::Scalar(0, 0, 0));
        for (int i = 0; i < 3; i++) {
            cv::add(foreground, blurred_imgs[i], foreground);
        }


        cv::Mat dst;
        cv::add(background, foreground, dst);


        // display background and foreground 
        std::string cv_window = "Background";
        cv::namedWindow(cv_window, cv::WINDOW_NORMAL | cv::WINDOW_KEEPRATIO);
        cv::imshow(cv_window, background);
        cv::waitKey(0);

        cv_window = "Foreground";
        cv::namedWindow(cv_window, cv::WINDOW_NORMAL | cv::WINDOW_KEEPRATIO);
        cv::imshow(cv_window, foreground);
        cv::waitKey(0);

        // display combined image
        cv_window = "Background + Foreground";
        cv::namedWindow(cv_window, cv::WINDOW_NORMAL | cv::WINDOW_KEEPRATIO);
        cv::imshow(cv_window, dst);
        cv::waitKey(0);

        // convert dst image to CV_8UC3
        cv::Mat dst_save;
        dst.convertTo(dst_save, CV_8UC3, 255); // use alpha parameter to scale 

        // save new image
        cv::imwrite("C:/Users/Javier/Documents/Projects/playground/images/checkerboard_blurred.jpg", dst_save);
    }
    catch(std::exception& e)
    {
        std::cout << "Error: " << e.what() << std::endl;
    }

    cv::destroyAllWindows();

    std::cout << "End of Program.  Press Enter to close..." << std::endl;
	std::cin.ignore();

}   // end of main

