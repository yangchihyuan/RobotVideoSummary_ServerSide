#pragma once

#include "tensorflow/core/framework/tensor.h"

using namespace tensorflow;
using namespace std;

vector<int> get_tensor_shape(const Tensor& tensor);
Tensor ConcatenateTensors(const Tensor& tensor1, const Tensor& tensor2, const int dimension);
Tensor ResizeTensor(const Tensor& input_tensor, const int height, const int width);
