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
#include "Header1.h"



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
    cv::RNG rng(1234567);

    // this will have to be adjusted based on where/how you are running the code... It should work for VS debugging
    std::string checker_file = "C:/Users/Javier/Documents/Projects/playground/images/checkerboard_10x10.png";

    if (argc > 1) 
    {
        checker_file = argv[1];
    }

    std::cout << "Path to image " << checker_file << std::endl;

    // do work here
    try
    {
        cv::Mat checker_img = cv::imread(checker_file, cv::IMREAD_COLOR);

        // convert data type to 32 bit float
        cv::Mat checkboard_img(600, 800, CV_32FC3);
        checker_img.convertTo(checkboard_img, CV_32FC3, 1/255.0); // CV_32F are from 0.0 to 1.0

        const int len_data = 8;
        double kernel_params[] = { 5, 25, 30, 8, 
                                   5, 25, 30, 8 };


        for (int i = 0; i < len_data; i++) {
            blur_layer(checkboard_img, rng, kernel_params[i]);
        }

        
        std::string cv_window = "Final Image";
        cv::namedWindow(cv_window, cv::WINDOW_NORMAL | cv::WINDOW_KEEPRATIO);
        cv::imshow(cv_window, checkboard_img);
        cv::waitKey(0);

        // convert dst image to CV_8UC3
        cv::Mat dst_save;
        checkboard_img.convertTo(dst_save, CV_8UC3, 255); // use alpha parameter to scale 

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

