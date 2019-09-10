#pragma once

#include "tensorflow/core/framework/tensor.h"
#include "tensorflow/core/public/session.h"

using namespace tensorflow;
using namespace std;

array<float, 1536> ComputePSN_Feature(Tensor InputTensor);
Status LoadGraph(const string &graph_file_name, unique_ptr<Session> *session);   //Status is an OpenCV enum

