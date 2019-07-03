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
#include <dirent.h>     //without this, DIR is unknown


#include <samples/ocv_common.hpp>

#include "8_openvino.hpp"
#include "SocketToServer.hpp"
#include "ProcessImage.hpp"

using namespace InferenceEngine;

bool ParseAndCheckCommandLine(int argc, char* argv[]) {
    // ---------------------------Parsing and validation of input args--------------------------------------

    gflags::ParseCommandLineNonHelpFlags(&argc, &argv, true);
    if (FLAGS_h) {
        showUsage();
        return false;
    }

    std::cout << "[ INFO ] Parsing input parameters" << std::endl;

    if (FLAGS_i.empty()) {
        throw std::logic_error("Parameter -i is not set");
    }

    if (FLAGS_pose_model.empty()) {
        throw std::logic_error("Parameter -pose_model is not set");
    }

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

        std::thread thread_receive_frame(receive_socket, FLAGS_port_number);
        std::thread thread_report_results(report_results, FLAGS_port_number+1);     //another thread to send back results.
        std::thread thread_image_process(process_image, FLAGS_pose_model, FLAGS_detect_model, FLAGS_l, 
            FLAGS_yolo_threshold_t, FLAGS_yolo_threshold_iou_t, FLAGS_ShowRenderedImage, FLAGS_SaveTransmittedImage, FLAGS_save_to_directory,
            (float)FLAGS_midPointsScoreThreshold
        );
        if(FLAGS_SaveTransmittedImage){
            DIR* dir = opendir(FLAGS_save_to_directory.c_str() );
            if (dir)
            {
                /* Directory exists. */
                closedir(dir);
            }
            else if (ENOENT == errno)
            {
                /* Directory does not exist. */
                mkdir(FLAGS_save_to_directory.c_str(), S_IRWXU|S_IRWXG);
            }
        }
//        std::thread thread_save_buffer_as_JPEG(process_save_frame_buffer_as_JPEG_images, FLAGS_SaveTransmittedImage, FLAGS_save_to_directory);

        thread_receive_frame.join();
        thread_report_results.join();
        thread_image_process.join();
//        thread_save_buffer_as_JPEG.join();


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
