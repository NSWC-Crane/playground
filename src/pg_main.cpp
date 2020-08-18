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
    int raduis = rand() % 50 + 100;
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

        // clone blurred image and overlay shape
        cv::Mat checkboard_blur_copy = checkboard_blur.clone();
        cv::circle(checkboard_blur, point, raduis, color, thickness);


        
        // create mask 
        cv::Mat mask;
        cv::inRange(checkboard_blur, color, color, mask);
        
        // convert mask to CV_32F
        cv::Mat mask_blur, mask_inv_blur;
        mask.convertTo(mask_blur, CV_32FC1, 1 / 255.0);

        // apply 9x9 convolution
        cv::boxFilter(mask_blur, mask_blur, -1, cv::Size(9, 9), cv::Point(-1, -1), true);

        // create (1-mask)
        cv::Scalar ones_scalar = cv::Scalar(1, 1, 1);
        cv::subtract(ones_scalar, mask_blur, mask_inv_blur);

        // create 3 channel masks for background + foreground
        cv::Mat background_mask;
        cv::Mat in_bg[] = { mask_inv_blur, mask_inv_blur, mask_inv_blur };
        cv::merge(in_bg, 3, background_mask);

        cv::Mat foreground_mask;
        cv::Mat in_fg[] = { mask_blur, mask_blur, mask_blur };
        cv::merge(in_fg, 3, foreground_mask);


        // apply 9x9 convolution on checkboard img with overlayed shape
        cv::boxFilter(checkboard_blur, checkboard_blur, -1, cv::Size(9, 9), cv::Point(-1, -1), true);

        // 3C masks and images element wise multiplication
        cv::Mat background, foreground;
        cv::multiply(checkboard_blur_copy, background_mask, background);
        cv::multiply(checkboard_blur, foreground_mask, foreground);

        // display background (5x5 filter) and foreground (9x9 filter) 
        std::string cv_window = "Background";
        cv::namedWindow(cv_window, cv::WINDOW_NORMAL | cv::WINDOW_KEEPRATIO);
        cv::imshow(cv_window, background);
        cv::waitKey(0);

        cv_window = "Foreground";
        cv::namedWindow(cv_window, cv::WINDOW_NORMAL | cv::WINDOW_KEEPRATIO);
        cv::imshow(cv_window, foreground);
        cv::waitKey(0);

        // add both background and foreground
        cv::Mat dst;
        cv::add(background, foreground, dst);

        // display combined image
        cv_window = "Background + Foreground";
        cv::namedWindow(cv_window, cv::WINDOW_NORMAL | cv::WINDOW_KEEPRATIO);
        cv::imshow(cv_window, dst);
        cv::waitKey(0);

        // convert dst image to CV_8UC3
        cv::multiply(dst, cv::Scalar(1, 1, 1), dst, 255);
        dst.convertTo(dst, CV_8UC3); // use alpha parameter to scale 

        // save new image
        cv::imwrite("C:/Users/Javier/Documents/Projects/playground/images/checkerboard_blurred.jpg", dst);
    }
    catch(std::exception& e)
    {
        std::cout << "Error: " << e.what() << std::endl;
    }

    cv::destroyAllWindows();

    std::cout << "End of Program.  Press Enter to close..." << std::endl;
	std::cin.ignore();

}   // end of main

