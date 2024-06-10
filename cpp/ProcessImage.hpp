/* Chih-Yuan Yang 2024
This file should call a pose estimation algorithm.
*/
#include <string>
#include <vector>

#include "Pose.hpp"
using namespace std;

void process_image(string pose_model, 
    bool bShowRenderedImage, 
    bool bSaveTransmittedImage, 
    string save_to_directory);

//Chih-Yuan Yang: What is this function?
void render_poses_crop_regions(string pose_model, 
    string save_to_directory, 
    vector<string> file_list);

void process_save_frame_buffer_as_JPEG_images(bool bSaveTransmittedImage, std::string save_to_directory);
void SaveSamples(const string& save_to_directory, const string& timestamp, const vector<PoseRegion>& pairs, const cv::Mat& displayImage);