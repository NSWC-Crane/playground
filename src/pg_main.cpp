#define _CRT_SECURE_NO_WARNINGS

#if defined(_WIN32) | defined(__WIN32__) | defined(__WIN32) | defined(_WIN64) | defined(__WIN64)
#include <windows.h>

#else
#include <dlfcn.h>
typedef void* HINSTANCE;

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
inline std::ostream& operator<<(std::ostream& out, std::vector<uint8_t>& item)
{
    for (uint64_t idx = 0; idx < item.size() - 1; ++idx)
    {
        out << static_cast<uint32_t>(item[idx]) << ",";
    }
    out << static_cast<uint32_t>(item[item.size() - 1]);
    return out;
}

// ----------------------------------------------------------------------------------------
template <typename T>
inline std::ostream& operator<<(std::ostream& out, std::vector<T>& item)
{
    for (uint64_t idx = 0; idx < item.size() - 1; ++idx)
    {
        out << item[idx] << ",";
    }
    out << item[item.size() - 1];
    return out;
}

// ----------------------------------------------------------------------------------------
int main(int argc, char** argv)
{
    uint32_t idx = 0, jdx = 0;
    uint32_t img_h = 480;
    uint32_t img_w = 480;
    cv::Size img_size(img_h, img_w);

    cv::RNG rng(time(NULL));

    cv::Mat img_f1, img_f2;
    cv::Mat kernel;
    cv::Mat output_img, mask;
    cv::Mat f1_layer, f2_layer;
    cv::Mat random_img;
    cv::Mat montage;
    //std::vector<uint16_t> dm_values;
    std::vector<uint16_t> dm_indexes;
    int32_t min_N, max_N;
    uint32_t N;
    double scale = 0.1;
    uint32_t BN = 1000;
    std::string window_name = "montage";

    std::vector<uint8_t> depthmap_values;
    std::vector<double> sigma_table;
    std::vector<uint8_t> br1_table;
    std::vector<uint8_t> br2_table;
    uint8_t dataset_type;
    uint32_t max_dm_num;
    uint32_t num_objects;
    uint32_t num_images;
    std::string save_location;
    std::string lib_filename;

    uint32_t octaves = 7;
    double sn_scale = 0.020;
    double sn_slope = 1.0; 
    double sn_int = 0.01;
    double persistence = 0.70;
    //std::vector<cv::Vec3b> wood = { cv::Vec3b(41,44,35), cv::Vec3b(57,91,61), cv::Vec3b(80,114,113), cv::Vec3b(64,126,132) };
    std::vector<uint8_t> wood = { 41,44,35, 57,91,61, 80,114,113, 64,126,132 };

    // use these variables for the datatype > 0
    typedef void (*init_)(long seed);
    typedef unsigned int (*evaluate_)(double x, double y, double scale, unsigned int num);
    typedef unsigned int (*octave_evaluate_)(double x, double y, double scale, unsigned int octaves, double persistence);
    typedef void (*create_color_map_)(unsigned int h, unsigned int w, double scale, unsigned int octaves, double persistence, unsigned char* color, unsigned char* map);
    HINSTANCE simplex_noise_lib = NULL;
    init_ init;
    evaluate_ evaluate;
    octave_evaluate_ octave_evaluate;
    create_color_map_ create_color_map;

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

    // create results directories if they do not exist
    mkdir(save_location + "images");
    mkdir(save_location + "depth_maps");

    // save the parameters that were used to generate the dataset
    std::ofstream param_stream(save_location + "parameters.txt", std::ofstream::out);
    param_stream << "# Parameters used to generate the dataset" << std::endl;
    param_stream << depthmap_values << std::endl;
    param_stream << sigma_table << std::endl;
    param_stream << br1_table << std::endl;
    param_stream << br2_table << std::endl;
    param_stream << static_cast<uint32_t>(dataset_type) << std::endl;
    param_stream << max_dm_num << std::endl;
    param_stream << num_objects << std::endl;
    param_stream << num_images << std::endl;
    param_stream.close();

    // print out the parameters
    std::cout << "------------------------------------------------------------------" << std::endl;
    std::cout << "Parameters used to generate the dataset" << std::endl;
    std::cout << "Depthmap Values: " << depthmap_values << std::endl;
    std::cout << "Sigma Table:     " << sigma_table << std::endl;
    std::cout << "Blur Radius 1:   " << br1_table << std::endl;
    std::cout << "Blur Radius 2:   " << br2_table << std::endl;
    std::cout << "Dataset Type:    " << static_cast<uint32_t>(dataset_type) << std::endl;
    std::cout << "DM Values/Image: " << max_dm_num << std::endl;
    std::cout << "# of Objects:    " << num_objects << std::endl;
    std::cout << "# of Images:     " << num_images << std::endl;
    std::cout << "------------------------------------------------------------------" << std::endl << std::endl;

    // setup the windows to display the results
    cv::namedWindow(window_name, cv::WINDOW_NORMAL);
    cv::resizeWindow(window_name, 2*img_w, img_h);

    // do work here
    try
    {    
        if (dataset_type == 1)
        {
            lib_filename = "D:/Projects/simplex_noise/build/Release/sn_lib.dll";

#if defined(_WIN32) | defined(__WIN32__) | defined(__WIN32) | defined(_WIN64) | defined(__WIN64)

            simplex_noise_lib = LoadLibrary(lib_filename.c_str());

            if (simplex_noise_lib == NULL)
            {
                throw std::runtime_error("error loading library");
            }

            init = (init_)GetProcAddress(simplex_noise_lib, "init");
            evaluate = (evaluate_)GetProcAddress(simplex_noise_lib, "evaluate");
            octave_evaluate = (octave_evaluate_)GetProcAddress(simplex_noise_lib, "octave_evaluate");
            create_color_map = (create_color_map_)GetProcAddress(simplex_noise_lib, "create_color_map");
#else
            simplex_noise_lib = dlopen(lib_filename.c_str(), RTLD_NOW);

            if (simplex_noise_lib == NULL)
            {
                throw std::runtime_error("error loading library");
            }

            init = (init_)dlsym(simplex_noise_lib, "init");
            evaluate = (evaluate_)dlsym(simplex_noise_lib, "evaluate");
            octave_evaluate = (octave_evaluate_)dlsym(simplex_noise_lib, "octave_evaluate");
            create_color_map = (create_color_map_)dlsym(simplex_noise_lib, "create_color_map");
#endif

            init((long)time(NULL));
            sn_int = 0.01;
            sn_slope = (0.1234 - sn_int) / (double)(max_dm_value - min_dm_value);
        }

        std::ofstream DataLog_Stream(save_location + "input_file.txt", std::ofstream::out);
        DataLog_Stream << "# Data Directory" << std::endl;
        DataLog_Stream << save_location << std::endl;
        DataLog_Stream << std::endl;
        DataLog_Stream << "# focus point 1 filename, focus point 2 filename, depthmap filename" << std::endl;
        
        std::cout << "Data Directory: " << save_location << std::endl;

        for (jdx = 0; jdx < num_images; ++jdx)
        {
            // generate dm_values
            //generate_depthmap_set(min_dm_value, max_dm_value, max_dm_num, depthmap_values, dm_values, rng);
            generate_depthmap_index_set(min_dm_value, max_dm_value, max_dm_num, depthmap_values, dm_indexes, rng);

            N = (uint32_t)(img_h * img_w * 0.005);
            if (dataset_type == 0)
            {
                generate_random_image(img_f1, rng, img_h, img_w, BN, 0.1);
            }
            else
            {
                img_f1 = cv::Mat(img_h, img_w, CV_8UC3);
                sn_scale = sn_slope * dm_indexes[0] + sn_int;
                create_color_map(img_h, img_w, sn_scale, octaves, persistence, wood.data(), img_f1.data);
            }

            // clone the images
            img_f2 = img_f1.clone();

            // create gaussian kernel and blur imgs
            create_gaussian_kernel(kernel_size, sigma_table[br1_table[dm_indexes[0]]], kernel);
            cv::filter2D(img_f1, img_f1, -1, kernel, cv::Point(-1, -1), 0.0, cv::BorderTypes::BORDER_REPLICATE);

            create_gaussian_kernel(kernel_size, sigma_table[br2_table[dm_indexes[0]]], kernel);
            cv::filter2D(img_f2, img_f2, -1, kernel, cv::Point(-1, -1), 0.0, cv::BorderTypes::BORDER_REPLICATE);

            // create depth map
            cv::Mat depth_map(img_h, img_w, CV_8UC1, cv::Scalar::all(depthmap_values[dm_indexes[0]]));

            // blur imgs using dm_values and random masks
            for (idx = 1; idx < dm_indexes.size(); ++idx)
            {
                f1_layer = img_f1.clone();
                f2_layer = img_f2.clone();

                //min_N = (int32_t)ceil((num_objects) / (1 + exp(-0.35 * dm_values[idx] + (0.035 * num_objects))) + 2);
                min_N = (int32_t)(num_objects / (double)(1.0 + exp(-0.1 * (depthmap_values[dm_indexes[idx]] - (max_dm_value-min_dm_value)/2.0))) + 5);
                max_N = (int32_t)ceil(1.25 * min_N);
                N = rng.uniform(min_N, max_N + 1);

                // define the scale factor
                scale = 60.0 / (double)img_size.width;

                // 
                if (dataset_type == 0)
                {
                    generate_random_image(random_img, rng, img_h, img_w, BN, 0.1);
                }
                else
                {
                    sn_scale = sn_slope * dm_indexes[idx] + sn_int;
                    random_img = cv::Mat(img_h, img_w, CV_8UC3);
                    create_color_map(img_h, img_w, sn_scale, octaves, persistence, wood.data(), random_img.data);
                }

                // generate random overlay
                generate_random_overlay(random_img, rng, output_img, mask, N, scale);

                overlay_image(f1_layer, output_img, mask);
                overlay_image(f2_layer, output_img, mask);

                // overlay depthmap
                overlay_depthmap(depth_map, mask, depthmap_values[dm_indexes[idx]]);

                // blur f1
                create_gaussian_kernel(kernel_size, sigma_table[br1_table[dm_indexes[idx]]], kernel);
                blur_layer(f1_layer, img_f1, mask, kernel, rng);

                // blur f2
                create_gaussian_kernel(kernel_size, sigma_table[br2_table[dm_indexes[idx]]], kernel);
                blur_layer(f2_layer, img_f2, mask, kernel, rng);
            }

            cv::hconcat(img_f1, img_f2, montage);
            cv::imshow(window_name, montage);
            cv::waitKey(10);

            std::string f1_filename = num2str<int>(jdx, "images/image_f1_%04i.png");
            std::string f2_filename = num2str<int>(jdx, "images/image_f2_%04i.png");
            std::string dmap_filename = num2str<int>(jdx, "depth_maps/dm_%04i.png");

            cv::imwrite(save_location + f1_filename, img_f1);
            cv::imwrite(save_location + f2_filename, img_f2);
            cv::imwrite(save_location + dmap_filename, depth_map);

            std::cout << f1_filename << ", " << f2_filename << ", " << dmap_filename << std::endl;
            std::cout << dm_indexes << std::endl;
            DataLog_Stream << f1_filename << ", " << f2_filename << ", " << dmap_filename << std::endl;
        } // end of for loop

        DataLog_Stream.close();
    }
    catch(std::exception& e)
    {
        std::cout << "Error: " << e.what() << std::endl;
    }


    if (dataset_type == 1)
    {
#if defined(_WIN32) | defined(__WIN32__) | defined(__WIN32) | defined(_WIN64) | defined(__WIN64)
        FreeLibrary(simplex_noise_lib);
#else
        dlclose(simplex_noise_lib);
#endif
    }


    std::cout << "End of Program.  Press Enter to close..." << std::endl;
	std::cin.ignore();
    cv::destroyAllWindows();

}   // end of main

