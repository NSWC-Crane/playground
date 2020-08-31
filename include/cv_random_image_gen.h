#ifndef _CV_RANDOM_IMAGE_GEN_H_
#define _CV_RANDOM_IMAGE_GEN_H_

#include <cstring>

// OpenCV includes
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>


// ----------------------------------------------------------------------------------------
void generate_random_image(
    cv::Mat& img,
    cv::RNG& rng,
    long nr, 
    long nc, 
    unsigned int N, 
    double scale   
)
{
    unsigned int idx;

    // get the random background color
    cv::Scalar bg_color = cv::Scalar(rng.uniform(0, 256), rng.uniform(0, 256), rng.uniform(0, 256));

    // create the image with the random background color
    img = cv::Mat(nr, nc, CV_8UC3, bg_color);

    // create N shapes
    for (idx = 0; idx < N; ++idx)
    {

        // get the random color for the shape
        cv::Scalar C = cv::Scalar(rng.uniform(0, 256), rng.uniform(0, 256), rng.uniform(0, 256));

        // make sure the color picked is not the background color
        while (C == bg_color)
        {
            C = cv::Scalar(rng.uniform(0, 256), rng.uniform(0, 256), rng.uniform(0, 256));
        }

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

            cv::ellipse(img, cv::Point(x, y), cv::Size(r1, r2), a, 0.0, 360.0, C, -1, cv::LineTypes::LINE_8, 0);
            break;

        // filled rectangle
        case 1:

            h = std::floor(scale * rng.uniform(0, std::min(nr, nc)));
            w = std::floor(scale * rng.uniform(0, std::min(nr, nc)));
            a = rng.uniform(0.0, 360.0);

            // Create the rotated rectangle
            rect = cv::RotatedRect(cv::Point(x,y), cv::Size(w,h), a);

            // We take the edges that OpenCV calculated for us
            rect.points(vertices2f);

            // Convert them so we can use them in a fillConvexPoly
            for (int jdx = 0; jdx < 4; ++jdx) 
            {
                vertices[jdx] = vertices2f[jdx];
            }

            // Now we can fill the rotated rectangle with our specified color
            cv::fillConvexPoly(img, vertices, 4, C);
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
            cv::fillPoly(img, vpts, C, cv::LineTypes::LINE_8, 0);

            break;

        }   // end switch

    }   // end for loop

}   // end of generate_random_image


void generate_random_image(unsigned char*& img,
    long long seed,
    long nr,
    long nc,
    unsigned int N,
    double scale
)
{
    cv::RNG rng(seed);

    cv::Mat cv_img;

    generate_random_image(cv_img, rng, nr, nc, N, scale);

    //    memcpy((void*)data_params, &network_output(0, 0), network_output.size() * sizeof(float));
    img = new unsigned char[cv_img.total()*3];
    std::memcpy((void*)img, cv_img.ptr<unsigned char>(0), cv_img.total()*3);

}   // end of generate_random_image

#endif  // _CV_RANDOM_IMAGE_GEN_H_
