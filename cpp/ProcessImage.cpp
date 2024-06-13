#include <fstream>
#include <mutex>
#include <thread>
#include "human_pose_estimator.hpp"

#include "ProcessImage.hpp"
#include <numeric>      // std::iota
#include "JPEG.hpp"
#include "AnalyzedResults.pb.h"
#include <gflags/gflags.h>   //Why do I need gflags here?
#include "utility_TimeRecorder.hpp"
#include "utility_directory.hpp"
#include "utility_string.hpp"
#include "utility_csv.hpp"
#include "Pose.hpp"

using namespace human_pose_estimation;
using namespace cv;

extern int status_frame_buffer1;
extern int status_frame_buffer2;
extern char *frame_buffer1;
extern char *frame_buffer2;
extern int frame_buffer1_length;
extern int frame_buffer2_length;
extern std::mutex gMutex;
extern std::mutex gMutex_save_JPEG;
extern std::mutex gMutex_send_results;
extern char str_results[122880];

std::string save_to_directory;

//Something wrong here. I did not check the content. I may save corrupt images.
void process_save_frame_buffer_as_JPEG_images(bool bSaveTransmittedImage, std::string save_to_directory)
{
    if(bSaveTransmittedImage)
    while(true)
    {
        if( status_frame_buffer2 == 1)
        {
            char *data_ = frame_buffer2;
            std::string key_info(data_);
            std::string str_JPEG_length(data_+ key_info.length() + 1);
            int JPEG_length = frame_buffer2_length - (key_info.length() + str_JPEG_length.length() + 2 );
            save_image_JPEG( data_ + key_info.length() + str_JPEG_length.length() + 2, JPEG_length , save_to_directory + "/" + key_info.substr(0,13) + ".jpg");
            status_frame_buffer2 = 0;
        }
        else
        {
            gMutex_save_JPEG.lock();
        }
    }
}

typedef std::pair<float,int> mypair;
bool comparator ( const mypair& l, const mypair& r)
   { return l.first < r.first; }

string ConvertPoseToString(HumanPose pose)
{
    string temp;
    for( auto keypoint : pose.keypoints)
    {
        if(keypoint.x == -1 && keypoint.y == -1)
        {
            temp += to_string(0.0f) + " ";
            temp += to_string(0.0f) + " ";
            temp += to_string(0.0f) + " " + '\n';
        }
        else
        {
            temp += to_string(keypoint.x) + " ";
            temp += to_string(keypoint.y) + " ";
            temp += to_string(1.0f) + " " + '\n';
        }
    }
    return temp;
}

//Chih-Yuan Yang: This is the function used by a thread.
void process_image(std::string pose_model_path, 
    bool bShowRenderedImage, 
    bool bSaveTransmittedImage, 
    string save_to_directory)
{
    HumanPoseEstimator estimator(pose_model_path);
    string raw_images_directory = save_to_directory + "/raw_images";
    if(bSaveTransmittedImage)
        CreateDirectory(raw_images_directory);

    while(true)
    {
        if( status_frame_buffer1 == 1)
        {
            char *data_ = frame_buffer1;
            int length = frame_buffer1_length;
            string key_info(data_);
            string str_timestamp = key_info.substr(0,13);

            int key_length = key_info.length();  
            string str_JPEG_length(data_+ key_info.length() + 1);
            //it appears that the stio() still can crash. I don't know why. Use try to protect it.
            int JPEG_length = 0;
            try{
                JPEG_length = stoi(str_JPEG_length);
            }
            catch( exception &e){
            }
 
            //2024/6/8 Report result back to Zenbo so it can take actions.
            ImageAnalyzedResults::ReportData report_data;
            report_data.set_key(data_, key_length);
            vector<char> JPEG_Data(data_ + key_info.length() + str_JPEG_length.length() + 2, data_ + length -1);
            bool bCorrectlyDecoded = false;
            Mat inputImage;
            try{
                //Chih-Yuan Yang: imdecode is an OpenCV function. Because I use using namespace cv, the linker
                //find the imdecode() function and the IMREAD_COLOR tag.
                
                inputImage = imdecode(JPEG_Data, IMREAD_COLOR); //check this result. The image may be corrupt.
                if( inputImage.data )
                    bCorrectlyDecoded = true;
            }
            catch(exception &e)
            {
                cout << "Received JPEG frame are corrupt although the signature is correct." << std::endl;
            }

            if( bCorrectlyDecoded)
            {
                if(bSaveTransmittedImage)
                {
                    string filename = raw_images_directory + "/" + str_timestamp + ".jpg";
                    save_image_JPEG(data_ + key_info.length() + str_JPEG_length.length() + 2, JPEG_length , filename);
                }

                Mat displayImage = inputImage.clone();


                vector<HumanPose> poses = estimator.estimate(inputImage );
                //This function is written in the Pose.cpp
                poses = SortPosesByHeight(poses);
                

                //Convert openpose results to our own format
                report_data.set_openpose_cnt(poses.size());
                for( unsigned int idx = 0; idx < poses.size(); idx++ )
                {
                    HumanPose pose = poses[idx];
                    string temp = ConvertPoseToString(pose);
                    report_data.add_openpose_coord(temp);
                }

                strcpy(str_results, report_data.DebugString().c_str());     //How to transmit a protobuf object?

                status_frame_buffer1 = 0;
                gMutex_send_results.unlock();
            }
        }
        else
        {
            gMutex.lock();
        }
    }
}
