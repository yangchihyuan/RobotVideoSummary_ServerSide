// Copyright (C) 2018 Intel Corporation
// SPDX-License-Identifier: Apache-2.0
//

#pragma once

#include <iostream>
#include <gflags/gflags.h>
#include <google/protobuf/message.h>
#include <google/protobuf/message_lite.h>

/// @brief Message for help argument
static const char help_message[] = "Print a usage message.";

/// @brief Message for video argument
static const char video_message[] = "Required. Path to a video. Default value is \"cam\" to work with camera.";

/// @brief Message for model argument
static const char human_pose_estimation_model_message[] = "Required. Path to the Human Pose Estimation model (.xml) file.";

static const char object_detection_model_message[] = "Required. Path to the Object Detetion model (.xml) file.";

/// @brief Message for user library argument
static const char custom_cpu_library_message[] = "Optional. Required for CPU custom layers." \
"Absolute path to a shared library with the layers implementation.";

/// @brief Message for assigning Human Pose Estimation inference to device
static const char target_device_message[] = "Optional. Specify the target device for Human Pose Estimation "\
                                            "(CPU, GPU, FPGA or MYRIAD is acceptable). Default value is \"CPU\".";

/// @brief Message for performance counter
static const char performance_counter_message[] = "Optional. Enable per-layer performance report.";

/// @brief Message for not showing processed video
static const char no_show_processed_video[] = "Optional. Do not show processed video.";

/// @brief Message for raw output
static const char raw_output_message[] = "Optional. Output inference results as raw values.";

/// @brief Defines flag for showing help message <br>
DEFINE_bool(h, false, help_message);

/// @brief Defines parameter for setting video file <br>
/// It is a required parameter
DEFINE_string(i, "cam", video_message);

/// @brief Defines parameter for human pose estimation model file <br>
/// It is a required parameter
DEFINE_string(pose_model, "", human_pose_estimation_model_message);
DEFINE_string(detect_model, "", "object detection model");
/// @brief Defines parameter for the target device to infer on <br>
/// It is an optional parameter
DEFINE_string(d, "CPU", target_device_message);

/// @brief Defines flag for per-layer performance report <br>
/// It is an optional parameter
DEFINE_bool(pc, false, performance_counter_message);

/// @brief Defines flag for disabling processed video showing <br>
/// It is an optional parameter
DEFINE_bool(no_show, false, no_show_processed_video);

/// @brief Defines flag to output raw results <br>
/// It is an optional parameter
DEFINE_bool(r, false, raw_output_message);

/// @brief Defines absolute path to CPU library with user layers <br>
/// It is a optional parameter
DEFINE_string(l, "", custom_cpu_library_message);

DEFINE_double(yolo_threshold_t, 0.5, "");
DEFINE_double(yolo_threshold_iou_t, 0.4, "");

DEFINE_bool(ShowRenderedImage, false, "If enabled, it will show the rendered skeleton, but it will inrease the network load.");
DEFINE_bool(SaveTransmittedImage, false, "If enabled, it will save the transmitted image into the disk.");
DEFINE_string(save_to_directory, "/4t/yangchihyuan/TransmittedImages/temp","directory of transmitted JPEG frames");
DEFINE_uint64(port_number, 8895, "Port number to received HTTP packages");
DEFINE_bool(Verbose, true, "Show information.");

//OpenPose parameter
DEFINE_double(midPointsScoreThreshold, 0.5, "");

/**
* @brief This function shows a help message
*/
static void showUsage() {
    std::cout << std::endl;
    std::cout << "human_pose_estimation_demo [OPTION]" << std::endl;
    std::cout << "Options:" << std::endl;
    std::cout << std::endl;
    std::cout << "    -h                         " << help_message << std::endl;
    std::cout << "    -i \"<path>\"                " << video_message << std::endl;
    std::cout << "    -pose_model \"<path>\"                " << human_pose_estimation_model_message << std::endl;
    std::cout << "    -detect_model \"<path>\"                " << object_detection_model_message << std::endl;
    std::cout << "      -l \"<absolute_path>\"    " << custom_cpu_library_message << std::endl;
    std::cout << "    -d \"<device>\"              " << target_device_message << std::endl;
    std::cout << "    -pc                        " << performance_counter_message << std::endl;
    std::cout << "    -no_show                   " << no_show_processed_video << std::endl;
    std::cout << "    -r                         " << raw_output_message << std::endl;
}
