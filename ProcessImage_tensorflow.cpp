#include <fstream>
#include <mutex>
#include <thread>
#include <inference_engine.hpp>
#include "human_pose_estimator.hpp"
#include "render_human_pose.hpp"

#include "ProcessImage.hpp"

// Header files required by Tensorflow
#include <cmath>
#include <chrono>
#include "tensorflow/core/platform/init_main.h"
#include "tensorflow/core/util/command_line_flags.h"
#include <tensorflow/core/platform/env.h>
#include <tensorflow/core/public/session.h>
#include "tensorflow/core/framework/graph.pb.h"
#include "tensorflow/core/framework/tensor.h"
#include "tensorflow/core/graph/default_device.h"
#include "tensorflow/core/graph/graph_def_builder.h"
#include "tensorflow/core/lib/io/path.h"
#include <tensorflow/core/protobuf/meta_graph.pb.h>

using namespace human_pose_estimation;
//using namespace InferenceEngine;        //for yolov3 InferencePlugin
using namespace cv;
using namespace std;
using namespace tensorflow;

DECLARE_bool(Verbose);

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



//save JPEG data into a file
void save_image_JPEG(char* data_, std::size_t length, std::string filename)
{
    std::ofstream outfile (filename.c_str(),std::ofstream::binary);
    outfile.write(data_, length);
    outfile.close();
}


//Something wrong here. I did not check the content. I may save corrupt images.
void process_save_frame_buffer_as_JPEG_images(bool bSaveTransmittedImage, std::string save_to_directory)
{
    if(bSaveTransmittedImage)
    while(true)
    {
        if( status_frame_buffer2 == 1)
        {
            char *data_ = frame_buffer2;
            int length = frame_buffer2_length;
            std::string key_info(data_);
            int key_length = key_info.length();
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

void process_image(std::string pose_model, std::string detect_model, std::string extension, double threshold_t, 
    double threshold_iou_t, bool bShowRenderedImage, bool bSaveTransmittedImage, std::string save_to_directory, double midPointsScoreThreshold)
{
    HumanPoseEstimator estimator(pose_model, "CPU", false, (float)midPointsScoreThreshold); //the 3rd argument is per-layer performance report
    // declares all required variables
    
    Rect2d roi;
    // create a tracker object
    //Ptr<Tracker> tracker = TrackerMOSSE::create();
    //Ptr<Tracker> tracker = TrackerTLD::create();
    std::string str_timestamp_initialize_tracker;
    bool b_tracker_initialized = false;
    Ptr<Tracker> tracker;


    bool b_run_reid = true;
    if( b_run_reid )
    {
        std::unique_ptr<tensorflow::Session> tf_session;

        //load the tensorflow model
        const string pathToGraph = "/4t/yangchihyuan/PSE_tfModels/model.ckpt-104801.meta";
        const string checkpointPath = "/4t/yangchihyuan/PSE_tfModels";    

        auto session = NewSession(SessionOptions());
        if (session == nullptr) {
            std::cout << "Could not create Tensorflow session." << std::endl;
            throw runtime_error("Could not create Tensorflow session.");
        }
        else
        {
            std::cout << "NewSession ok." << std::endl;
        }
        
        Status status;

        // Read in the protobuf graph we exported
        MetaGraphDef meta_graph_def;
        status = ReadBinaryProto(Env::Default(), pathToGraph, &meta_graph_def);
        if (!status.ok()) {
            std::cout << "Error reading graph definition from" << std::endl;
            throw runtime_error("Error reading graph definition from " + pathToGraph + ": " + status.ToString());
        }
        else
        {
            std::cout << "ReadBinaryProto ok." << std::endl;
        }

        // Add the graph to the session
        //status = session->Create(meta_graph_def.graph_def());
        status = session->Create(meta_graph_def.graph_def());
        if (!status.ok()) {
            throw runtime_error("Error creating graph: " + status.ToString());
        }
        else
        {
            std::cout << "Create session ok." << std::endl;
        }
        
        // Load weight
        Tensor checkpointPathTensor(DT_STRING, TensorShape());
        checkpointPathTensor.scalar<std::string>()() = checkpointPath;
        status = session->Run(
                {{ meta_graph_def.saver_def().filename_tensor_name(), checkpointPathTensor },},
                {},
                {meta_graph_def.saver_def().restore_op_name()},
                nullptr);
        if (!status.ok()) {
            std::cout << status.ToString() << std::endl;
        } else {
            std::cout << "Load Model "<<checkpointPath<<" successfully" << std::endl;
        }    
    }


    while(true)
    {
        if( status_frame_buffer1 == 1)
        {
//            system_clock::time_point time_detection_start = system_clock::now();
            char *data_ = frame_buffer1;
            int length = frame_buffer1_length;
            std::string key_info(data_);
            std::string str_timestamp = key_info.substr(0,13);

            int key_length = key_info.length();  
            std::string str_JPEG_length(data_+ key_info.length() + 1);
            //it appears that the stio() still can crash. I don't know why. Use try to protect it.
            int JPEG_length = 0;
            try{
                JPEG_length = std::stoi(str_JPEG_length);
            }
            catch( std::exception &e){
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

                const size_t width  = (size_t) inputImage.cols;
                const size_t height = (size_t) inputImage.rows;

                cv::Mat displayImage = inputImage.clone();

                //std::thread thread_yolo(yolo_detect, inputImage, &body_coord);
                //std::thread thread_charades_squeezenet(action_recognition_charades_webcam,inputImage, &body_coord);
                std::vector<HumanPose> poses = estimator.estimate(inputImage);

                if( poses.size() == 1)
                {
                    //set the tracking region as the face
                    HumanPose pose = poses.at(0);
                    auto keypoint_rightear = pose.keypoints.at(16); //right ear
                    auto keypoint_leftear = pose.keypoints.at(17); //left ear
                    if( keypoint_rightear.x >=0 && keypoint_rightear.y != -1 && keypoint_leftear.x >=0 && keypoint_leftear.y != -1 )
                    {
                        bool bTwoEarsSufficientlyApart = false;
                        if( keypoint_leftear.x - keypoint_rightear.x >= 20 )
                        {
                            bTwoEarsSufficientlyApart = true;
                            roi.width = keypoint_leftear.x - keypoint_rightear.x;
                            roi.x = keypoint_rightear.x;
                            roi.height = roi.width;
                            roi.y = keypoint_leftear.y - 0.5 *roi.width;
                        }
                        else if(keypoint_rightear.x - keypoint_leftear.x >= 20)
                        {
                            bTwoEarsSufficientlyApart = true;
                            roi.width = keypoint_rightear.x - keypoint_leftear.x;
                            roi.x = keypoint_leftear.x;
                            roi.height = roi.width;
                            roi.y = keypoint_rightear.y - 0.5 *roi.width;                        
                        }

                        if( bTwoEarsSufficientlyApart == true && roi.y >= 0)
                        {
                            tracker.reset();
                            tracker = TrackerGOTURN::create();
                            tracker->init(inputImage, roi);
                            rectangle(displayImage, roi, Scalar( 0, 255, 0 ), 2, 1 );     //green
                            str_timestamp_initialize_tracker = str_timestamp;
                            b_tracker_initialized = true;
                            report_data.set_tracker_roi_x(static_cast<int>(roi.x));
                            report_data.set_tracker_roi_y(static_cast<int>(roi.y));
                            report_data.set_tracker_roi_width(static_cast<int>(roi.width));
                            report_data.set_tracker_roi_height(static_cast<int>(roi.height));
                            report_data.set_roi_rectangle_color(1);
                        }
                    }
                }

                if(b_tracker_initialized && str_timestamp.compare(str_timestamp_initialize_tracker) != 0)
                {
                    bool b_do_track = false;
                    if(poses.size() == 0 )
                        b_do_track = true;
                    
                    if( poses.size() == 1)
                    {
                        HumanPose pose = poses.at(0);
                        auto keypoint_rightear = pose.keypoints.at(16); //right ear
                        auto keypoint_leftear = pose.keypoints.at(17); //left ear
                        if( keypoint_rightear.x <= 0 || keypoint_leftear.x <= 0)
                            b_do_track = true;
                    }

                    if( b_do_track)
                    {
                        bool ok = tracker->update(inputImage, roi);
                        if(ok)
                        {
                            if(FLAGS_Verbose)
                                cout << "Tracker x:" << roi.x << " y:" << roi.y << " width:" << roi.width << " height:" << roi.height << endl;
                            //how to let the robot to track the box? There is no openpose landmark locations. The robot need to take two different input format.
                            report_data.set_tracker_roi_x(static_cast<int>(roi.x));
                            report_data.set_tracker_roi_y(static_cast<int>(roi.y));
                            report_data.set_tracker_roi_width(static_cast<int>(roi.width));
                            report_data.set_tracker_roi_height(static_cast<int>(roi.height));
                            report_data.set_roi_rectangle_color(2);

                            if( 0 <= roi.x && 0 <= roi.y && roi.x + roi.width <= inputImage.cols && roi.y + roi.height <= inputImage.rows )
                                rectangle(displayImage, roi, Scalar( 0, 0, 255 ), 2, 1 );     //red
                        }
                        else
                        {
                            if(FLAGS_Verbose)
                                cout << "Tracker not ok" << endl;
                        }
                    }                    
                }


                //Convert openpose results to our own format
                report_data.set_openpose_cnt(poses.size());
                for( HumanPose pose: poses)
                {
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

                
                //thread_charades_squeezenet.join();
//                if( bSaveTransmittedImage )
//                    thread_save_image.join();

                strcpy(str_results, report_data.DebugString().c_str());     //How to transmit a protobuf object?

                if( bShowRenderedImage )
                {
                    renderHumanPose(poses, displayImage);
                    cv::imshow("opencv result", displayImage);
                    cv::waitKey(1);
                }

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