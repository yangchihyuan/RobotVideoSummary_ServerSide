#include <fstream>
#include <mutex>
#include <thread>
#include "human_pose_estimator.hpp"
#include "render_human_pose.hpp"

#include "ProcessImage.hpp"
#include <vector>
#include <numeric>      // std::iota
#include "JPEG.hpp"
#include "AnalyzedResults.pb.h"
#include <gflags/gflags.h>
#include "Logger.hpp"
#include "utility_TimeRecorder.hpp"
#include "ReID.hpp"


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

void process_image(std::string pose_model, 
    bool bShowRenderedImage, 
    bool bSaveTransmittedImage, 
    string save_to_directory, 
    double midPointsScoreThreshold,
    PSE id_feature_generator)
{
    HumanPoseEstimator estimator(pose_model, "CPU", false, (float)midPointsScoreThreshold); //the 3rd argument is per-layer performance report
    unique_ptr<Session> psession;
    psession.reset(NewSession(SessionOptions()));
    Status session_create_status = psession->Create(id_feature_generator.graph_def);
    if (!session_create_status.ok())
    {
        Logger("Session creation fail.");
    }
    ReID reid;

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
 
            ImageAnalyzedResults::ReportData report_data;
            report_data.set_key(data_, key_length);
            std::vector<char> JPEG_Data(data_ + key_info.length() + str_JPEG_length.length() + 2, data_ + length -1);
            bool bCorrectlyDecoded = false;
            cv::Mat inputImage;
            try{
                inputImage = cv::imdecode(JPEG_Data, cv::IMREAD_COLOR); //check this result. The image may be corrupt.
                if( inputImage.data )
                    bCorrectlyDecoded = true;
            }
            catch(std::exception &e)
            {
                std::cout << "Received JPEG frame are corrupt although the signature is correct." << std::endl;
            }
            if( bCorrectlyDecoded)
            {
                if(bSaveTransmittedImage)
                {
                    std::string filename = save_to_directory + "/" + str_timestamp + ".jpg";
                    save_image_JPEG(data_ + key_info.length() + str_JPEG_length.length() + 2, JPEG_length , filename);
                }

                cv::Mat displayImage = inputImage.clone();

                //std::thread thread_yolo(yolo_detect, inputImage, &body_coord);
                //std::thread thread_charades_squeezenet(action_recognition_charades_webcam,inputImage, &body_coord);
                TimeRecorder Recorder_OpenVINO_estimator;
                std::vector<HumanPose> poses = estimator.estimate(inputImage );
                Recorder_OpenVINO_estimator.Stop();
                Logger("OpenVINO pose estimator time (millisecond):" + Recorder_OpenVINO_estimator.GetDurationString());

                //ReID feature computation
                TimeRecorder Recorder_ReID;
                vector<PoseRegion> regions = CropRegionsFromPoses(inputImage, poses);
                vector<array<float,1536>> ReID_features = ConvertPoseRegionsToReIDFeatures(regions, id_feature_generator, psession);
                Recorder_ReID.Stop();
                Logger("PSE Process time (millisecond):" + Recorder_ReID.GetDurationString());

                //add positive example
                if( regions.size() == 1 && regions.size() == 1)
                {
                    reid.AddSample(ReID_features[0], 0);        //assume id==0 is the target
                    Logger("reid sample number: " + to_string(reid.GetSampleNumber()));
                }
                else if(regions.size() > 1 && reid.HaveSufficientSamples())
                {
                    //evaluate ReID feature similarity
                    vector<int> index_vector = reid.SortByFeatureSimilarity(ReID_features);

                    //re-arrange the order of poses
                    int index_of_most_similar_region = index_vector[0];
                    int index_of_most_similar_pose = regions[index_of_most_similar_region].index_in_poses;
                    HumanPose selected_pose = poses[index_of_most_similar_pose];
                    poses.erase(poses.begin()+index_of_most_similar_pose);
                    poses.insert(poses.begin(), selected_pose);
                }
/*                
                Logger("number of samples: " + to_string(reid.GetSampleNumber()));
                Logger("index_vector");
                for(unsigned int i = 0 ; i<index_vector.size() ; i++)
                {
                    Logger(to_string(index_vector[i]));
                }
*/
                
                //Sort OpenVINO's openpose results by the distance between neck to nose, eyes, and ears
                //compute the distnaces
//                vector<int> index_vector = SortPosesByNeckToNose(poses);

                //Convert openpose results to our own format
                report_data.set_openpose_cnt(poses.size());
                for( unsigned int idx = 0; idx < poses.size(); idx++ )
                {
                    HumanPose pose = poses[idx];
                    std::string temp;
                    for( auto keypoint : pose.keypoints)
                    {
                        if(keypoint.x == -1 && keypoint.y == -1)
                        {
                            temp += std::to_string(0.0f) + " ";
                            temp += std::to_string(0.0f) + " ";
                            temp += std::to_string(0.0f) + " " + '\n';
                        }
                        else
                        {
                            temp += std::to_string(keypoint.x) + " ";
                            temp += std::to_string(keypoint.y) + " ";
                            temp += std::to_string(1.0f) + " " + '\n';
                        }
                    }
                    report_data.add_openpose_coord(temp);
                }

                strcpy(str_results, report_data.DebugString().c_str());     //How to transmit a protobuf object?

                if( bShowRenderedImage )
                {
                    renderHumanPose(poses, displayImage);
                    cv::imshow("opencv result", displayImage);
                    cv::waitKey(1);
                }

                renderHumanPose(poses, displayImage);
//                SaveSamples(save_to_directory, str_timestamp, regions, displayImage);

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

void SaveSamples(const string& save_to_directory, const string& timestamp, const vector<PoseRegion>& regions, const Mat& displayImage)
{
    string save_file_name = save_to_directory + "/" + timestamp + "_render.jpg";
    imwrite(save_file_name, displayImage);
    for( unsigned int i=0; i< regions.size(); i++)
    {
        save_file_name = save_to_directory + "/" + timestamp + "_region" + to_string(i) + ".jpg";
        imwrite(save_file_name, regions[i].mat);
    }
}


/*
void yolo_detect(cv::Mat img_raw, ImageAnalyzedResults::ReportData *body_coord)
{
    // yolo_names, yolo_alphabet, yolo_net as global variables
    //const float thresh = 0.5, nms = 0.45;
    const float thresh = 0.4, nms = 0.45;
    double time = what_time_is_it_now();
    // Convert CV::Mat to darknet image
    // Refer to darknet/src/image.c

    // (1) Initialize Iplimage ( 01 ms)

    // Using IplImage is fast! more than three times faster.
    // But there are severe caveats:
    // First is (random) Segmentation Fault without any explicit reason
    // (Maybe due to inner data formatting problem)
    // Second is Image Corruption
    // Therefore, it would be safe to use cv::Mat rather than obsolete IplImage
    // in terms of robustness.
    // IplImage* img_temp = new IplImage(cv::imdecode(JPEG_Data, cv::IMREAD_COLOR));
    // IplImage* resized = cvCreateImage(cvSize(416, 311), img_temp->depth, img_temp->nChannels);
    // cvResize(img_temp, resized);

    //    std::vector<char> JPEG_Data(pFramedata, pFramedata + JPEG_data_size);
    //    cv::Mat img_raw = cv::imdecode(JPEG_Data, cv::IMREAD_COLOR);
    cv::Mat img_downscale;
    cv::resize(img_raw, img_downscale, cv::Size(416, 312), 0, 0, cv::INTER_CUBIC);

    IplImage resized_orig = img_downscale;
    IplImage *resized = &resized_orig;
    //416x312 was set in accordance with 640x480, original zenbo camera resolution
    //for better code, it is better not to use magic number like below
    //note that computation is way more efficient for uchar than float
    //thus, letterbox_image(im, yolo_net->w, yolo_net->h) is replaced with below

    unsigned char *data = (unsigned char *)resized->imageData;
    int h = resized->height, w = resized->width, c = resized->nChannels;
    int step = resized->widthStep;

    // (2) Converting image ( 03 ms)
    image im = make_image(yolo_net->w, yolo_net->h, c);
    // Note that darknet color channel is like BGR, not RGB.
    // instead of [c*w*h + i*w + j] as given in the original darknet code,
    // modify c into 'c-1-k' so that the channel mapping is inverted.
    for (int i = 0; i < h; ++i)
        for (int k = 0; k < c; ++k)
            for (int j = 0; j < w; ++j)
                im.data[k * w * w + (i + 52) * w + j] = data[i * step + j * c + (c - 1 - k)] / 255.;

    for (int i = 0; i < 52; ++i)
    {
        for (int k = 0; k < c; ++k)
        {
            for (int j = 0; j < w; ++j)
            {
                im.data[k * w * w + i * w + j] = 0.5;
                im.data[k * w * w + (i + 364) * w + j] = 0.5;
            }
        }
    }

    // (3) Resizing image ( 30 ms)
    //image sized = letterbox_image(im, yolo_net->w, yolo_net->h);
    //after optimization, the aggregated classification time was reduced from 54ms to 22ms
    image sized = im;
    layer l = yolo_net->layers[yolo_net->n - 1];

    // (4) Prediction ( 17 ms)
    float *X = sized.data;
    float *out = network_predict(yolo_net, X);
    int nboxes = 0;
    // (5) Counting & Drawing Bboxes ( 03 ms)
    detection *dets = get_network_boxes(yolo_net, im.w, im.h, thresh, thresh, 0, 1, &nboxes);
    if (nms)
        do_nms_sort(dets, nboxes, l.classes, nms);
    //draw_detections(im, dets, nboxes, thresh, yolo_names, yolo_alphabet, l.classes);

    // from image.c:239 draw_detections
    // names[0], dets[i].prob[0] indicates person
    char string_for_point[1024];
    int person_cnt = 0;
    int TV_cnt = 0;
    for (int i = 0; i < nboxes; ++i)
    {
        if (dets[i].prob[0] > FLAGS_YOLO_person_threshold)  //Detect person
        {
            //printf("person detected");
            box b = dets[i].bbox;
            //printf("\t x:%f y:%f w:%f h:%f\n", b.x, b.y, b.w, b.h);
            sprintf(string_for_point, "%f, %f, %f, %f\n", b.x, b.y, b.w, b.h);
            body_coord->add_yolo_coord_person(string_for_point);
            person_cnt++;
        }
        if (dets[i].prob[62] > FLAGS_YOLO_TV_threshold)          //Detect TVMonitor
        {
//            printf("TV monitor detected\n");
            box b = dets[i].bbox;
            //printf("\t x:%f y:%f w:%f h:%f\n", b.x, b.y, b.w, b.h);
            sprintf(string_for_point, "%f, %f, %f, %f\n", b.x, b.y, b.w, b.h);
            body_coord->add_yolo_coord_tv(string_for_point);
            TV_cnt++;
        }

    }
    body_coord->set_yolo_cnt_person(person_cnt);
    body_coord->set_yolo_cnt_tv(TV_cnt);

    free_detections(dets, nboxes);

    //save_image(im, "pred");

    //free_image(sized);
    free_image(im);
    //cvReleaseImage(&img_temp);

    //printf("Predicted in %f seconds. \n", what_time_is_it_now() - time);
}
*/