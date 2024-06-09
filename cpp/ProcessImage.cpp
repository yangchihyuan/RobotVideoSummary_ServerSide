#include <fstream>
#include <mutex>
#include <thread>
//#include "include/human_pose_estimator.hpp"
//#include "include/render_human_pose.hpp"

#include "ProcessImage.hpp"
#include <numeric>      // std::iota
#include "JPEG.hpp"
//#include "AnalyzedResults.pb.h"
#include <gflags/gflags.h>
#include "Logger.hpp"
#include "utility_TimeRecorder.hpp"
#include "ReID.hpp"
#include "utility_directory.hpp"
#include "utility_string.hpp"
#include "utility_csv.hpp"

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


void process_image(std::string pose_model, 
    bool bShowRenderedImage, 
    bool bSaveTransmittedImage, 
    string save_to_directory//, 
//    PSE id_feature_generator,
//    string subject_name,
//    bool b_enable_reid
)
{
    //2024/6/8 The HumanPoseEstimator no longer works
    /*
    HumanPoseEstimator estimator(pose_model, "CPU", false ); //the 3rd argument is per-layer performance report
    unique_ptr<Session> psession;
    ReID reid(100);
    if( b_enable_reid)
    {
        psession.reset(NewSession(SessionOptions()));
        Status session_create_status = psession->Create(id_feature_generator.graph_def);
        if (!session_create_status.ok())
        {
            Logger("Session creation fail.");
        }
        string features_directory = save_to_directory + "/features";
        vector<array<float,1536>> features = read_features(features_directory + "/" + subject_name + ".csv");
        reid.LoadSampleFeature(features);
    }
    */
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
 
            //2024/6/8 Why do I need this?
            //ImageAnalyzedResults::ReportData report_data;
            //report_data.set_key(data_, key_length);
            vector<char> JPEG_Data(data_ + key_info.length() + str_JPEG_length.length() + 2, data_ + length -1);
            bool bCorrectlyDecoded = false;
            Mat inputImage;
            try{
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
/*                vector<HumanPose> poses = estimator.estimate(inputImage );
                if( b_enable_reid)
                {
                    vector<PoseRegion> regions = CropRegionsFromPoses(inputImage, poses);
                    vector<array<float,1536>> ReID_features = ConvertPoseRegionsToReIDFeatures(regions, id_feature_generator, psession);

                    if(regions.size() > 1)
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
                }
                else
                {
                    poses = SortPosesByHeight(poses);
                }
                

                //Convert openpose results to our own format
                report_data.set_openpose_cnt(poses.size());
                for( unsigned int idx = 0; idx < poses.size(); idx++ )
                {
                    HumanPose pose = poses[idx];
                    string temp = ConvertPoseToString(pose);
                    report_data.add_openpose_coord(temp);
                }

                strcpy(str_results, report_data.DebugString().c_str());     //How to transmit a protobuf object?

                if( bShowRenderedImage )
                {
                    renderHumanPose(poses, displayImage);
                    imshow("opencv result", displayImage);
                    waitKey(1);
                }
*/
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

void render_poses_crop_regions(string pose_model, 
    string save_to_directory, 
    vector<string> file_list)
{
//    HumanPoseEstimator estimator(pose_model, "CPU", false ); //the 3rd argument is per-layer performance report
//    string regions_directory = save_to_directory + "/regions";
//    CreateDirectory(regions_directory);
    string raw_images_directory = save_to_directory + "/raw_images";
    string rendered_images_directory = save_to_directory + "/rendered_images";
    CreateDirectory(rendered_images_directory);
    string csv_files_directory = save_to_directory + "/csv_files";
//    CreateDirectory(bounding_boxes_directory);
    string csv_filename = "0503_lab.csv";
    ofstream outfile(csv_files_directory + "/" + csv_filename, ofstream::out);
    outfile << "filename,index,x,y,width,height,label" << endl;

    for(unsigned int file_idx = 0 ; file_idx < file_list.size() ; file_idx++ )
    {
        string filename = file_list[file_idx];
        cout << "\r" << file_idx << " " << filename << flush;
        Mat inputImage = imread(raw_images_directory + "/" + filename);

        Mat displayImage = inputImage.clone();

//        vector<HumanPose> poses = estimator.estimate(inputImage);
        vector<HumanPose> poses;
        if( poses.size() > 0 )
        {
            string rawfilename = RemoveFileExtension(filename);
            //renderHumanPose(poses, displayImage);
            for( unsigned int pose_idx = 0 ; pose_idx < poses.size(); pose_idx++ )
            {
//                string temp = ConvertPoseToString(poses[pose_idx]);
//                cout << temp << endl;
                Rect region = GetPoseRegion(poses[pose_idx]);
                Scalar color;
                color = Scalar( 0, 0, 255 );        //Red, the channel order is BGR
                rectangle( displayImage,region.tl(), region.br(), color, 2, 8, 0 );
                string label = to_string(pose_idx) + ":" + to_string(region.x)+ "," +to_string(region.y)+ "," + to_string(region.width) + "," +to_string( region.height);

                putText(displayImage, 
                    label,
                    region.tl(), // Coordinates
                    cv::FONT_HERSHEY_COMPLEX_SMALL, // Font
                    1.0, // Scale. 2.0 = 2x bigger
                    color, // BGR Color
                    1 // Line Thickness (Optional)
                    ); // Anti-alias (Optional)

                //save bounding boxes
                if( region.width > 0 && region.height > 0)
                {
                    string filename_no_ext = RemoveFileExtension(filename);
                    outfile << filename_no_ext << "," << pose_idx << "," << region.x << "," << region.y << "," << region.width << "," << region.height << "," << endl;
                }
            }

            imwrite(rendered_images_directory + "/" + filename, displayImage);
        }
    }
    outfile.close();
}

/*
void dump_example_features(string pose_model, 
    string save_to_directory//, 
//    PSE id_feature_generator,
//    vector<string> filelist_example,
//    string subject_name
)
{
    HumanPoseEstimator estimator(pose_model, "CPU", false); //the 3rd argument is per-layer performance report
    unique_ptr<Session> psession;
    psession.reset(NewSession(SessionOptions()));
    Status session_create_status = psession->Create(id_feature_generator.graph_def);
    if (!session_create_status.ok())
    {
        Logger("Session creation fail.");
    }
    ReID reid(100);
    string raw_images_directory = save_to_directory + "/raw_images";
    string features_directory = save_to_directory + "/features";
    CreateDirectory(features_directory);

    for(unsigned int file_idx = 0 ; file_idx < filelist_example.size() ; file_idx++ )
    {
        cout << "Load example image " << file_idx << endl;
        string filename = filelist_example[file_idx];
        Mat inputImage = imread(raw_images_directory + "/" + filename);
        vector<HumanPose> poses = estimator.estimate(inputImage );
        vector<PoseRegion> regions = CropRegionsFromPoses(inputImage, poses);
        vector<array<float,1536>> ReID_features = ConvertPoseRegionsToReIDFeatures(regions, id_feature_generator, psession);
        reid.AddSample(ReID_features[0], 0);        //assume id==0 is the target
    }
    reid.DumpSamples(features_directory + "/" + subject_name + ".csv");
}
*/

/*
void process_image_offline(string pose_model, 
    bool bShowRenderedImage, 
    bool bSaveTransmittedImage, 
    string save_to_directory//, 
    //PSE id_feature_generator,
    //vector<string> file_list,
    //string subject_name
    )
{
    HumanPoseEstimator estimator(pose_model, "CPU", false); //the 3rd argument is per-layer performance report
    unique_ptr<Session> psession;
    psession.reset(NewSession(SessionOptions()));
    Status session_create_status = psession->Create(id_feature_generator.graph_def);
    if (!session_create_status.ok())
    {
        Logger("Session creation fail.");
    }
    ReID reid(100);
    string regions_directory = save_to_directory + "/regions";
    CreateDirectory(regions_directory);
    string raw_images_directory = save_to_directory + "/raw_images";
    string rendered_images_directory = save_to_directory + "/rendered_images";
    string test_directory = save_to_directory + "/test";
    CreateDirectory(test_directory);
    string features_directory = save_to_directory + "/features";

    vector<array<float,1536>> features = read_features(features_directory + "/" + subject_name + ".csv");
    reid.LoadSampleFeature(features);

    for(unsigned int file_idx = 0 ; file_idx < file_list.size() ; file_idx++ )
    {
        string filename = file_list[file_idx];
        Mat inputImage = imread(raw_images_directory + "/" + filename);

        Mat displayImage = inputImage.clone();

        vector<HumanPose> poses = estimator.estimate(inputImage );
        vector<PoseRegion> regions = CropRegionsFromPoses(inputImage, poses);
        //I have to dump regions to create ground truth labels
        string rawfilename = RemoveFileExtension(filename);

        vector<array<float,1536>> ReID_features = ConvertPoseRegionsToReIDFeatures(regions, id_feature_generator, psession);

        
        //evaluate ReID feature similarity
        if( regions.size() > 1)
        {
            vector<int> index_vector = reid.SortByFeatureSimilarity(ReID_features);

            //dump images for research
            for(unsigned int i=0; i<index_vector.size(); i++)
            {
                int index_region = index_vector[i];
                imwrite(test_directory + "/" + rawfilename + "_" + to_string(i) + ".jpg", regions[index_region].mat);
            }
        }
    }
}
*/

/*
void convert_regions_to_features(string pose_model, 
    bool bShowRenderedImage, 
    bool bSaveTransmittedImage, 
    string image_directory, 
    PSE id_feature_generator,
    string output_directory,
    vector<string> file_list)
{
    HumanPoseEstimator estimator(pose_model, "CPU", false); //the 3rd argument is per-layer performance report
    unique_ptr<Session> psession;
    psession.reset(NewSession(SessionOptions()));
    Status session_create_status = psession->Create(id_feature_generator.graph_def);
    if (!session_create_status.ok())
    {
        Logger("Session creation fail.");
    }

    ReID SampleCollector;

    for(unsigned int file_idx = 0 ; file_idx < file_list.size() ; file_idx++ )
    {
        string filename = file_list[file_idx];
        Mat inputImage;
        inputImage = imread(save_to_directory + "/" + filename);

        vector<HumanPose> poses = estimator.estimate(inputImage );

        vector<PoseRegion> regions = CropRegionsFromPoses(inputImage, poses);
        string rawfilename = RemoveFileExtension(filename);

        vector<array<float,1536>> ReID_features = ConvertPoseRegionsToReIDFeatures(regions, id_feature_generator, psession);

        //Collect Samples
        if( regions.size() == 1 && regions.size() == 1)
        {
            SampleCollector.AddSample(ReID_features[0], 0);
        }
    }
    SampleCollector.DumpSamples(output_directory + "/SamplesFeatures.csv");
}
*/