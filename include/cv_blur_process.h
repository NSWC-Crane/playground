#ifndef _CV_BLUR_PROCESS_H_
#define _CV_BLUR_PROCESS_H_



// OpenCV includes
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/calib3d.hpp>

#include <cv_random_image_gen.h>

#include <math.h>

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


//void distortion(cv::Mat src,
//    cv::Mat& dst)
//{
//    dst = cv::Mat(src.rows, src.cols, CV_8U, cv::Scalar::all(255));
//
//    cv::Mat cameraMatrix = cv::Mat::eye(cv::Size(3, 3), CV_32F);
//    cameraMatrix.at<float>(0, 0) = 40.0;
//    cameraMatrix.at<float>(1, 1) = 40.0;
//    cameraMatrix.at<float>(0, 2) = src.rows / 2.f;
//    cameraMatrix.at<float>(1, 2) = src.cols / 2.f;
//    
//    cv::Mat distortion_coef(1, 4, CV_32F);
//    distortion_coef.at<float>(0) = -0.001;
//    distortion_coef.at<float>(1) = 0;
//    distortion_coef.at<float>(2) = 0;
//    distortion_coef.at<float>(3) = 0;
//
//    cv::fisheye::distortPoints(src, dst, cameraMatrix, distortion_coef);
//}


// https://www.mathworks.com/help/vision/ug/camera-calibration.html
// http://marcodiiga.github.io/radial-lens-undistortion-filtering

void distortion(cv::Mat src,
    cv::Mat& dst,
    int start,
    int end)
{
    dst = cv::Mat(src.rows, src.cols, CV_8U, cv::Scalar::all(255));

    int nr = src.rows;
    int nc = src.cols;

    // center of distortion
    int xc = nr / 2;
    int yc = nc / 2;

    // distortion coefficients
    float k1 = -0.0009;
    float k2 = -0.00000008;

    float r;
    int xd, yd;
    float xn, yn, xd_f, yd_f;

    for (int x = start; x < end; x++)
    {
        for (int y = 0; y < src.cols; y++)
        {
            xn = (float)(2 * x - nr) / nr;
            yn = (float)(2 * y - nc) / nc;

            r = (x - xc) * (x - xc) + (y - yc) * (y - yc);
            r = sqrt(r);

            xd_f = xn * (1 + k1 * r + k2 * r * r);
            yd_f = yn * (1 + k1 * r + k2 * r * r);

            xd = (xd_f + 1.0) * nr / 2;
            yd = (yd_f + 1.0) * nc / 2;

            if (xd < src.rows && xd >= 0 && yd < src.cols && yd >= 0)
            {
                dst.at<uint8_t>(xd, yd) = src.at<uint8_t>(x, y);
            }

        } // end of inner for loop
    } // end of for loop

} // end of distortion



#endif // _CV_BLUR_PROCESS_H_