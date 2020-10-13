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
#include <set>

// OpenCV includes
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/video.hpp>
#include <opencv2/imgcodecs.hpp>

// custom includes
#include <cv_blur_process.h>
#include <cv_random_image_gen.h>
#include <cv_create_gaussian_kernel.h>
#include <blur_params.h>

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
    uint32_t img_h = 500;
    uint32_t img_w = 500;
    cv::Size img_size(img_h, img_w);

    cv::RNG rng(1234567);
    
    // do work here
    try
    {
        // generate and sort dm_values
        std::vector<uint16_t> dm_values;
        genrate_depthmap_set(min_dm_value, max_dm_value, dm_values, rng);

        // generate random image
        cv::Mat random_img;
        generate_random_image(random_img, rng, img_h, img_w, 40, 1.0);
        random_img.convertTo(random_img, CV_32FC3, (1/255.0));

        // create gaussian kernel 
        cv::Mat kernel;
        create_gaussian_kernel(kernel_size, sigma_table[br1_table[dm_values[0]]], kernel);

        // blur image - filter2D()
        cv::filter2D(random_img, random_img, -1, kernel, cv::Point(-1, -1), 0.0, cv::BorderTypes::BORDER_REPLICATE);

        // generate base depth map
        cv::Mat depth_map(img_h, img_w, CV_8UC1, cv::Scalar::all(dm_values[0]));

        for (int idx = 1; idx < dm_values.size(); ++idx)
        {
            // generate N
            int min_N = ceil((max_dm_value) / (1 + exp(-0.2 * dm_values[idx] + (0.1 * max_dm_value))) + 3);
            int max_N = ceil(1.25 * min_N);
            int N = rng.uniform(min_N, max_N + 1);

            // generate random overlay
            cv::Mat output_img, mask;
            generate_random_overlay(img_size, rng, N, output_img, mask);

            // overlay depthmap
            overlay_depthmap(depth_map, mask, dm_values[idx]);

            // generate kernel
            create_gaussian_kernel(kernel_size, sigma_table[br1_table[dm_values[idx]]], kernel);

            // blur layer
            blur_layer(random_img, output_img, mask, kernel, rng, 3);
        }

        // save blurred image 
        random_img.convertTo(random_img, CV_8UC3, 255);
        cv::imwrite("../images/blurred_img.png", random_img);
        // save depthmap image
        cv::imwrite("../images/depthmap_grayscale.png", depth_map);
    }
    catch(std::exception& e)
    {
        std::cout << "Error: " << e.what() << std::endl;
    }

    cv::destroyAllWindows();

    std::cout << "End of Program.  Press Enter to close..." << std::endl;
	std::cin.ignore();

}   // end of main

