#ifndef DETECT_APRILTAG_H
#define DETECT_APRILTAG_H

#include <opencv2/opencv.hpp>
#include "/home/cheol/git/apriltag/apriltag.h"

// #include <apriltag/apriltag.h>

class AprilTagDetector {
public:
    AprilTagDetector();
    ~AprilTagDetector();

    bool initialize();
    void detectAndDraw(cv::Mat& inputImage);

private:
    apriltag_family_t *tagFamily_;
    apriltag_detector_t *tagDetector_;

    void destroy();
};

#endif // DETECT_APRILTAG_H