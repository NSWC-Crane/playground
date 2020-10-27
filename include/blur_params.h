#ifndef _BLUR_PARAMS_H_
#define _BLUR_PARAMS_H_

// the sigma values used to generate the gaussian kernel
const std::vector<double> sigma_table = {0.0005,0.1725,0.3450,0.5175,0.6900,0.8625,1.0350,1.2075,1.3800,1.5525,1.7250,1.8975,2.0700,2.2425,2.4150,
                          2.5875,2.7600,2.9325,3.1050,3.2775,3.4500,3.6225,3.7950,3.9675,4.1400,4.3125,4.4850,4.6575,4.8300,5.0025,
                          5.1750,5.3475,5.5200,5.6925,5.8650,6.0375,6.2100,6.3825,6.5550,6.7275,6.9000,7.0725,7.2450,7.4175,7.5900,
                          7.7625,7.9350,8.1075,8.2800,8.4525,8.6250,8.7975,8.9700,9.1425,9.3150,9.4875,9.6600,9.8325,10.0050,10.1775,
                          10.3500,10.5225,10.6950,10.8675,11.0400,11.2125,11.3850,11.5575,11.7300,11.9025,12.0750,12.2475};

// the blur radius tables for two different focus points - tb22c: 60-100 V
// fp1: 594
const std::vector<uint8_t> br1_table = {1,2,3,4,5,6,7,8,9,9,10,11,12,12,13,14,15,15,16,16,17,18,18,19,19,20,20,21,21,22,22,23,23,24,24,25,25,25,26,26,27};
// fp2: 1010
const std::vector<uint8_t> br2_table = {26,25,24,23,22,21,21,20,19,18,17,16,16,15,14,14,13,12,12,11,10,10,9,9,8,8,7,7,6,6,5,5,4,4,3,3,2,2,2,1,1};

// gaussian kernel size
const uint32_t kernel_size = 69;

// maximum number of depthmap values within a single image
const uint32_t max_dm_num = 10;

// starting number of objects in the farthest layer
const uint32_t num_objects = 40;

// the range of depthmap values
uint16_t min_dm_value = 0;
uint16_t max_dm_value = br1_table.size()-1;


#include "file_parser.h"

void read_blur_params(std::string param_filename, 
    std::vector<uint8_t> &depthmap_values, 
    std::vector<double> &sigma_table, 
    std::vector<uint8_t> &br1_table, 
    std::vector<uint8_t> &br2_table,
    uint8_t &dataset_type,
    uint32_t &max_dm_num,
    uint32_t &num_objects,
    uint32_t &num_images,
    std::string &save_location
)
{
    std::vector<std::vector<std::string>> params;
    parse_csv_file(param_filename, params);


    for (uint64_t idx = 0; idx < params.size(); ++idx)
    {
        switch (idx)
        {
        case 0:
            for (uint32_t i = 0; i < params[idx].size(); i++)
            {
                depthmap_values.push_back((uint8_t)std::stoi(params[idx][i]));
            }
            break;
        case 1:
            for (uint32_t i = 0; i < params[idx].size(); i++)
            {
                sigma_table.push_back(std::stod(params[idx][i]));
            }
            break;
        case 2:
            for (uint32_t i = 0; i < params[idx].size(); i++)
            {
                br1_table.push_back((uint8_t)std::stod(params[idx][i]));
            }
            break;
        case 3:
            for (uint32_t i = 0; i < params[idx].size(); i++)
            {
                br2_table.push_back((uint8_t)std::stod(params[idx][i]));
            }
            break;
        case 4:
            dataset_type = (uint8_t)std::stoi(params[idx][0]);
            break;
        case 5:
            max_dm_num = (uint32_t)std::stoi(params[idx][0]);
            break;
        case 6:
            num_objects = (uint32_t)std::stoi(params[idx][0]);
            break;
        case 7:
            num_images = (uint32_t)std::stoi(params[idx][0]);
            break;
        case 8:
            save_location = params[idx][0];
            break;
        default:
            break;
        }
    }

    
}   // end of read_blur_params
    


#endif  // _BLUR_PARAMS_H_
