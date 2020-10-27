#ifndef _CV_BLUR_PROCESS_H_
#define _CV_BLUR_PROCESS_H_

#include <cstdint>

// OpenCV includes
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/calib3d.hpp>

#include <cv_random_image_gen.h>
#include <blur_params.h>

#include <set>

/*
This function will take a base image and overlay a set of shapes and then blur them according to the sigma value

@param src input image that will be modified and returned.  This image should be CV_32FC3 type
@param rng random number generator object
@param sigma value that determines the blur kernel properties
@param scale optional parameter to determine the size of the object
*/
inline void blur_layer(cv::Mat& src,
    cv::RNG& rng,
    double sigma,
    double scale = 0.3)
{

    // get the image dimensions for other uses
    int nr = src.rows;
    int nc = src.cols;

    // clone the source image
    cv::Mat src_clone = src.clone();

    // create the inital blank mask
    cv::Mat BM_1 = cv::Mat(src.size(), CV_32FC3, cv::Scalar::all(0.0));

    // generate a random color 
    cv::Scalar C = (1.0 / 255.0) * cv::Scalar(rng.uniform(0, 256), rng.uniform(0, 256), rng.uniform(0, 256));

    // generate a random point within the image using the cv::RNG uniform funtion (use nr,nc)
    cv::Point center = cv::Point(rng.uniform(0, nc), rng.uniform(0, nr));

    // generate the random radius values for an ellipse using one of the RNG functions
    cv::Size axes = cv::Size(rng.uniform(20, 200), rng.uniform(20, 200));
    double startAngle = 0;
    double endAngle = 360;

    // generate a random angle between 0 and 360 degrees for the ellipse using one of the RNG functions
    double angle = rng.uniform(0, 360);

    // use the cv::ellipse function and the random points to generate a random ellipse on top of the src_clone image
    cv::ellipse(src_clone, center, axes, angle, startAngle, endAngle, C, -1);

    // use the same points to generate the same ellipse on the mask with color = CV::Scalar(1.0, 1.0, 1.0)
    cv::ellipse(BM_1, center, axes, angle, startAngle, endAngle, cv::Scalar(1.0, 1.0, 1.0), -1);


    // blur the src_clone image with the overlay and blur the mask image using the sigma value to determine the blur kernel size
    cv::boxFilter(src_clone, src_clone, -1, cv::Size(sigma, sigma), cv::Point(-1, -1), true);
    cv::boxFilter(BM_1, BM_1, -1, cv::Size(sigma, sigma), cv::Point(-1, -1), true);


    // multiply the src image times (cv::Scalar(1.0, 1.0, 1.0) - BM_1)
    cv::Mat L1_1;
    cv::multiply(src, cv::Scalar(1.0, 1.0, 1.0) - BM_1, L1_1);

    // multiply the src_clone image times BM_1
    cv::Mat L1_2;
    cv::multiply(src_clone, BM_1, L1_2);

    // set src equal to L1_1 + L1_2
    cv::add(L1_1, L1_2, src);

}   // end of blur_layer 


inline void blur_layer(cv::Mat& random_img,
    cv::Mat random_overlay,
    cv::Mat mask,
    cv::Mat kernel,
    cv::RNG& rng,
    double sigma,
    double scale = 0.3)
{
    // clone the source image
    cv::Mat src_clone = random_img.clone();

    // blur the src_clone image with the overlay and blur the mask image
    cv::filter2D(src_clone, src_clone, -1, kernel, cv::Point(-1, -1), 0.0, cv::BorderTypes::BORDER_REPLICATE);
    cv::filter2D(mask, mask, -1, kernel, cv::Point(-1, -1), 0.0, cv::BorderTypes::BORDER_REPLICATE);

    // multiply the src image times (cv::Scalar(1.0, 1.0, 1.0) - mask)
    cv::Mat L1_1;
    cv::multiply(random_img, cv::Scalar(1.0, 1.0, 1.0) - mask, L1_1);

    // blur random_overlay
    cv::filter2D(random_overlay, random_overlay, -1, kernel, cv::Point(-1, -1), 0.0, cv::BorderTypes::BORDER_REPLICATE);

    // set src equal to L1_1 + random_overlay
    cv::add(L1_1, random_overlay, random_img);

}   // end of blur_layer 


//-----------------------------------------------------------------------------
void genrate_depthmap_set(uint16_t min_dm_value, uint16_t max_dm_value, std::vector<uint16_t> &dm_values, cv::RNG rng)
{
    std::set<uint16_t, std::greater<uint16_t>> values;

    for (int i = 0; i < max_dm_num; i++)
    {
        uint16_t idx = rng.uniform(min_dm_value, max_dm_value+1);
        values.insert(idx);
    }

    dm_values = std::vector<uint16_t>(values.begin(), values.end());
}


//-----------------------------------------------------------------------------
void generate_random_mask(cv::Mat& output_mask,
    cv::Size img_size,
    cv::RNG& rng,
    uint32_t num_shapes,
    double scale = 0.1)
{

    unsigned int idx;

    int nr = img_size.width;
    int nc = img_size.height;

    // create the image with a black background color
    output_mask = cv::Mat(nr, nc, CV_32FC3, cv::Scalar::all(0.0));

    // create N shapes
    for (idx = 0; idx < num_shapes; ++idx)
    {

        // color for all shapes
        cv::Scalar C = cv::Scalar(1.0, 1.0, 1.0);

        // generate the random point
        long x = rng.uniform(0, nc);
        long y = rng.uniform(0, nr);
        long r1, r2, h, w, s;
        double a;

        cv::RotatedRect rect;
        cv::Point2f vertices2f[4];
        cv::Point vertices[4];
        std::vector<cv::Point> pts(3);
        std::vector<std::vector<cv::Point> > vpts(1);
        vpts.push_back(pts);


        long x_1; //= -window_width / 2;
        long x_2; //= window_width * 3 / 2;
        long y_1; //= -window_width / 2;
        long y_2; //= window_width * 3 / 2;

        // get the shape type
        switch (rng.uniform(0, 3))
        {
            // filled ellipse
        case 0:

            // pick a random radi for the ellipse
            r1 = std::floor(0.5 * scale * rng.uniform(0, std::min(nr, nc)));
            r2 = std::floor(0.5 * scale * rng.uniform(0, std::min(nr, nc)));
            a = rng.uniform(0.0, 360.0);

            cv::ellipse(output_mask, cv::Point(x, y), cv::Size(r1, r2), a, 0.0, 360.0, C, -1, cv::LineTypes::LINE_8, 0);
            break;

            // filled rectangle
        case 1:

            h = std::floor(scale * rng.uniform(0, std::min(nr, nc)));
            w = std::floor(scale * rng.uniform(0, std::min(nr, nc)));
            a = rng.uniform(0.0, 360.0);

            // Create the rotated rectangle
            rect = cv::RotatedRect(cv::Point(x, y), cv::Size(w, h), a);

            // We take the edges that OpenCV calculated for us
            rect.points(vertices2f);

            // Convert them so we can use them in a fillConvexPoly
            for (int jdx = 0; jdx < 4; ++jdx)
            {
                vertices[jdx] = vertices2f[jdx];
            }

            // Now we can fill the rotated rectangle with our specified color
            cv::fillConvexPoly(output_mask, vertices, 4, C);
            break;

            // filled polygon
        case 2:

            // 3 to 8 sided polygon
            s = rng.uniform(3, 9);
            x_1 = -(0.5 * scale * nc);
            x_2 = (1.5 * scale * nc);
            y_1 = -(0.5 * scale * nr);
            y_2 = (1.5 * scale * nc);

            pts.clear();

            for (int jdx = 0; jdx < s; ++jdx)
            {
                pts.push_back(cv::Point((long)(rng.uniform((int)x_1, (int)x_2) + x), (long)(rng.uniform((int)y_1, (int)y_2) + y)));
            }

            vpts[0] = pts;
            cv::fillPoly(output_mask, vpts, C, cv::LineTypes::LINE_8, 0);

            break;

        }   // end switch

    }   // end for loop

} // end of generate_random_mask


//-----------------------------------------------------------------------------
void generate_random_overlay(cv::Size img_size,
    cv::RNG& rng, 
    uint32_t num_shapes, 
    cv::Mat& output_img, 
    cv::Mat& output_mask)
{
    // define the number of shapes for generate_radnom_image
    uint32_t N = img_size.height * img_size.width * 0.004;
    
    // define the scale factor
    double scale = 60 / (double)img_size.width;

    // generate random image
    cv::Mat random_img;
    generate_random_image(random_img, rng, img_size.width, img_size.height, N, scale);
    random_img.convertTo(random_img, CV_32FC3, (1.0/255.0));

    // generate random mask
    generate_random_mask(output_mask, img_size, rng, num_shapes);

    // multiply random_img times output_mask
    cv::multiply(random_img, output_mask, output_img);

} // end of generate_random_overlay


//-----------------------------------------------------------------------------
void overlay_depthmap(cv::Mat& depth_map, cv::Mat mask, uint16_t dm_value)
{
    cv::Mat bg_mask;
    
    mask.convertTo(mask, CV_8U); 
    cv::cvtColor(mask, mask, cv::COLOR_BGR2GRAY); 

    // set bg_mask = 1 - mask
    cv::subtract(cv::Scalar(1), mask, bg_mask);

    cv::multiply(depth_map, bg_mask, depth_map);

    // multiply mask with dm_value  
    cv::multiply(mask, cv::Scalar(dm_value), mask);

    // overlay depthmap
    cv::add(depth_map, mask, depth_map);

}


void new_shapes(cv::Mat& img, uint32_t img_h, uint32_t img_w, cv::RNG rng)
{
    img = cv::Mat(img_h, img_w, CV_8UC1, cv::Scalar::all(0));
    std::vector<cv::Point> pts;
    std::vector<std::vector<cv::Point> > vpts(1);
    double scale = 0.2;
    int num_shapes = 10;
    int h, w, s;
    double a;
    int radius;
    int angle;

    for (int N = 0; N < num_shapes; N++)
    {
        h = std::floor(0.5 * scale * rng.uniform(0, std::min(img_h, img_w)));
        w = std::floor(0.5 * scale * rng.uniform(0, std::min(img_h, img_w)));
        
        s = rng.uniform(3, 9);
        a = 360 / (double)s;
        
        cv::Point center(rng.uniform(0, img_h), rng.uniform(0, img_w));

        pts.clear();
        for (int i = 0; i < s; i++)
        {
            angle = rng.uniform(i * a, (i + 1) * a);
            
            if (w/std::abs(std::cos((CV_PI / 180) * angle)) <= h/std::abs(std::sin((CV_PI / 180) * angle)))
            {
                radius = std::abs(w / (double)std::cos((CV_PI / 180) * angle));
            }
            else
            {
                radius = std::abs(h / (double)std::sin((CV_PI / 180) * angle));
            }

            pts.push_back(cv::Point(radius * std::cos((CV_PI / 180) * angle), radius * std::sin((CV_PI / 180) * angle)));
        }

        vpts[0] = pts;
        cv::fillPoly(img, vpts, cv::Scalar(255), cv::LineTypes::LINE_8, 0, center);
        // display bounded box 
        cv::rectangle(img, cv::Point(center.x - w, center.y - h), cv::Point(center.x + w, center.y + h), cv::Scalar(128));
    }
}

#endif // _CV_BLUR_PROCESS_H_
