#include <string>
#include "Pose.hpp"
#include "PoseSensitiveEmbedding.hpp"

using namespace InferenceEngine;
using namespace std;

void process_image(string pose_model, 
    bool bShowRenderedImage, 
    bool bSaveTransmittedImage, 
    string save_to_directory, 
    double midPointsScoreThreshold,
    PSE id_feature_generator);

void process_image_offline(string pose_model, 
    bool bShowRenderedImage, 
    bool bSaveTransmittedImage, 
    string save_to_directory, 
    double midPointsScoreThreshold,
    PSE id_feature_generator,
    string output_directory,
    vector<string> file_list);

void convert_regions_to_features(string pose_model, 
    bool bShowRenderedImage, 
    bool bSaveTransmittedImage, 
    string image_directory, 
    double midPointsScoreThreshold,
    PSE id_feature_generator,
    string output_directory,
    vector<string> file_list);


void process_save_frame_buffer_as_JPEG_images(bool bSaveTransmittedImage, std::string save_to_directory);
void SaveSamples(const string& save_to_directory, const string& timestamp, const vector<PoseRegion>& pairs, const cv::Mat& displayImage);