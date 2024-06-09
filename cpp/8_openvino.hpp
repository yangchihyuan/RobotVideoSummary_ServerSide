// Copyright (C) 2018 Intel Corporation
// SPDX-License-Identifier: Apache-2.0
//

#pragma once

#include <iostream>
#include <gflags/gflags.h>
#include <google/protobuf/message.h>
#include <google/protobuf/message_lite.h>


DEFINE_string(pose_model, "", "model, an xml file");
DEFINE_string(detect_model, "", "object detection model");

DEFINE_bool(ShowRenderedImage, false, "If enabled, it will show the rendered skeleton, but it will inrease the network load.");
DEFINE_bool(SaveTransmittedImage, false, "If enabled, it will save the transmitted image into the disk.");
DEFINE_string(save_to_directory, "/4t/yangchihyuan/TransmittedImages/temp","directory of transmitted JPEG frames");
DEFINE_uint64(port_number, 8895, "Port number to received HTTP packages");

DEFINE_bool(enable_reid, false, "");

//Tensorflow parameters
DEFINE_string(graph_path, "","path to the PSE model file (pb)");
DEFINE_string(filelist_example, "","path to the PSE model file (pb)");
DEFINE_string(filelist_path, "/4t/yangchihyuan/TransmittedImages/0918_filelist.txt","");
DEFINE_string(mode, "" , "server_side_program or offline_test or render_poses_crop_regions");
DEFINE_string(subject_name, "" , "");