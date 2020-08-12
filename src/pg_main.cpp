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

    // this will have to be adjusted based on where/how you are running the code... It should work for VS debugging
    std::string checker_file = "C:/Users/Javier/Documents/Projects/playground/images/checkerboard_10x10.png";

    if (argc > 1)
        checker_file = argv[1];

    std::cout << "Path to image " << checker_file << std::endl;

    // do work here
    try
    {
        cv::Mat checker_img = cv::imread(checker_file, cv::IMREAD_COLOR);
        
        std::string cv_window = "Checkerboard Image";
        cv::namedWindow(cv_window, cv::WINDOW_NORMAL | cv::WINDOW_KEEPRATIO);
        cv::imshow(cv_window, checker_img);
        cv::waitKey(0);
        
        // initialize parameters for cv::circle
        cv::Point point = cv::Point(600, 200);
        int raduis = 60;
        cv::Scalar color = cv::Scalar(255, 155, 140);
        int thickness = -1;
        
        // draw circle on top of image
        cv::circle(checker_img, point, 100, color, thickness);

        // create masks 
        cv::Mat mask, mask_inv;
        cv::inRange(checker_img, color, color, mask);
        cv::bitwise_not(mask, mask_inv);
        cv::divide(255, mask, mask, -1);
        cv::divide(255, mask_inv, mask_inv, -1);

        //cv::Mat out;
        //cv::boxFilter(mask, out, -1, cv::Size(4, 4), cv::Point(-1, -1) , true); type is unsigned int8

        // initialize parameters for cv::boxFilter
        cv::Mat filter_mask;
        int ddepth = -1;
        int kernel_size = 10;
        point = cv::Point(-1, -1);
        cv::Size ksize = cv::Size(3, 3);

        cv::Mat mask_filter, mask_inv_filter;
        cv::boxFilter(checker_img, mask_filter, ddepth, cv::Size(18, 18), point, true);
        cv::boxFilter(checker_img, mask_inv_filter, ddepth, cv::Size(5, 5), point, true);

        cv::Mat mat1, mat2;
        cv::copyTo(mask_filter, mat1, mask);
        cv::copyTo(mask_inv_filter, mat2, mask_inv);
        cv::Mat dst;
        cv::add(mat1, mat2, dst);

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

