#ifndef _CV_BLUR_PROCESS_H_
#define _CV_BLUR_PROCESS_H_

#include <cstdint>
#include <set>

// OpenCV includes
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/calib3d.hpp>

#include <cv_random_image_gen.h>



//-----------------------------------------------------------------------------
inline void blur_layer(cv::Mat& random_img,
    cv::Mat random_overlay,
    cv::Mat mask,
    cv::Mat kernel,
    cv::RNG& rng,
    double sigma,
    double scale = 0.3)
{
    random_img.convertTo(random_img, CV_32FC3);
    random_overlay.convertTo(random_overlay, CV_32FC3);
    mask.convertTo(mask, CV_32FC3);

    // blur the src_clone image with the overlay and blur the mask image
    cv::filter2D(mask, mask, -1, kernel, cv::Point(-1, -1), 0.0, cv::BorderTypes::BORDER_REPLICATE);

    // multiply the src image times (cv::Scalar(1.0, 1.0, 1.0) - mask)
    cv::Mat L1_1;
    cv::multiply(random_img, cv::Scalar(1.0, 1.0, 1.0) - mask, L1_1);

    // blur random_overlay
    cv::filter2D(random_overlay, random_overlay, -1, kernel, cv::Point(-1, -1), 0.0, cv::BorderTypes::BORDER_REPLICATE);

    // set src equal to L1_1 + random_overlay
    cv::add(L1_1, random_overlay, random_img);

    random_img.convertTo(random_img, CV_8UC3);
}   // end of blur_layer 


//-----------------------------------------------------------------------------
void genrate_depthmap_set(uint16_t min_dm_value, uint16_t max_dm_value, uint32_t max_dm_num, 
    const std::vector<uint8_t> depthmap_values, std::vector<uint16_t> &dm_values, cv::RNG rng)
{
    std::set<uint16_t, std::greater<uint16_t>> set_values;

    for (int idx = 0; idx<max_dm_num; idx++)
    {
        uint16_t random_idx = rng.uniform(0, depthmap_values.size());
        set_values.insert(depthmap_values.at(random_idx));
    }

    dm_values = std::vector<uint16_t>(set_values.begin(), set_values.end());
}


//-----------------------------------------------------------------------------
void generate_random_mask(cv::Mat& output_mask,
    cv::Size img_size,
    cv::RNG& rng,
    uint32_t num_shapes,
    double scale = 0.1
    )
{

    uint32_t idx, jdx;

    int nr = img_size.width;
    int nc = img_size.height;
    int min_dim;

    long x, y;
    long r1, r2, h, w, s;
    double a, angle;

    int32_t radius, max_radius;

    // create the image with a black background color
    output_mask = cv::Mat(nr, nc, CV_8UC3, cv::Scalar::all(0));

    // create N shapes
    for (idx = 0; idx < num_shapes; ++idx)
    {

        // color for all shapes
        cv::Scalar C = cv::Scalar(1, 1, 1);

        // generate the random point
        x = rng.uniform(0, nc);
        y = rng.uniform(0, nr);
        
        cv::RotatedRect rect;
        cv::Point2f vertices2f[4];
        cv::Point vertices[4];
        std::vector<cv::Point> pts;
        std::vector<std::vector<cv::Point> > vpts(1);
        //vpts.push_back(pts);

        min_dim = std::min(nr, nc);
        //long x_1; //= -window_width / 2;
        //long x_2; //= window_width * 3 / 2;
        //long y_1; //= -window_width / 2;
        //long y_2; //= window_width * 3 / 2;

        // get the shape type
        switch (rng.uniform(0, 3))
        {
        // filled ellipse
        case 0:
            
            // pick a random radi for the ellipse
            r1 = std::floor(0.5 * scale * rng.uniform(min_dim >> 2, min_dim));
            r2 = std::floor(0.5 * scale * rng.uniform(min_dim >> 2, min_dim));
            a = rng.uniform(0.0, 360.0);

            cv::ellipse(output_mask, cv::Point(x, y), cv::Size(r1, r2), a, 0.0, 360.0, C, -1, cv::LineTypes::LINE_8, 0);
            break;

        // filled rectangle
        case 1:

            h = std::floor(scale * rng.uniform(min_dim >> 2, min_dim));
            w = std::floor(scale * rng.uniform(min_dim >> 2, min_dim));
            a = rng.uniform(0.0, 360.0);

            // Create the rotated rectangle
            rect = cv::RotatedRect(cv::Point(x, y), cv::Size(w, h), a);

            // We take the edges that OpenCV calculated for us
            rect.points(vertices2f);

            // Convert them so we can use them in a fillConvexPoly
            for (jdx = 0; jdx < 4; ++jdx)
            {
                vertices[jdx] = vertices2f[jdx];
            }

            // Now we can fill the rotated rectangle with our specified color
            cv::fillConvexPoly(output_mask, vertices, 4, C);
            break;

        // filled polygon
        case 2:

            h = std::floor(scale * rng.uniform(min_dim >> 2, min_dim));
            w = std::floor(scale * rng.uniform(min_dim >> 2, min_dim));

            s = rng.uniform(3, 9);
            a = 360.0 / (double)s;

            cv::Point center(rng.uniform(0, img_size.height), rng.uniform(0, img_size.width));

            pts.clear();
            for (jdx = 0; jdx < s; ++jdx)
            {
                angle = rng.uniform(jdx * a, (jdx + 1) * a);

                if (w / std::abs(std::cos((CV_PI / 180.0) * angle)) <= h / std::abs(std::sin((CV_PI / 180.0) * angle)))
                {
                    max_radius = std::abs(w / (double)std::cos((CV_PI / 180.0) * angle));
                }
                else
                {
                    max_radius = std::abs(h / (double)std::sin((CV_PI / 180.0) * angle));
                }

                radius = rng.uniform(max_radius >> 2, max_radius);
                pts.push_back(cv::Point(radius * std::cos((CV_PI / 180.0) * angle), radius * std::sin((CV_PI / 180.0) * angle)));
            }

            vpts[0] = pts;
            cv::fillPoly(output_mask, vpts, C, cv::LineTypes::LINE_8, 0, center);

            break;

        }   // end switch

    }   // end for loop

} // end of generate_random_mask


//-----------------------------------------------------------------------------
void generate_random_overlay(cv::Size img_size,
    cv::RNG &rng, 
    uint32_t num_shapes, 
    cv::Mat &output_img, 
    cv::Mat &output_mask, 
    double scale = 0.1)
{
    // define the number of shapes for generate_radnom_image
    uint32_t N = img_size.height * img_size.width * 0.004;

    // generate random image
    cv::Mat random_img;
    generate_random_image(random_img, rng, img_size.width, img_size.height, N, scale);

    // generate random mask
    generate_random_mask(output_mask, img_size, rng, num_shapes, scale);

    // multiply random_img times output_mask
    cv::multiply(random_img, output_mask, output_img);

} // end of generate_random_overlay


//-----------------------------------------------------------------------------
void overlay_depthmap(cv::Mat &depth_map, cv::Mat mask, uint16_t dm_value)
{
    cv::Mat bg_mask;
    
    cv::cvtColor(mask, mask, cv::COLOR_BGR2GRAY); 

    // set bg_mask = 1 - mask
    cv::subtract(cv::Scalar(1), mask, bg_mask);

    cv::multiply(depth_map, bg_mask, depth_map);

    // multiply mask with dm_value  
    cv::multiply(mask, cv::Scalar(dm_value), mask);

    // overlay depthmap
    cv::add(depth_map, mask, depth_map);

}


//-----------------------------------------------------------------------------
void new_shapes(cv::Mat &img, uint32_t img_h, uint32_t img_w, cv::RNG rng)
{
    img = cv::Mat(img_h, img_w, CV_8UC1, cv::Scalar::all(0));
    std::vector<cv::Point> pts;
    std::vector<std::vector<cv::Point> > vpts(1);
    double scale = 0.2;
    int num_shapes = 10;
    int h, w, s;
    double a;
    int radius, max_radius;
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
                max_radius = std::abs(w / (double)std::cos((CV_PI / 180) * angle));
            }
            else
            {
                max_radius = std::abs(h / (double)std::sin((CV_PI / 180) * angle));
            }
            radius = rng.uniform(max_radius/4, max_radius);
            pts.push_back(cv::Point(radius * std::cos((CV_PI / 180) * angle), radius * std::sin((CV_PI / 180) * angle)));
        }

        vpts[0] = pts;
        cv::fillPoly(img, vpts, cv::Scalar(255), cv::LineTypes::LINE_8, 0, center);
        // display bounded box 
        cv::rectangle(img, cv::Point(center.x - w, center.y - h), cv::Point(center.x + w, center.y + h), cv::Scalar(128));
    }
}

#endif // _CV_BLUR_PROCESS_H_
