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
#include <num2string.h>
#include <file_ops.h>


// ----------------------------------------------------------------------------------------
int main(int argc, char** argv)
{
    uint32_t img_h = 500;
    uint32_t img_w = 500;
    cv::Size img_size(img_h, img_w);

    cv::RNG rng(1234567);

    cv::Mat img_f1, img_f2;
    cv::Mat kernel;
    cv::Mat output_img, mask;
    std::vector<uint16_t> dm_values;
    int min_N, max_N, N;
    double scale = 0.1;

    std::vector<uint8_t> depthmap_values;
    std::vector<double> sigma_table;
    std::vector<uint8_t> br1_table;
    std::vector<uint8_t> br2_table;
    uint8_t dataset_type;
    uint32_t max_dm_num;
    uint32_t num_objects;
    uint32_t num_images;
    std::string save_location;

    if (argc == 1)
    {
        std::cout << "Error: Missing confige file" << std::endl;
        std::cout << "Usage: ./pg <confige_file.txt>" << std::endl;
        std::cout << std::endl;
        std::cin.ignore();
        return 0;
    }

    std::string param_filename = argv[1];
    read_blur_params(param_filename, depthmap_values, sigma_table, br1_table, br2_table,
        dataset_type, max_dm_num, num_objects, num_images, save_location);

    uint16_t min_dm_value = depthmap_values.front();
    uint16_t max_dm_value = depthmap_values.back();

    // do work here
    try
    {
        std::ofstream DataLog_Stream("../input_file_m.txt", std::ofstream::out);
        DataLog_Stream << "# Data Directory" << std::endl;
        DataLog_Stream << save_location << std::endl;
        DataLog_Stream << std::endl;
        DataLog_Stream << "# focus point 1 filename, focus point 2 filename, depthmap filename" << std::endl;
        
        for (uint32_t i = 0; i<num_images; ++i)
        {
            // generate dm_values
            genrate_depthmap_set(min_dm_value, max_dm_value, max_dm_num, depthmap_values, dm_values, rng);
            
            generate_random_image(img_f1, rng, img_h, img_w, 800, 0.1);
            img_f2 = img_f1.clone();

            // create gaussian kernel and blur imgs
            create_gaussian_kernel(kernel_size, sigma_table[br1_table[dm_values[0]]], kernel);
            cv::filter2D(img_f1, img_f1, -1, kernel, cv::Point(-1, -1), 0.0, cv::BorderTypes::BORDER_REPLICATE);

            create_gaussian_kernel(kernel_size, sigma_table[br2_table[dm_values[0]]], kernel);
            cv::filter2D(img_f2, img_f2, -1, kernel, cv::Point(-1, -1), 0.0, cv::BorderTypes::BORDER_REPLICATE);

            // create depth map
            cv::Mat depth_map(img_h, img_w, CV_8UC1, cv::Scalar::all(dm_values[0]));

            // blur imgs using dm_values and random masks
            for (uint32_t idx = 1; idx<dm_values.size(); ++idx)
            {
                min_N = ceil((max_dm_value) / (1 + exp(-0.2 * dm_values[idx] + (0.1 * max_dm_value))) + 3);
                max_N = ceil(1.25 * min_N);
                N = rng.uniform(min_N, max_N + 1);

                // define the scale factor
                scale = 80.0 / (double)img_size.width;

                // generate random overlay
                generate_random_overlay(img_size, rng, N, output_img, mask, scale);

                // overlay depthmap
                overlay_depthmap(depth_map, mask, dm_values[idx]);

                // blur f1
                create_gaussian_kernel(kernel_size, sigma_table[br1_table[dm_values[idx]]], kernel);
                blur_layer(img_f1, output_img, mask, kernel, rng, 3);

                // blur f2
                create_gaussian_kernel(kernel_size, sigma_table[br2_table[dm_values[idx]]], kernel);
                blur_layer(img_f2, output_img, mask, kernel, rng, 3);
            }

            std::string f1_filename = save_location + num2str<int>(i, "image_f1_%04i.png");
            std::string f2_filename = save_location + num2str<int>(i, "image_f2_%04i.png");
            std::string dmap_filename = save_location + num2str<int>(i, "dm_%04i.png");

            cv::imwrite(f1_filename, img_f1);
            cv::imwrite(f2_filename, img_f2);
            cv::imwrite(dmap_filename, depth_map);

            DataLog_Stream << f1_filename << ", " << f2_filename << ", " << dmap_filename << std::endl;
        } // end of for loop

        DataLog_Stream.close();
    }
    catch(std::exception& e)
    {
        std::cout << "Error: " << e.what() << std::endl;
    }

    cv::destroyAllWindows();

    std::cout << "End of Program.  Press Enter to close..." << std::endl;
	std::cin.ignore();

}   // end of main

