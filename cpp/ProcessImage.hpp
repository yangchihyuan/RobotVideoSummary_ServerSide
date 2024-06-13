/* Chih-Yuan Yang 2024
*/
#include <string>
#include <vector>

//#include "Pose.hpp"
using namespace std;

void process_image(string pose_model, 
    bool bShowRenderedImage, 
    bool bSaveTransmittedImage, 
    string save_to_directory);

void process_save_frame_buffer_as_JPEG_images(bool bSaveTransmittedImage, std::string save_to_directory);
