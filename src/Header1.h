#pragma once

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