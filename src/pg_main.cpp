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
int const max_binary_value = 255;

// initialize parameters for filter2D
int const kernel_size = 5;
double const delta = 0;
int const ddepth = -1;
cv::Point anchor = cv::Point(-1, -1);


// ----------------------------------------------------------------------------------------
int main(int argc, char** argv)
{
    cv::Mat blurred_img;
    cv::Mat mask_img;
    cv::Mat src_img;
    cv::Mat src_gray;


    // do work here
    try
    {
        // this will have to be adjusted based on where/how you are running the code... It should work for VS debugging
        std::string test_file = "C:/Users/Javier/Documents/Projects/playground/images/4ZSWD4L.jpg";

        // read image 
        src_img = cv::imread(test_file, cv::IMREAD_COLOR);
        
        std::string cv_window = "Original Image";
        cv::namedWindow(cv_window, cv::WINDOW_NORMAL | cv::WINDOW_KEEPRATIO);
        cv::imshow(cv_window, src_img);
        cv::waitKey(0);


        cv_window = "Mask Image";
        cv::cvtColor(src_img, src_gray, cv::COLOR_BGR2GRAY);
        cv::threshold(src_gray, mask_img, threshold, max_binary_value, cv::THRESH_BINARY);
        cv::imshow(cv_window, mask_img);
        cv::waitKey(0);


        // define blur filter
        cv::Mat kernel = cv::Mat::ones(kernel_size, kernel_size, CV_32F) / (float)(kernel_size * kernel_size);
        std::cout << std::endl << "kernel = " << std::endl << " " << kernel << std::endl << std::endl;

        cv_window = "Blurred Image";
        cv::filter2D(src_img, blurred_img, ddepth, kernel, anchor, delta , cv::BORDER_DEFAULT);
        cv::imshow(cv_window, blurred_img);
        cv::waitKey(0);


        // save new images
        cv::imwrite("C:/Users/Javier/Documents/Projects/playground/images/4ZSWD4L_blur.jpg", blurred_img);
        cv::imwrite("C:/Users/Javier/Documents/Projects/playground/images/4ZSWD4L_mask.jpg", mask_img);
    }
    catch(std::exception& e)
    {

        std::cout << "Error: " << e.what() << std::endl;
    }

    cv::destroyAllWindows();

    std::cout << "End of Program.  Press Enter to close..." << std::endl;
	std::cin.ignore();

}   // end of main

