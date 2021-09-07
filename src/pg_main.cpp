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
#include <cv_dft_conv.h>
#include <blur_params.h>
#include <num2string.h>
#include <file_ops.h>

//template<typename T>
//bool comp_pair(T fisrt, T second)
//{
//    return first < second;
//}


// ----------------------------------------------------------------------------------------
bool compare(std::pair<uint8_t, uint8_t> p1, std::pair<uint8_t, uint8_t> p2)
{
    return max(p1.first, p1.second) < max(p2.first, p2.second);
}

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
    cv::Size img_size(img_h, img_w);

    cv::RNG rng(time(NULL));

    // timing variables
    typedef std::chrono::duration<double> d_sec;
    auto start_time = chrono::system_clock::now();
    auto stop_time = chrono::system_clock::now();
    auto elapsed_time = chrono::duration_cast<d_sec>(stop_time - start_time);

    cv::Mat img_f1, img_f2;
    cv::Mat kernel;
    cv::Mat output_img, mask;
    cv::Mat f1_layer, f2_layer;
    cv::Mat random_img;
    cv::Mat montage;
    std::vector<uint16_t> dm_values;
    std::vector<uint16_t> dm_indexes;
    int32_t min_N, max_N;
    uint32_t N;
    double scale = 0.1;
    uint32_t BN = 1000;
    std::string window_name = "montage";

    //uint8_t bg_dm_value;
    //uint8_t fg_dm_value;
    std::string scenario_name;
    std::pair<uint8_t, double> bg_dm;
    std::pair<uint8_t, double> fg_dm;
    std::vector<std::pair<uint8_t, uint8_t>> bg_br_table;
    std::vector<std::pair<uint8_t, uint8_t>> fg_br_table;
    double prob_bg = 0.31;    // set the probablility of selecting the background depthmap value
    double prob_fg = 0.35;    // set the probability of selecting the foreground depthmap value
    double bg_x = 0, fg_x = 0;
    //double sigma_1 = 0.0, sigma_2 = 0.0;
    //uint8_t dm_value;
    std::vector<cv::Mat> blur_kernels;
    std::vector<cv::Mat> fft_blur_kernels;

    std::vector<uint8_t> depthmap_values;
    std::vector<double> sigma_table;
    std::vector<uint8_t> br1_table, tmp_br1_table;
    std::vector<uint8_t> br2_table, tmp_br2_table;
    uint8_t aperture;
    uint16_t slope;
    uint16_t intercept;
    uint32_t wavelength_min;
    uint32_t wavelength_max;
    double refractive_index_min;
    double refractive_index_max;
    uint8_t dataset_type;
    int32_t max_dm_num;
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
    read_blur_params(param_filename, scenario_name, bg_dm, fg_dm, bg_br_table, fg_br_table, depthmap_values, sigma_table,
        br1_table, br2_table, aperture, slope, intercept, wavelength_min, wavelength_max, refractive_index_min, refractive_index_max,
        dataset_type, img_h, img_w, max_dm_num, num_objects, num_images, save_location);

    uint16_t min_dm_value = fg_dm.first;        // depthmap_values.front();
    uint16_t max_dm_value = bg_dm.first;        // depthmap_values.back();
    prob_bg = bg_dm.second;                     // set the probablility of selecting the background depthmap value
    prob_fg = fg_dm.second;                     // set the probability of selecting the foreground depthmap value

    // get the max value of blur radius
    uint8_t max_br_value = *std::max_element(br1_table.begin(), br1_table.end());
    max_br_value = std::max(max_br_value, *std::max_element(br2_table.begin(), br2_table.end()));
    auto p1 = *std::max_element(bg_br_table.begin(), bg_br_table.end(), compare);
    auto p2 = *std::max_element(fg_br_table.begin(), fg_br_table.end(), compare);
    max_br_value = std::max(max_br_value, std::max(p1.first, p1.second));
    max_br_value = std::max(max_br_value, std::max(p2.first, p2.second));

    cv::Mat tmp;
    // gerenate all of the kernels once and then use them throughout
    for (idx = 0; idx <= max_br_value; ++idx)
    {
        create_gaussian_kernel(kernel_size, sigma_table[idx], kernel);
        blur_kernels.push_back(kernel.clone());

        cv::dft(kernel, tmp);// , cv::DFT_COMPLEX_OUTPUT);
        fft_blur_kernels.push_back(tmp.clone());    
    }


    //cv::Mat planes[2];// = { cv::Mat::zeros(blur_kernels[max_br_value].size(), CV_32F), cv::Mat::zeros(blur_kernels[max_br_value].size(), CV_32F) };

    //cv::split(fft_blur_kernels[0], planes);


    // create results directories if they do not exist
    mkdir(save_location + "images");
    mkdir(save_location + "depth_maps");

    // save the parameters that were used to generate the dataset
    std::ofstream param_stream(save_location + scenario_name + "parameters.txt", std::ofstream::out);
    param_stream << "# Parameters used to generate the dataset" << std::endl;
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

    //param_stream.close();

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
            sn_int = 0.02;
            sn_slope = (0.06 - sn_int) / (double)(max_dm_value - min_dm_value);
        }

        std::ofstream DataLog_Stream(save_location + scenario_name + "input_file.txt", std::ofstream::out);
        DataLog_Stream << "# Data Directory" << std::endl;
        DataLog_Stream << save_location << ", " << save_location << std::endl;
        DataLog_Stream << std::endl;
        DataLog_Stream << "# focus point 1 filename, focus point 2 filename, depthmap filename" << std::endl;
        
        std::cout << "Data Directory: " << save_location << std::endl;

        start_time = chrono::system_clock::now();

        for (jdx = 0; jdx < num_images; ++jdx)
        {
            // clear out the temp blur radius tables
            tmp_br1_table.clear();
            tmp_br2_table.clear();

            dm_values.clear();

            // generate random dm_values that include the foreground and background values
            int32_t tmp_dm_num = max_dm_num;

            // get the probablility that the background depthmap value will be used
            bg_x = rng.uniform(0.0, 1.0);

            // get the probability that the foreground depthmap value will be used
            fg_x = rng.uniform(0.0, 1.0);

            if (bg_x < prob_bg)
                tmp_dm_num--;

            if (fg_x < prob_fg)
                tmp_dm_num--;

            //generate_depthmap_set(min_dm_value, max_dm_value, tmp_dm_num, depthmap_values, dm_values, rng);
            generate_depthmap_index_set(min_dm_value, max_dm_value, tmp_dm_num, depthmap_values, dm_indexes, rng);

            // check the background probability and fill in the tables
            if (bg_x < prob_bg)
            {
                uint16_t dm = rng.uniform(0, bg_br_table.size());
                tmp_br1_table.push_back(bg_br_table[dm].first);
                tmp_br2_table.push_back(bg_br_table[dm].second);
                //sigma_1 = sigma_table[bg_br_table[dm].first];
                //sigma_2 = sigma_table[bg_br_table[dm].second];
                dm_values.push_back(bg_dm.first);
                //dm_values.insert(dm_values.begin(), )
            }

            // fill in the tables for the region of interest depthmap values
            for (idx = 0; idx < dm_indexes.size(); ++idx)
            {
                tmp_br1_table.push_back(br1_table[dm_indexes[idx]]);
                tmp_br2_table.push_back(br2_table[dm_indexes[idx]]);
                dm_values.push_back(depthmap_values[dm_indexes[idx]]);
            }

            // check the foreground probability and fill in the tables
            if (fg_x < prob_fg)
            {
                uint16_t dm = rng.uniform(0, bg_br_table.size());
                tmp_br1_table.push_back(fg_br_table[dm].first);
                tmp_br2_table.push_back(fg_br_table[dm].second);
                //sigma_1 = sigma_table[bg_br_table[dm].first];
                //sigma_2 = sigma_table[bg_br_table[dm].second];
                dm_values.push_back(fg_dm.first);
                //dm_values.insert(dm_values.begin(), )
            }

            N = (uint32_t)(img_h * img_w * 0.001);
            switch (dataset_type)
            {
            case 0:
                generate_random_image(img_f1, rng, img_h, img_w, BN, 0.1);
                break;

            case 1:
                img_f1 = cv::Mat(img_h, img_w, CV_8UC3);
                sn_scale = sn_slope * dm_values[0] + sn_int;
                create_color_map(img_h, img_w, sn_scale, octaves, persistence, wood.data(), img_f1.data);
                break;

            case 2:
                //img_f1 = cv::Mat(img_h, img_w, CV_8UC3, cv::Scalar::all(0,0,0));
                cv::hconcat(cv::Mat(img_h, img_w >> 1, CV_8UC3, cv::Scalar::all(0)), cv::Mat(img_h, img_w - (img_w >> 1), CV_8UC3, cv::Scalar::all(255)), img_f1);
                break;

            // black and white checkerboard
            case 3:
                generate_checkerboard(48, 48, img_w, img_h, img_f1);
                break;
            }

            // clone the images
            img_f2 = img_f1.clone();


            //cv::Mat img_f1_t = img_f1.clone();
            //cv::Mat dst;
            //dft_conv_rgb(img_f1_t, fft_blur_kernels[tmp_br1_table[0]], dst);


            // create gaussian kernel and blur imgs
            //create_gaussian_kernel(kernel_size, sigma_table[tmp_br1_table[0]], kernel);
            //cv::filter2D(img_f1, img_f1, -1, kernel, cv::Point(-1, -1), 0.0, cv::BorderTypes::BORDER_REPLICATE);
            cv::filter2D(img_f1, img_f1, -1, blur_kernels[tmp_br1_table[0]], cv::Point(-1, -1), 0.0, cv::BorderTypes::BORDER_REPLICATE);

            //create_gaussian_kernel(kernel_size, sigma_table[tmp_br2_table[0]], kernel);
            //cv::filter2D(img_f2, img_f2, -1, kernel, cv::Point(-1, -1), 0.0, cv::BorderTypes::BORDER_REPLICATE);
            cv::filter2D(img_f2, img_f2, -1, blur_kernels[tmp_br2_table[0]], cv::Point(-1, -1), 0.0, cv::BorderTypes::BORDER_REPLICATE);

            // create the initial depth map
            cv::Mat depth_map(img_h, img_w, CV_8UC1, cv::Scalar::all(dm_values[0]));

            // blur imgs using dm_values and random masks
            for (idx = 1; idx < dm_values.size(); ++idx)
            {
                f1_layer = img_f1.clone();
                f2_layer = img_f2.clone();

                // This part help shape the final distribution of depthmap values
                //min_N = (int32_t)ceil((num_objects) / (1 + exp(-0.35 * dm_values[idx] + (0.035 * num_objects))) + 2);
                //min_N = (int32_t)(num_objects / (double)(1.0 + exp(-0.1 * (depthmap_values[dm_indexes[idx]] - (max_dm_value-min_dm_value)/2.0))) + 5);
                //min_N = (int32_t)ceil(((max_dm_value) / (double)(1.0 + exp(-0.35 * depthmap_values[dm_values[idx]] + (0.175 * max_dm_value)))) + 3);
                min_N = (int32_t)ceil(((max_dm_value) / (double)(1.0 + exp(-0.365 * dm_values[idx] + (0.175 * max_dm_value)))) + 3);
                max_N = (int32_t)ceil(2.0* min_N);  // 2.0

                N = rng.uniform(min_N, max_N + 1);

                // define the scale factor
                scale = 60.0 / (double)img_size.width;

                // 
                switch (dataset_type)
                {
                case 0:
                    generate_random_image(random_img, rng, img_h, img_w, BN, 0.1);
                    break;
                    
                case 1:
                    sn_scale = sn_slope * dm_values[idx] + sn_int;
                    random_img = cv::Mat(img_h, img_w, CV_8UC3);
                    create_color_map(img_h, img_w, sn_scale, octaves, persistence, wood.data(), random_img.data);
                    break;

                case 2:
                    cv::hconcat(cv::Mat(img_h, img_w >> 1, CV_8UC3, cv::Scalar::all(0)), cv::Mat(img_h, img_w - (img_w >> 1), CV_8UC3, cv::Scalar::all(255)), random_img);
                    break;
                }

                // generate random overlay
                generate_random_overlay(random_img, rng, output_img, mask, N, scale);

                overlay_image(f1_layer, output_img, mask);
                overlay_image(f2_layer, output_img, mask);

                // overlay depthmap
                //overlay_depthmap(depth_map, mask, depthmap_values[dm_indexes[idx]]);
                overlay_depthmap(depth_map, mask, dm_values[idx]);

                // blur f1
                //create_gaussian_kernel(kernel_size, sigma_table[br1_table[dm_indexes[idx]]], kernel);
                //create_gaussian_kernel(kernel_size, sigma_table[tmp_br1_table[idx]], kernel);
                //blur_layer(f1_layer, img_f1, mask, kernel, rng);
                blur_layer(f1_layer, img_f1, mask, blur_kernels[tmp_br1_table[idx]], rng);

                // blur f2
                //create_gaussian_kernel(kernel_size, sigma_table[br2_table[dm_indexes[idx]]], kernel);
                //create_gaussian_kernel(kernel_size, sigma_table[tmp_br2_table[idx]], kernel);
                //blur_layer(f2_layer, img_f2, mask, kernel, rng);
                blur_layer(f2_layer, img_f2, mask, blur_kernels[tmp_br2_table[idx]], rng);
            }

            cv::hconcat(img_f1, img_f2, montage);
            cv::imshow(window_name, montage);
            cv::waitKey(10);

            std::string f1_filename = "images/" + scenario_name + num2str<int>(jdx, "image_f1_%04i.png");
            std::string f2_filename = "images/" + scenario_name + num2str<int>(jdx, "image_f2_%04i.png");
            std::string dmap_filename = "depth_maps/" + scenario_name + num2str<int>(jdx, "dm_%04i.png");

            cv::imwrite(save_location + f1_filename, img_f1);
            cv::imwrite(save_location + f2_filename, img_f2);
            cv::imwrite(save_location + dmap_filename, depth_map);

            std::cout << f1_filename << ", " << f2_filename << ", " << dmap_filename << std::endl;
            std::cout << dm_values << std::endl;

            param_stream << "image " << num2str<int>(jdx, "%03d: ") << dm_values << std::endl;
            param_stream << "           " << tmp_br1_table << std::endl;
            param_stream << "           " << tmp_br2_table << std::endl;

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

