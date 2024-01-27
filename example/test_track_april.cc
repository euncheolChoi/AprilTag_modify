#include <iostream>
#include <opencv4/opencv2/opencv.hpp>
#include <apriltag/apriltag.h>
// #include "/usr/local/include/apriltag/tag36h11.h"
#include "apriltag/tag36h11.h"
#include "apriltag/tag25h9.h"


class AprilTagDetector
{

public:
    AprilTagDetector(){};
    ~AprilTagDetector(){};

    bool initialize();
    void detectAndDraw(cv::Mat &inputImage);
    void destroy();

private:
    apriltag_family_t *tagFamily_;
    apriltag_detector_t *tagDetector_;
};

void AprilTagDetector::destroy() {
    if (tagDetector_ != nullptr) {
        apriltag_detector_destroy(tagDetector_);
        tagDetector_ = nullptr;
    }

    if (tagFamily_ != nullptr) {
        if (!strcmp(tagFamily_->name, "tag36h11")) {
            tag36h11_destroy(tagFamily_);
        } else if (!strcmp(tagFamily_->name, "tag25h9")) {
            tag25h9_destroy(tagFamily_);
        } // Add other tag families as needed
        tagFamily_ = nullptr;
    }
}


bool AprilTagDetector::initialize() {

    // 특정 패턴 및 비트의 배열로 정의된 2D 매트릭스 -> 이러한 특징에 맞는 Apriltag를 식별할 수 있음 
    // tag36h11_create() : 이러한 태그의 정보를 담는 구조체에 대한 포인터를 반환
    tagFamily_ = tag36h11_create();     
    if (tagFamily_ == nullptr) {
        std::cerr << "Failed to create tag family." << std::endl;
        return false;
    }

    // detector 정의
    tagDetector_ = apriltag_detector_create();
    apriltag_detector_add_family(tagDetector_, tagFamily_);

    if (tagDetector_ == nullptr) {
        std::cerr << "Failed to create tag detector." << std::endl;
        destroy();
        return false;
    }

    return true;
};

void AprilTagDetector::detectAndDraw(cv::Mat& inputImage) {
    // Convert the input image to grayscale
    cv::Mat gray;
    cv::cvtColor(inputImage, gray, cv::COLOR_BGR2GRAY);

    // Make an image_u8_t header for the Mat data
    image_u8_t im = {gray.cols, gray.rows, gray.cols, gray.data};
    std::cout << im << std::endl;

    // Detect AprilTags -> 이 함수를 사용하면, 코너점 또한 zarray 타입 변수를 가리키는 포인터 변수 detections에 저장되어 있음
    zarray_t *detections = apriltag_detector_detect(tagDetector_, &im);

    // Draw detection outlines
    // TODO : detection된 객체의 갯수만큼 순회 
    for (int i = 0; i < zarray_size(detections); i++) {
        apriltag_detection_t *det;
        zarray_get(detections, i, &det);
        line(inputImage, cv::Point(det->p[0][0], det->p[0][1]),
             cv::Point(det->p[1][0], det->p[1][1]), cv::Scalar(0, 255, 0), 2);
        line(inputImage, cv::Point(det->p[0][0], det->p[0][1]),
             cv::Point(det->p[3][0], det->p[3][1]), cv::Scalar(0, 0, 255), 2);
        line(inputImage, cv::Point(det->p[1][0], det->p[1][1]),
             cv::Point(det->p[2][0], det->p[2][1]), cv::Scalar(255, 0, 0), 2);
        line(inputImage, cv::Point(det->p[2][0], det->p[2][1]),
             cv::Point(det->p[3][0], det->p[3][1]), cv::Scalar(255, 0, 0), 2);


        // Draw corners
        for (int j = 0; j < 4; j++) {
            cv::circle(inputImage, cv::Point(det->p[j][0], det->p[j][1]), 10, cv::Scalar(255, 255, 255), -1);
        }

        // Display tag ID
        std::stringstream ss;
        ss << det->id;
        cv::String text = ss.str();
        int fontface = cv::FONT_HERSHEY_SCRIPT_SIMPLEX;
        double fontscale = 1.0;
        int baseline;
        cv::Size textsize = cv::getTextSize(text, fontface, fontscale, 2, &baseline);
        cv::putText(inputImage, text, cv::Point(det->c[0] - textsize.width / 2,
                   det->c[1] + textsize.height / 2), fontface, fontscale, cv::Scalar(255, 153, 0), 2);

    }

    // Release memory
    apriltag_detections_destroy(detections);
}

// void AprilTagDetector::detectAndDraw(cv::Mat& inputImage) {
//     // Convert the input image to grayscale
//     cv::Mat gray;
//     cv::cvtColor(inputImage, gray, cv::COLOR_BGR2GRAY);

//     // Make an image_u8_t header for the Mat data
//     image_u8_t im = {gray.cols, gray.rows, gray.cols, gray.data};
//     std::cout << im << std::endl;

//     // Detect AprilTags -> 이 함수를 사용하면, 코너점 또한 zarray 타입 변수를 가리키는 포인터 변수 detections에 저장되어 있음
//     zarray_t *detections = apriltag_detector_detect(tagDetector_, &im);

//     // Draw detection outlines
//     // TODO : detection된 객체의 갯수만큼 순회 
//     for (int i = 0; i < zarray_size(detections); i++) {
//         apriltag_detection_t *det;
//         zarray_get(detections, i, &det);
//         line(inputImage, cv::Point(det->p[0][0], det->p[0][1]),
//              cv::Point(det->p[1][0], det->p[1][1]), cv::Scalar(0, 255, 0), 2);
//         line(inputImage, cv::Point(det->p[0][0], det->p[0][1]),
//              cv::Point(det->p[3][0], det->p[3][1]), cv::Scalar(0, 0, 255), 2);
//         line(inputImage, cv::Point(det->p[1][0], det->p[1][1]),
//              cv::Point(det->p[2][0], det->p[2][1]), cv::Scalar(255, 0, 0), 2);
//         line(inputImage, cv::Point(det->p[2][0], det->p[2][1]),
//              cv::Point(det->p[3][0], det->p[3][1]), cv::Scalar(255, 0, 0), 2);


//         // Draw corners
//         for (int j = 0; j < 4; j++) {
//             cv::circle(inputImage, cv::Point(det->p[j][0], det->p[j][1]), 10, cv::Scalar(255, 255, 255), -1);
//         }

//         // Display tag ID
//         std::stringstream ss;
//         ss << det->id;
//         cv::String text = ss.str();
//         int fontface = cv::FONT_HERSHEY_SCRIPT_SIMPLEX;
//         double fontscale = 1.0;
//         int baseline;
//         cv::Size textsize = cv::getTextSize(text, fontface, fontscale, 2, &baseline);
//         cv::putText(inputImage, text, cv::Point(det->c[0] - textsize.width / 2,
//                    det->c[1] + textsize.height / 2), fontface, fontscale, cv::Scalar(255, 153, 0), 2);
//         std::cout << i << std::endl;
//     }

//     // Release memory
//     apriltag_detections_destroy(detections);
// }


int main(int argc, char* argv[]){
    
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " <image_file_path>" << std::endl;
        return -1;
    }

    // Read the input image
    cv::Mat inputImage = cv::imread(argv[1]);
    if (inputImage.empty()) {
        std::cerr << "Error: Couldn't open the image file." << std::endl;
        return -1;
    }
    else{
        cv::Mat resizedImg;
        cv::resize(inputImage, resizedImg, cv::Size(640, 480));
        cv::imshow("test_april_image", resizedImg);

        // Create an instance of AprilTagDetector
        AprilTagDetector aprilTagDetector;

        // Initialize the detector
        if (!aprilTagDetector.initialize()) 
        {
            std::cerr << "Error: Failed to initialize AprilTag detector." << std::endl;
            return -1;
        }

        // Detect and draw AprilTags in the image
        aprilTagDetector.detectAndDraw(resizedImg);
        // Display the result
        cv::imshow("AprilTag Detection", resizedImg);
        cv::waitKey(0);
    }

    return 0;

};