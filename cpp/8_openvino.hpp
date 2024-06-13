// Copyright (C) 2018 Intel Corporation
// SPDX-License-Identifier: Apache-2.0
//

#pragma once

#include <iostream>
#include <gflags/gflags.h>

DEFINE_string(pose_model, "", "model, an xml file");
DEFINE_bool(ShowRenderedImage, false, "If enabled, it will show the rendered skeleton, but it will inrease the network load.");
DEFINE_bool(SaveTransmittedImage, false, "If enabled, it will save the transmitted image into the disk.");
DEFINE_string(save_to_directory, "/home/chihyuan/Downloads","directory of transmitted JPEG frames");
DEFINE_uint64(port_number, 8895, "Port number to received HTTP packages");
