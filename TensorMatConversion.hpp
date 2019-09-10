#pragma once

#include "tensorflow/core/framework/tensor.h"
#include <opencv2/highgui.hpp>

using namespace tensorflow;
using namespace cv;
using namespace std;

Mat ConvertTensorToImageMat(Tensor input_tensor);
Tensor ReadTensorFromImageMat(Mat img);
Tensor ConvertMatToNormalizedTensor(Mat img, const Vec3f& img_mean, const Vec3f& img_std);
