#include <string>
#include <samples/ocv_common.hpp>
#include "AnalyzedResults.pb.h"
#include <inference_engine.hpp>     //for yolov3, 2019/6/4 Do I still need it?
#include <opencv2/opencv.hpp>
#include <opencv2/core/utility.hpp>
//#include <opencv2/tracking.hpp>  this is for OpenCV 3.4.6
#include <opencv4/opencv2/tracking/tracker.hpp> 
#include <opencv2/highgui.hpp>
#include <gflags/gflags.h>
using namespace InferenceEngine;

void process_image(std::string pose_model, std::string detect_model, std::string extension, double threshold_t, 
                    double threshold_iou_t, bool bShowRenderedImage, bool bSaveTransmittedImage, std::string save_to_directory, double midPointsScoreThreshold);
void process_save_frame_buffer_as_JPEG_images(bool bSaveTransmittedImage, std::string save_to_directory);
void save_image_JPEG(char* data_, std::size_t length, std::string filename);
/*
void FrameToBlob(const cv::Mat &frame, InferRequest::Ptr &inferRequest, const std::string &inputName);

struct DetectionObject {
    int xmin, ymin, xmax, ymax, class_id;
    float confidence;

    DetectionObject(double x, double y, double h, double w, int class_id, float confidence, float h_scale, float w_scale) {
        this->xmin = static_cast<int>((x - w / 2) * w_scale);
        this->ymin = static_cast<int>((y - h / 2) * h_scale);
        this->xmax = static_cast<int>(this->xmin + w * w_scale);
        this->ymax = static_cast<int>(this->ymin + h * h_scale);
        this->class_id = class_id;
        this->confidence = confidence;
    }

    bool operator<(const DetectionObject &s2) const {
        return this->confidence < s2.confidence;
    }
};

void ParseYOLOV3Output(const CNNLayerPtr &layer, const Blob::Ptr &blob, const unsigned long resized_im_h,
                       const unsigned long resized_im_w, const unsigned long original_im_h,
                       const unsigned long original_im_w,
                       const double threshold, std::vector<DetectionObject> &objects);
double IntersectionOverUnion(const DetectionObject &box_1, const DetectionObject &box_2);
*/