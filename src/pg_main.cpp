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

// initialize parameters for filter2D
int const kernel_size = 5;
double const delta = 0;
int const ddepth = -1;
cv::Point anchor = cv::Point(-1, -1);



void replace_method(cv::Mat src_img, cv::Mat src_blur, cv::Mat &dst, cv::Mat mask)
{
    std::cout << "Inside replace_method" << std::endl;

    dst.create(src_blur.rows, src_img.cols, CV_8UC3);

    uint8_t* blurred_pixel_ptr = (uint8_t*)src_blur.data;
    uint8_t* source_pixel_ptr = (uint8_t*)src_img.data;
    uint8_t* pixelPtr = (uint8_t*)dst.data;
    int cn = src_img.channels();

    for (int i = 0; i < src_img.rows; i++)
    {
        for (int j = 0; j < src_img.cols; j++)
        {
            if ((int)mask.at<uchar>(i, j) == 1) {
                pixelPtr[i * src_img.cols * cn + j * cn + 0] = blurred_pixel_ptr[i * src_img.cols * cn + j * cn + 0];
                pixelPtr[i * src_img.cols * cn + j * cn + 1] = blurred_pixel_ptr[i * src_img.cols * cn + j * cn + 1];
                pixelPtr[i * src_img.cols * cn + j * cn + 2] = blurred_pixel_ptr[i * src_img.cols * cn + j * cn + 2];
            } 
            else 
            {
                pixelPtr[i * src_img.cols * cn + j * cn + 0] = source_pixel_ptr[i * src_img.cols * cn + j * cn + 0];
                pixelPtr[i * src_img.cols * cn + j * cn + 1] = source_pixel_ptr[i * src_img.cols * cn + j * cn + 1];
                pixelPtr[i * src_img.cols * cn + j * cn + 2] = source_pixel_ptr[i * src_img.cols * cn + j * cn + 2];
            }
        }
    }
}


void copy_method(cv::Mat src_img, cv::Mat src_blur, cv::Mat &dst, cv::Mat mask) 
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

void matrix_mult_method(cv::Mat &src_img, cv::Mat &src_blur, cv::Mat &dst, cv::Mat mask) 
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
    cv::Mat src_img;
    cv::Mat src_gray;
    cv::Mat src_blur;
    cv::Mat mask;
    cv::Mat mask_inv;

    // this will have to be adjusted based on where/how you are running the code... It should work for VS debugging
    std::string test_file = "C:/Users/Javier/Documents/Projects/playground/images/4ZSWD4L.jpg";

    if (argc > 1)
        test_file = argv[1];

    std::cout << "Path to image " << test_file << std::endl;

    // do work here
    try
    {
        src_img = cv::imread(test_file, cv::IMREAD_COLOR);
        
        std::string cv_window = "Original Image";
        cv::namedWindow(cv_window, cv::WINDOW_NORMAL | cv::WINDOW_KEEPRATIO);
        cv::imshow(cv_window, src_img);
        cv::waitKey(0);

        
        // blurred image
        cv::Size ksize = cv::Size(kernel_size, kernel_size);
        bool normalize = true;
        cv::boxFilter(src_img, src_blur, ddepth, ksize, anchor, normalize, cv::BORDER_DEFAULT);
        
        // creating mask with values between [0, 1]
        cv::cvtColor(src_img, src_gray, cv::COLOR_BGR2GRAY);
        cv::threshold(src_gray, mask, threshold, max_binary_value, cv::THRESH_BINARY);
             

        cv::Mat dst;
        matrix_mult_method(src_img, src_blur, dst, mask);

        cv_window = "New Image";
        cv::imshow(cv_window, dst);
        cv::waitKey(0);

        cv::imwrite("C:/Users/Javier/Documents/Projects/playground/images/4ZSWD4L_NEW.jpg", dst);
    }
    catch(std::exception& e)
    {

        std::cout << "Error: " << e.what() << std::endl;
    }

    cv::destroyAllWindows();

    std::cout << "End of Program.  Press Enter to close..." << std::endl;
	std::cin.ignore();

}   // end of main

