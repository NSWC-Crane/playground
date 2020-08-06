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
    cv::Mat src_img;
    cv::Mat src_gray;
    cv::Mat src_blur;
    cv::Mat mask;
    cv::Mat mask_inv;


    // do work here
    try
    {
        // this will have to be adjusted based on where/how you are running the code... It should work for VS debugging
        std::string test_file = "C:/Users/Javier/Documents/Projects/playground/images/4ZSWD4L.jpg";
        if (argc > 1) 
            test_file = argv[1];
        
        std::cout << "Path to image " << test_file << std::endl;
        src_img = cv::imread(test_file, cv::IMREAD_COLOR);
        
        
        std::string cv_window = "Original Image";
        cv::namedWindow(cv_window, cv::WINDOW_NORMAL | cv::WINDOW_KEEPRATIO);
        cv::imshow(cv_window, src_img);
        cv::waitKey(0);


        cv::Size ksize = cv::Size(kernel_size, kernel_size);
        bool normalize = true;

        cv_window = "Blurred Image";
        cv::boxFilter(src_img, src_blur, ddepth, ksize, anchor, normalize, cv::BORDER_DEFAULT);
        cv::imshow(cv_window, src_blur);
        cv::waitKey(0);


        // CREATE MASKS //
        cv_window = "Mask Image";
        cv::cvtColor(src_img, src_gray, cv::COLOR_BGR2GRAY);
        cv::threshold(src_gray, mask, threshold, max_binary_value, cv::THRESH_BINARY);
        cv::imshow(cv_window, mask);
        cv::waitKey(0);

        cv_window = "Mask Inverted Image";
        cv::bitwise_not(mask, mask_inv);
        cv::imshow(cv_window, mask_inv);
        cv::waitKey(0);

        // CREATE MASKED IMAGES // 
        cv::Mat original_img_mask_0;
        cv_window = "Original Imaged Where Mask is 0";
        cv::copyTo(src_img, original_img_mask_0, mask_inv);
        cv::imshow(cv_window, original_img_mask_0);
        cv::waitKey(0);

        cv::Mat blurred_img_mask_1;
        cv_window = "Blurred Imaged Where Mask is 1";
        cv::copyTo(src_blur, blurred_img_mask_1, mask);
        cv::imshow(cv_window, blurred_img_mask_1);
        cv::waitKey(0);

        // COMBINE BOTH MASKED IMAGES ///
        cv::Mat result_img;
        cv_window = "Original and Blurred Image Combined!!!!";
        cv::add(original_img_mask_0, blurred_img_mask_1, result_img);
        cv::imshow(cv_window, result_img);
        cv::waitKey(0);

        // DISPLAY BOTH SOURCE AND RESULTING IMAGE BY CONCAT. //
        cv::Mat concat_imgs;
        cv::hconcat(src_img, result_img, concat_imgs);
        cv::imshow("You already know", concat_imgs);
        cv::waitKey(0);

        //std::cout << "mask: " << mask.rows << " " << mask.cols << std::endl;
        //std::cout << "source: " << src_img.rows << " " << src_img.cols << std::endl;

        uint8_t* blurred_pixel_ptr = (uint8_t*)src_blur.data;
        uint8_t* pixelPtr = (uint8_t*)src_img.data;
        int cn = src_img.channels();
        cv::Scalar_<uint8_t> bgrPixel;

        for (int i = 0; i < src_img.rows; i++)
        {
            for (int j = 0; j < src_img.cols; j++)
            {
                //bgrPixel.val[0] = pixelPtr[i * src_img.cols * cn + j * cn + 0]; // B
                //bgrPixel.val[1] = pixelPtr[i * src_img.cols * cn + j * cn + 1]; // G
                //bgrPixel.val[2] = pixelPtr[i * src_img.cols * cn + j * cn + 2]; // R

                // do something with BGR values...

                if ((int)mask.at<uchar>(i, j) == 255) {
                    pixelPtr[i * src_img.cols * cn + j * cn + 0] = blurred_pixel_ptr[i * src_img.cols * cn + j * cn + 0];
                    pixelPtr[i * src_img.cols * cn + j * cn + 1] = blurred_pixel_ptr[i * src_img.cols * cn + j * cn + 1];
                    pixelPtr[i * src_img.cols * cn + j * cn + 2] = blurred_pixel_ptr[i * src_img.cols * cn + j * cn + 2];
                }
            }
        }

        cv_window = "Original Image";
        cv::namedWindow(cv_window, cv::WINDOW_NORMAL | cv::WINDOW_KEEPRATIO);
        cv::imshow(cv_window, src_img);
        cv::waitKey(0);

        cv::Mat both_methods;
        cv::hconcat(result_img, src_img, both_methods);
        cv::imshow("Comparing both methods!", both_methods);
        cv::waitKey(0);


        // save new images
        //cv::imwrite("C:/Users/Javier/Documents/Projects/playground/images/mask.jpg", mask);
        //cv::imwrite("C:/Users/Javier/Documents/Projects/playground/images/mask_inv.jpg", mask_inv);
        //cv::imwrite("C:/Users/Javier/Documents/Projects/playground/images/4ZSWD4L_blur.jpg", src_blur);
        //cv::imwrite("C:/Users/Javier/Documents/Projects/playground/images/4ZSWD4L_masked.jpg", masked_blurred);
        //cv::imwrite("C:/Users/Javier/Documents/Projects/playground/images/4ZSWD4L_masked_inv.jpg", masked_org);
    }
    catch(std::exception& e)
    {

        std::cout << "Error: " << e.what() << std::endl;
    }

    cv::destroyAllWindows();

    std::cout << "End of Program.  Press Enter to close..." << std::endl;
	std::cin.ignore();

}   // end of main

