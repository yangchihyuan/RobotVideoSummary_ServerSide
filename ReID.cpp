#include "ReID.hpp"
#include "tensorflow/core/framework/graph.pb.h"
#include "tensorflow/core/graph/graph_def_builder.h"
#include "Logger.hpp"
#include "Tensor.hpp"

array<float, 1536> ComputePSN_Feature(Tensor InputTensor)
{
    Tensor dummyTensor(DT_FLOAT, TensorShape({15, 224, 224, 21}));
    Tensor ConcatenatedTensor = ConcatenateTensors(InputTensor, dummyTensor, 0);

    unique_ptr<Session> tf_session;
    string graph_path = "/4t/yangchihyuan/pose-tensorflow-cpp/PSN.pb";
    Status load_graph_status = LoadGraph(graph_path, &tf_session);
    if (!load_graph_status.ok())
    {
        LOG(ERROR) << load_graph_status;
    }
    string input_layer = "resnet_v1_50/Pad";
    string output_layer = "resnet_v1_50/pre_logits/convolution";
    std::vector<Tensor> outputs;
    Status run_status = tf_session->Run({{input_layer, ConcatenatedTensor}},
                                        {output_layer}, {}, &outputs);
    if (!run_status.ok())
    {
        Logger(run_status.ToString());
    }

    Logger("outputs size: " + to_string(outputs.size()));
    array<float, 1536> result;

    if( outputs.size() > 0)
    {
        auto output_tensor_mapped = outputs[0].tensor<float, 4>();      //the output tensor is 4 dim, check the shape.
        for (int i = 0; i < 1536; i++)
        {
            result[i] = output_tensor_mapped(0, i);
        }
    }
    return result;
}

// Reads a model graph definition from disk, and creates a session object you
// can use to run it.
Status LoadGraph(const string &graph_file_name,
                 std::unique_ptr<tensorflow::Session> *session)
{
    tensorflow::GraphDef graph_def;
    Status load_graph_status =
        ReadBinaryProto(tensorflow::Env::Default(), graph_file_name, &graph_def);
    if (!load_graph_status.ok())
    {
        return tensorflow::errors::NotFound("Failed to load compute graph at '",
                                            graph_file_name, "'");
    }
    session->reset(tensorflow::NewSession(tensorflow::SessionOptions()));
    Status session_create_status = (*session)->Create(graph_def);
    if (!session_create_status.ok())
    {
        return session_create_status;
    }
    return Status::OK();
}
