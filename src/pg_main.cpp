#define _CRT_SECURE_NO_WARNINGS
// D:\Projects\vs_gen\build\Release\vs_gen.lib

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
//#include <cv_blur_process.h>
//#include <cv_random_image_gen.h>
//#include <cv_create_gaussian_kernel.h>
//#include <cv_dft_conv.h>
#include <blur_params.h>
#include <num2string.h>
#include <file_ops.h>

#include <vs_gen_lib.h>

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

// ----------------------------------------------------------------------------
void generate_checkerboard(uint32_t block_w, uint32_t block_h, uint32_t img_w, uint32_t img_h, cv::Mat& checker_board)
{
    uint32_t idx = 0, jdx = 0;

    cv::Mat white = cv::Mat(block_w, block_h, CV_8UC3, cv::Scalar::all(255));

    checker_board = cv::Mat(img_h + block_h, img_w + block_w, CV_8UC3, cv::Scalar::all(0));

    bool color_row = false;
    bool color_column = false;

    for (idx = 0; idx < img_h; idx += block_h)
    {
        color_row = !color_row;
        color_column = color_row;

        for (jdx = 0; jdx < img_w; jdx += block_w)
        {
            if (!color_column)
                white.copyTo(checker_board(cv::Rect(jdx, idx, block_w, block_h)));

            color_column = !color_column;
        }

    }

    // need to add cropping of image
    cv::Rect roi(0, 0, img_w, img_h);
    checker_board = checker_board(roi);
}

// ----------------------------------------------------------------------------------------
int main(int argc, char** argv)
{
    uint32_t idx = 0, jdx = 0;
    uint32_t img_h = 512;
    uint32_t img_w = 512;
    //cv::Size img_size(img_h, img_w);

    cv::RNG rng(time(NULL));

    // timing variables
    typedef std::chrono::duration<double> d_sec;
    auto start_time = chrono::system_clock::now();
    auto stop_time = chrono::system_clock::now();
    auto elapsed_time = chrono::duration_cast<d_sec>(stop_time - start_time);
    std::string platform;

    cv::Mat img_f1, img_f2;
    cv::Mat montage;
    cv::Mat depth_map;
    //std::vector<uint16_t> dm_values;
    //std::vector<uint16_t> dm_indexes;
    //int32_t min_N, max_N;
    //uint32_t N;
    //double scale = 0.1;
    //uint32_t BN = 1000;
    std::string window_name = "montage";

    std::string blur_param_file;

    //uint8_t bg_dm_value;
    //uint8_t fg_dm_value;
    std::string scenario_name;
    std::pair<uint8_t, double> bg_dm;
    std::pair<uint8_t, double> fg_dm;
    std::vector<std::pair<uint8_t, uint8_t>> bg_br_table;
    std::vector<std::pair<uint8_t, uint8_t>> fg_br_table;
    //double prob_bg = 0.31;    // set the probablility of selecting the background depthmap value
    //double prob_fg = 0.35;    // set the probability of selecting the foreground depthmap value
    //double bg_x = 0, fg_x = 0;
    //double sigma_1 = 0.0, sigma_2 = 0.0;
    //uint8_t dm_value;
    //std::vector<cv::Mat> blur_kernels;
    //std::vector<cv::Mat> fft_blur_kernels;

    std::vector<uint8_t> depthmap_values;
    std::vector<double> sigma_table;
    std::vector<uint8_t> br1_table, tmp_br1_table;
    std::vector<uint8_t> br2_table, tmp_br2_table;
    //uint8_t aperture;
    //uint16_t slope;
    //uint16_t intercept;
    //uint32_t wavelength_min;
    //uint32_t wavelength_max;
    //double refractive_index_min;
    //double refractive_index_max;
    //int32_t max_dm_num;
    uint8_t dataset_type;
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
    init_ simplex_init;
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

    uint8_t HPC = 0;
    get_platform(platform);
    if (platform.compare(0, 3, "HPC") == 0)
    {
        std::cout << "HPC Platform Detected." << std::endl;
        HPC = 1;
    }

    std::string param_filename = argv[1];
    //read_blur_params(param_filename, scenario_name, bg_dm, fg_dm, bg_br_table, fg_br_table, depthmap_values, sigma_table,
    //    br1_table, br2_table, aperture, slope, intercept, wavelength_min, wavelength_max, refractive_index_min, refractive_index_max,
    //    dataset_type, img_h, img_w, max_dm_num, num_objects, num_images, save_location);

    read_blur_params(param_filename, scenario_name, blur_param_file, img_h, img_w, num_images, save_location);

    //uint16_t min_dm_value = fg_dm.first;        // depthmap_values.front();
    //uint16_t max_dm_value = bg_dm.first;        // depthmap_values.back();
    //prob_bg = bg_dm.second;                     // set the probablility of selecting the background depthmap value
    //prob_fg = fg_dm.second;                     // set the probability of selecting the foreground depthmap value

    //// get the max value of blur radius
    //uint8_t max_br_value = *std::max_element(br1_table.begin(), br1_table.end());
    //max_br_value = std::max(max_br_value, *std::max_element(br2_table.begin(), br2_table.end()));
    //auto p1 = *std::max_element(bg_br_table.begin(), bg_br_table.end(), compare);
    //auto p2 = *std::max_element(fg_br_table.begin(), fg_br_table.end(), compare);
    //max_br_value = std::max(max_br_value, std::max(p1.first, p1.second));
    //max_br_value = std::max(max_br_value, std::max(p2.first, p2.second));

    //cv::Mat tmp;
    //// gerenate all of the kernels once and then use them throughout
    //for (idx = 0; idx <= max_br_value; ++idx)
    //{
    //    create_gaussian_kernel(kernel_size, sigma_table[idx], kernel);
    //    blur_kernels.push_back(kernel.clone());

    //    cv::dft(kernel, tmp);// , cv::DFT_COMPLEX_OUTPUT);
    //    fft_blur_kernels.push_back(tmp.clone());    
    //}


    ////cv::Mat planes[2];// = { cv::Mat::zeros(blur_kernels[max_br_value].size(), CV_32F), cv::Mat::zeros(blur_kernels[max_br_value].size(), CV_32F) };

    ////cv::split(fft_blur_kernels[0], planes);


    // create results directories if they do not exist
    mkdir(save_location + "images");
    mkdir(save_location + "depth_maps");

    // save the parameters that were used to generate the dataset
    std::ofstream param_stream(save_location + scenario_name + "parameters.txt", std::ofstream::out);
    param_stream << "# Parameters used to generate the dataset" << std::endl;
/*
    param_stream << depthmap_values << std::endl;
    param_stream << sigma_table << std::endl;
    param_stream << br1_table << std::endl;
    param_stream << br2_table << std::endl;
    param_stream << static_cast<uint32_t>(dataset_type) << std::endl;
    param_stream << img_h << "," << img_w << std::endl;
    param_stream << max_dm_num << std::endl;
    param_stream << num_objects << std::endl;
    param_stream << num_images << std::endl << std::endl;
    param_stream << "------------------------------------------------------------------" << std::endl;

    // print out the parameters
    std::cout << "------------------------------------------------------------------" << std::endl;
    std::cout << "Parameters used to generate the dataset" << std::endl;
    std::cout << "Depthmap Values:  " << depthmap_values << std::endl;
    std::cout << "Sigma Table:      " << sigma_table << std::endl;
    std::cout << "Blur Radius 1:    " << br1_table << std::endl;
    std::cout << "Blur Radius 2:    " << br2_table << std::endl;
    std::cout << "Dataset Type:     " << static_cast<uint32_t>(dataset_type) << std::endl;
    std::cout << "Image Size (hxw): " << img_h << " x " << img_w << std::endl;
    std::cout << "DM Values/Image:  " << max_dm_num << std::endl;
    std::cout << "# of Objects:     " << num_objects << std::endl;
    std::cout << "# of Images:      " << num_images << std::endl;
    std::cout << "------------------------------------------------------------------" << std::endl << std::endl;
*/

    // if the platform is an HPC platform then don't display anything
    if (!HPC)
    {
        // setup the windows to display the results
        cv::namedWindow(window_name, cv::WINDOW_NORMAL);
        cv::resizeWindow(window_name, 2 * img_w, img_h);
    }

    // do work here
    try
    {    
//        if (dataset_type == 1)
//        {
//            lib_filename = "D:/Projects/simplex_noise/build/Release/sn_lib.dll";
//
//#if defined(_WIN32) | defined(__WIN32__) | defined(__WIN32) | defined(_WIN64) | defined(__WIN64)
//
//            simplex_noise_lib = LoadLibrary(lib_filename.c_str());
//
//            if (simplex_noise_lib == NULL)
//            {
//                throw std::runtime_error("error loading library");
//            }
//
//            init = (init_)GetProcAddress(simplex_noise_lib, "init");
//            evaluate = (evaluate_)GetProcAddress(simplex_noise_lib, "evaluate");
//            octave_evaluate = (octave_evaluate_)GetProcAddress(simplex_noise_lib, "octave_evaluate");
//            create_color_map = (create_color_map_)GetProcAddress(simplex_noise_lib, "create_color_map");
//#else
//            simplex_noise_lib = dlopen(lib_filename.c_str(), RTLD_NOW);
//
//            if (simplex_noise_lib == NULL)
//            {
//                throw std::runtime_error("error loading library");
//            }
//
//            init = (init_)dlsym(simplex_noise_lib, "init");
//            evaluate = (evaluate_)dlsym(simplex_noise_lib, "evaluate");
//            octave_evaluate = (octave_evaluate_)dlsym(simplex_noise_lib, "octave_evaluate");
//            create_color_map = (create_color_map_)dlsym(simplex_noise_lib, "create_color_map");
//#endif
//
//            init((long)time(NULL));
//            sn_int = 0.02;
//            sn_slope = (0.06 - sn_int) / (double)(max_dm_value - min_dm_value);
//        }

        std::ofstream DataLog_Stream(save_location + scenario_name + "input_file.txt", std::ofstream::out);
        DataLog_Stream << "# Data Directory" << std::endl;
        DataLog_Stream << save_location << ", " << save_location << std::endl;
        DataLog_Stream << std::endl;
        DataLog_Stream << "# focus point 1 filename, focus point 2 filename, depthmap filename" << std::endl;
        
        std::cout << "Data Directory: " << save_location << std::endl;

        init_from_file(blur_param_file.c_str());

        //init(sigma_table.size(), sigma_table.data(), depthmap_values.size(), depthmap_values.data(),
        //    br1_table.data(), br2_table.data(), bg_br_table.size(), (void*)bg_br_table.data(), fg_br_table.size(), (void*)fg_br_table.data(),
        //    fg_dm.second, bg_dm.second, fg_dm.first, bg_dm.first, max_dm_num);

        start_time = chrono::system_clock::now();

        double scale = 0.1;

        for (jdx = 0; jdx < num_images; ++jdx)
        {
            
            img_f1 = cv::Mat(img_h, img_w, CV_8UC3, cv::Scalar::all(0));
            img_f2 = cv::Mat(img_h, img_w, CV_8UC3, cv::Scalar::all(0));
            depth_map = cv::Mat(img_h, img_w, CV_8UC1, cv::Scalar::all(0));
            generate_scene(scale, img_w, img_h, img_f1.ptr<uint8_t>(0), img_f2.ptr<uint8_t>(0), depth_map.ptr<uint8_t>(0));


            // if the platform is an HPC platform then don't display anything
            if (!HPC)
            {
                cv::hconcat(img_f1, img_f2, montage);
                cv::imshow(window_name, montage);
                cv::waitKey(10);
            }

            std::string f1_filename = "images/" + scenario_name + num2str<int>(jdx, "image_f1_%04i.png");
            std::string f2_filename = "images/" + scenario_name + num2str<int>(jdx, "image_f2_%04i.png");
            std::string dmap_filename = "depth_maps/" + scenario_name + num2str<int>(jdx, "dm_%04i.png");

            cv::imwrite(save_location + f1_filename, img_f1);
            cv::imwrite(save_location + f2_filename, img_f2);
            cv::imwrite(save_location + dmap_filename, depth_map);

            std::cout << f1_filename << ", " << f2_filename << ", " << dmap_filename << std::endl;
            
            // this doesn't get filled in anymore
            //std::cout << dm_values << std::endl;

            //param_stream << "image " << num2str<int>(jdx, "%03d: ") << dm_values << std::endl;
            //param_stream << "           " << tmp_br1_table << std::endl;
            //param_stream << "           " << tmp_br2_table << std::endl;

            DataLog_Stream << f1_filename << ", " << f2_filename << ", " << dmap_filename << std::endl;

        } // end of for loop

        stop_time = chrono::system_clock::now();
        elapsed_time = chrono::duration_cast<d_sec>(stop_time - start_time);

        std::cout << std::endl << "------------------------------------------------------------------" << std::endl;
        std::cout << "elapsed_time (s): " << elapsed_time.count() << std::endl;
        std::cout << "------------------------------------------------------------------" << std::endl << std::endl;

        param_stream.close();
        DataLog_Stream.close();
    }
    catch(std::exception& e)
    {
        std::cout << "Error: " << e.what() << std::endl;
    }


//    if (dataset_type == 1)
//    {
//#if defined(_WIN32) | defined(__WIN32__) | defined(__WIN32) | defined(_WIN64) | defined(__WIN64)
//        FreeLibrary(simplex_noise_lib);
//#else
//        dlclose(simplex_noise_lib);
//#endif
//    }


    std::cout << "End of Program.  Press Enter to close..." << std::endl;
	std::cin.ignore();
    cv::destroyAllWindows();

}   // end of main

