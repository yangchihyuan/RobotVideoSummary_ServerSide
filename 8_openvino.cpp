// Copyright (C) 2018 Intel Corporation
// SPDX-License-Identifier: Apache-2.0
//

/**
* \brief The entry point for the Inference Engine Human Pose Estimation demo application
* \file human_pose_estimation_demo/main.cpp
* \example human_pose_estimation_demo/main.cpp
*/

#include <vector>
#include <thread>       //without this, std::thread is unknown
//#include <dirent.h>     //without this, DIR is unknown


#include <samples/ocv_common.hpp>

#include "8_openvino.hpp"
#include "SocketToServer.hpp"
#include "ProcessImage.hpp"
#include "utility_directory.hpp"
#include "utility_string.hpp"
#include "utility_csv.hpp"

using namespace InferenceEngine;

bool ParseAndCheckCommandLine(int argc, char* argv[]) {
    // ---------------------------Parsing and validation of input args--------------------------------------

    gflags::ParseCommandLineNonHelpFlags(&argc, &argv, true);
    std::cout << "[ INFO ] Parsing input parameters" << std::endl;
    return true;
}

int main(int argc, char* argv[]) {

    try {
        std::cout << "InferenceEngine: " << GetInferenceEngineVersion() << std::endl;

        // ------------------------------ Parsing and validation of input args ---------------------------------
        if (!ParseAndCheckCommandLine(argc, argv)) {
            return EXIT_SUCCESS;
        }

        //Setup protobuf
        GOOGLE_PROTOBUF_VERIFY_VERSION;

        PSE id_feature_generator(FLAGS_graph_path);
        vector<string> filelist_example = LoadFileList(FLAGS_filelist_example);

        if(FLAGS_mode.compare("offline_test") == 0 )
        {
            vector<string> filename_vector = LoadFileList(FLAGS_filelist_path);
            process_image_offline( 
                FLAGS_pose_model, 
                FLAGS_ShowRenderedImage, 
                FLAGS_SaveTransmittedImage, 
                FLAGS_save_to_directory,
                id_feature_generator,
                filename_vector,
                FLAGS_subject_name
            );
        }
        else if(FLAGS_mode.compare("server_side_program") == 0 )
        {
            std::thread thread_receive_frame(receive_socket, FLAGS_port_number);
            std::thread thread_report_results(report_results, FLAGS_port_number+1);     //another thread to send back results.
            std::thread thread_image_process(process_image, 
                FLAGS_pose_model, 
                FLAGS_ShowRenderedImage, 
                FLAGS_SaveTransmittedImage, 
                FLAGS_save_to_directory,
                id_feature_generator,
                FLAGS_subject_name,
                FLAGS_enable_reid
            );
    //        std::thread thread_save_buffer_as_JPEG(process_save_frame_buffer_as_JPEG_images, FLAGS_SaveTransmittedImage, FLAGS_save_to_directory);

            thread_receive_frame.join();
            thread_report_results.join();
            thread_image_process.join();
//        thread_save_buffer_as_JPEG.join();
        }
        else if(FLAGS_mode.compare("render_poses_crop_regions") == 0)
        {
            vector<string> filename_vector = LoadFileList(FLAGS_filelist_path);
            render_poses_crop_regions(FLAGS_pose_model, 
                FLAGS_save_to_directory, 
                filename_vector);
        }
        else if(FLAGS_mode.compare("dump_example_features") == 0)
        {
            dump_example_features( 
                FLAGS_pose_model, 
                FLAGS_save_to_directory,
                id_feature_generator,
                filelist_example,
                FLAGS_subject_name
            );
        }
    }
    catch (const std::exception& error) {
        std::cerr << "[ ERROR ] " << error.what() << std::endl;
        return EXIT_FAILURE;
    }
    catch (...) {
        std::cerr << "[ ERROR ] Unknown/internal exception happened." << std::endl;
        return EXIT_FAILURE;
    }

    std::cout << "[ INFO ] Execution successful" << std::endl;
    return EXIT_SUCCESS;
}
