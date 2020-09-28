#ifndef _CV_BLUR_PROCESS_H_
#define _CV_BLUR_PROCESS_H_

#include <cstdint>

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



//-----------------------------------------------------------------------------
void genrate_depthmap_set(uint16_t min_dm_value, uint16_t max_dm_value, std::vector<uint16_t> &dm_values)
{
    
    
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
    int xc, 
    int yc, 
    float kx1, 
    float kx2, 
    float ky1, 
    float ky2)
{
    dst = cv::Mat(src.rows, src.cols, CV_8U, cv::Scalar::all(255));

    int nr = src.rows;
    int nc = src.cols;

    float r;
    int xd, yd;
    float xn, yn, xd_f, yd_f;

    for (int x = 0; x < src.cols; x++)
    {
        for (int y = 0; y < src.rows; y++)
        {
            xn = (float)(2 * x - nc) / (float)nc;
            yn = (float)(2 * y - nr) / (float)nr;

            r = (x - xc) * (x - xc) + (y - yc) * (y - yc);
            r = sqrt(r);

            xd_f = xn * (1 + kx1 * r + kx2 * r * r);
            yd_f = yn * (1 + ky1 * r + ky2 * r * r);

            xd = (uint64_t)((xd_f + 1.0) * nc / 2);
            yd = (uint64_t)((yd_f + 1.0) * nr / 2);

            if (xd < src.cols && xd >= 0 && yd < src.rows && yd >= 0)
            {
                dst.at<uint8_t>(y, x) = src.at<uint8_t>(yd, xd);
            }

        } // end of inner for loop
    } // end of for loop

} // end of distortion


void distortion(cv::Mat src,
    cv::Mat& dst)
{
    int nr = src.rows;
    int nc = src.cols;

    // center of distortion
    int xc = nc / 2;
    int yc = nr / 2;

    // distortion coefficients
    float k1 = 0.0008;
    float k2 = 0.0;

    distortion(src, dst, xc, yc, k1, k2, k1, k2);
}


cv::Mat img, dst;

struct cv_distortion_coeffs
{
    int xc = 0;
    int yc = 0;
    int kx1 = 0;
    int kx2 = 0;
    int ky1 = 0;
    int ky2 = 0;
};


void trackbar_callback(int, void* user_data)
{
    cv_distortion_coeffs params = *((cv_distortion_coeffs*)user_data);

    float kx1 = (float)((params.kx1 + 1) / 100000.0);
    float kx2 = (float)((params.kx2 + 1) / 1000000.0);
    float ky1 = (float)((params.ky1 + 1) / 100000.0);
    float ky2 = (float)((params.ky2 + 1) / 1000000.0);

    std::cout << "center of distortion (" << params.xc << ", " << params.yc << ")" << std::endl;
    std::cout << "distortion coefficients (kx1, ky1) (" << kx1 << ", " << ky1 << ")" <<
        " -- (kx2, ky2) (" << kx2 << ", " << ky2 << ")" << std::endl << std::endl;

    distortion(img, dst, (int)params.xc, (int)params.yc, kx1, kx2, ky1, ky2);
    cv::imshow("Checkbard Image", dst);
}

void button_callback(int state, void* user_data) {
    if (state)
    {
        cv_distortion_coeffs params = *((cv_distortion_coeffs*)user_data);

        std::string file_name = "checkboard_img-" + std::to_string(params.xc) + "-" + std::to_string(params.yc) + ".jpg";
        
        cv::imwrite("../images/"+file_name, dst);
    }
}


void load_gui(std::string file_path)
{
    img = cv::imread(file_path, cv::IMREAD_COLOR);
    
    int coefficient_max = 100;
    int xc_max = img.cols;
    int yc_max = img.rows;

    cv_distortion_coeffs user_data;
    user_data.xc = img.cols / 2;
    user_data.yc = img.rows / 2;

    std::string parameter_window = "Parameter Options";
    cv::namedWindow(parameter_window, cv::WINDOW_AUTOSIZE); // Create Window

    // xc and yc trackbar
    char TrackbarName[50];
    sprintf(TrackbarName, "Xc - %d", xc_max);
    cv::createTrackbar(TrackbarName, parameter_window, &user_data.xc, xc_max, trackbar_callback, &user_data);
    sprintf(TrackbarName, "Yc - %d", yc_max);
    cv::createTrackbar(TrackbarName, parameter_window, &user_data.yc, yc_max, trackbar_callback, &user_data);

    // kx1 and kx2 trackbar
    sprintf(TrackbarName, "Kx1 - %d", coefficient_max);
    cv::createTrackbar(TrackbarName, parameter_window, &user_data.kx1, coefficient_max, trackbar_callback, &user_data);
    sprintf(TrackbarName, "Kx2 - %d", coefficient_max);
    cv::createTrackbar(TrackbarName, parameter_window, &user_data.kx2, coefficient_max, trackbar_callback, &user_data);

    // ky1 and ky2 trackbar
    sprintf(TrackbarName, "Ky1 - %d", coefficient_max);
    cv::createTrackbar(TrackbarName, parameter_window, &user_data.ky1, coefficient_max, trackbar_callback, &user_data);
    sprintf(TrackbarName, "Ky2 - %d", coefficient_max);
    cv::createTrackbar(TrackbarName, parameter_window, &user_data.ky2, coefficient_max, trackbar_callback, &user_data);

    // create button to save images
    //cv::createButton("Save image", button_callback, NULL);

    cv::namedWindow("Checkbard Image", cv::WINDOW_AUTOSIZE); // Create Window
    trackbar_callback(0, &user_data);
    cv::waitKey(0);

    // save new image
    cv::imwrite("../images/checkerboard_pincushion.jpg", dst);
}

#endif // _CV_BLUR_PROCESS_H_
