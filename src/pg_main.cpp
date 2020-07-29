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
#include <file_ops.h>


// ----------------------------------------------------------------------------------------
int main(int argc, char** argv)
{
    cv::Mat dst;
    cv::Mat kernel;

    // initialize parameters for the filter
    int ddepth = -1;
    cv::Point anchor = cv::Point(-1,-1);
    double delta = 0;
    int kernel_size = 3;

    // define filters
    float outline[9] = { -1, -1, -1,
                         -1,  20, -1,
                         -1, -1, -1};

    float sharpen[9] = { 0, -1, 0,
                        -1, 5, -1,
                         0, -1, 0};


    // do work here
    try
    {
        // this will have to be adjusted based on where/how you are running the code... It should work for VS debugging
        std::string test_file = "C:/Users/Javier/Pictures/4ZSWD4L.jpg";

        cv::Mat cv_img = cv::imread(test_file, cv::IMREAD_COLOR);

        std::string cv_window = "Original Image";
        cv::namedWindow(cv_window, cv::WINDOW_NORMAL | cv::WINDOW_KEEPRATIO);
        cv::imshow(cv_window, cv_img);
        cv::waitKey(0);

        kernel = cv::Mat(kernel_size, kernel_size, CV_32F, outline);
        std::cout << std::endl << "kernel = " << std::endl << " " << kernel << std::endl << std::endl;

        cv_window = "Filtered Image";
        cv::filter2D(cv_img, dst, ddepth, kernel, anchor, delta , cv::BORDER_DEFAULT);
        cv::imshow(cv_window, dst);
        cv::waitKey(0);

        // save new image
        cv::imwrite("C:/Users/Javier/Pictures/4ZSWD4L_filter2d.jpg", dst);

    }
    catch(std::exception& e)
    {

        std::cout << "Error: " << e.what() << std::endl;
    }

    cv::destroyAllWindows();

    std::cout << "End of Program.  Press Enter to close..." << std::endl;
	std::cin.ignore();

}   // end of main

