//Why do I need to use Tensor.cpp? Where do I use the Tensor.cpp?
#include "Tensor.hpp"
#include "tensorflow/cc/client/client_session.h"
#include "tensorflow/cc/ops/standard_ops.h"


vector<int> get_tensor_shape(const Tensor& tensor)
{
    vector<int> shape;
    int num_dimensions = tensor.shape().dims();
    for(int ii_dim=0; ii_dim<num_dimensions; ii_dim++) {
        shape.push_back(tensor.shape().dim_size(ii_dim));
    }
    return shape;
}

Tensor ConcatenateTensors(const Tensor& tensor1, const Tensor& tensor2, const int dimention)
{
    using namespace tensorflow::ops;
    Scope root = Scope::NewRootScope();

    // Concatenate
    auto concatT1T2 = Concat(root.WithOpName("ConcatT1T2"), { tensor1, tensor2 }, dimention);       //concatenate at the 3rd (0-based) dimention.

    // Evaluate
    std::vector<Tensor> outputs;
    ClientSession session(root);
    TF_CHECK_OK(session.Run({concatT1T2}, &outputs));
    // Get output tensor
    Tensor result = outputs[0];
    return result;
}

Tensor ResizeTensor(const Tensor& input_tensor, const int new_height, const int new_width)
{
    using namespace tensorflow::ops;
    Scope root = Scope::NewRootScope();

    // Resize
    Tensor size_tensor(DT_INT32, TensorShape({2}));
    auto size_tensor_mapped = size_tensor.tensor<int, 1>();
    size_tensor_mapped(0) = new_height;
    size_tensor_mapped(1) = new_width;

    auto resize_graph = ResizeBilinear(root.WithOpName("ResizeBilinear"), input_tensor, size_tensor);

    // Evaluate
    vector<Tensor> outputs;
    ClientSession session(root);
    TF_CHECK_OK(session.Run({resize_graph}, &outputs));
    // Get output tensor
    Tensor result = outputs[0];
    return result;

}
