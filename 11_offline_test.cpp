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


#include <samples/ocv_common.hpp>

#include "11_offline_test.hpp"
#include "SocketToServer.hpp"
#include "ProcessImage.hpp"
#include "utility_directory.hpp"
#include "utility_string.hpp"

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

        if(!FLAGS_save_to_directory.empty())
            CreateDirectory(FLAGS_save_to_directory);
        PSE id_feature_generator(FLAGS_graph_path);
        CreateDirectory(FLAGS_output_directory);
        CreateDirectory(FLAGS_output_directory + "_region");
        vector<string> filename_vector = LoadFileList(FLAGS_filelist_path);
        process_image_offline( 
            FLAGS_pose_model, 
            FLAGS_ShowRenderedImage, 
            FLAGS_SaveTransmittedImage, 
            FLAGS_save_to_directory,
            (float)FLAGS_midPointsScoreThreshold,
            id_feature_generator,
            FLAGS_output_directory,
            filename_vector
        );
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
