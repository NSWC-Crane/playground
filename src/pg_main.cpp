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
#include <cv_blur_process.h>
#include <cv_random_image_gen.h>



void replace_pixels(cv::Mat src_img, cv::Mat src_blur, cv::Mat& dst, cv::Mat mask) 
{
    dst.create(src_blur.rows, src_img.cols, CV_8UC3);

    cv::Mat mask_3C;
    cv::cvtColor(mask, mask_3C, cv::COLOR_GRAY2BGR);  //3 channel mask

    cv::Mat prod1, prod2, mask_inv;
    cv::bitwise_not(mask_3C, mask_inv);
    cv::subtract(mask_inv, 254, mask_inv);

    cv::multiply(src_img, mask_inv, prod1);
    cv::multiply(src_blur, mask_3C, prod2);

    cv::add(prod1, prod2, dst);
}


// ----------------------------------------------------------------------------------------
int main(int argc, char** argv)
{
    cv::RNG rng(1234567);

    // this will have to be adjusted based on where/how you are running the code... It should work for VS debugging
    std::string checker_file = "../images/checkerboard_10x10.png"; 
    
    if (argc > 1) 
    {
        checker_file = argv[1];
    }

    std::cout << "Path to image " << checker_file << std::endl;

    // do work here
    try
    {
        cv::Mat checker_img = cv::imread(checker_file, cv::IMREAD_COLOR);

        cv::Mat dist;
        distortion(checker_img, dist, 0, checker_img.rows-1);

        cv::Mat img, mask;
        generate_random_overlay(cv::Size(600, 800), rng, 50, img, mask);

        // convert data type to 32 bit float
        checker_img.convertTo(checker_img, CV_32FC3, 1/255.0); // CV_32F values are from 0.0 to 1.0

        for (int i = 0; i < 8; i++) 
        {
            blur_layer(checker_img, rng, rng.uniform(0, 60));
        }
        

        std::string cv_window = "Final Image";
        cv::namedWindow(cv_window, cv::WINDOW_NORMAL | cv::WINDOW_KEEPRATIO);
        cv::imshow(cv_window, dist);
        cv::waitKey(0);

        // convert dst image to CV_8UC3
        cv::Mat dst_save;
        checker_img.convertTo(dst_save, CV_8UC3, 255); // use alpha parameter to scale 

        // save new image
        cv::imwrite("../images/checkerboard_blurred.jpg", dst_save);
    }
    catch(std::exception& e)
    {
        std::cout << "Error: " << e.what() << std::endl;
    }

    cv::destroyAllWindows();

    std::cout << "End of Program.  Press Enter to close..." << std::endl;
	std::cin.ignore();

}   // end of main

