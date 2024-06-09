#pragma once

#include "tensorflow/core/framework/tensor.h"
#include "tensorflow/core/public/session.h"

using namespace tensorflow;
using namespace std;

class PSE
{
    public:
    PSE(const string& graph_path);
    array<float, 1536> ComputePSN_Feature(Tensor& InputTensor, unique_ptr<Session>& tf_session);
    GraphDef graph_def;

    private:
    const string graph_file_path;
    const string input_layer;
    const string output_layer;
};