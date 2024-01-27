// #include "common/trackApril.h"
#include "/home/cheol/git/apriltag/common/trackApril.h"

AprilTagDetector::AprilTagDetector() : tagFamily_(nullptr), tagDetector_(nullptr) {}

AprilTagDetector::~AprilTagDetector() {
    destroy();
}

bool AprilTagDetector::initialize() {
    // Choose tag family (e.g., tag36h11)
    tagFamily_ = tag36h11_create();

    if (tagFamily_ == nullptr) {
        std::cerr << "Failed to create tag family." << std::endl;
        return false;
    }

    // Create tag detector
    tagDetector_ = apriltag_detector_create();
    apriltag_detector_add_family(tagDetector_, tagFamily_);

    if (tagDetector_ == nullptr) {
        std::cerr << "Failed to create tag detector." << std::endl;
        destroy();
        return false;
    }

    // Set other parameters if needed
    // tagDetector_->quad_decimate = 2.0; // Example: Decimate input image by this factor
    // tagDetector_->quad_sigma = 0.0;    // Example: Apply low-pass blur to input
    // tagDetector_->nthreads = 1;        // Example: Use this many CPU threads

    return true;
}

void AprilTagDetector::detectAndDraw(cv::Mat& inputImage) {
    // Convert the input image to grayscale
    cv::Mat gray;
    cv::cvtColor(inputImage, gray, cv::COLOR_BGR2GRAY);

    // Make an image_u8_t header for the Mat data
    image_u8_t im = {gray.cols, gray.rows, gray.cols, gray.data};

    // Detect AprilTags
    zarray_t *detections = apriltag_detector_detect(tagDetector_, &im);

    // Draw detection outlines
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

int main(int argc, char* argv[]) {
    // Check if an image file is provided as a command-line argument
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

    // Create an instance of AprilTagDetector
    AprilTagDetector aprilTagDetector;

    // Initialize the detector
    if (!aprilTagDetector.initialize()) {
        std::cerr << "Error: Failed to initialize AprilTag detector." << std::endl;
        return -1;
    }

    // Detect and draw AprilTags in the image
    aprilTagDetector.detectAndDraw(inputImage);

    // Display the result
    cv::imshow("AprilTag Detection", inputImage);
    cv::waitKey(0);

    return 0;
}