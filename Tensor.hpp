#include "tensorflow/core/framework/tensor.h"
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>

using namespace tensorflow;
using namespace cv;
using namespace std;
typedef vector<float

Mat ConvertTensorToImageMat(Tensor input_tensor);
Tensor ReadTensorFromImageMat(Mat img);
Tensor ConvertMatToNormalizedTensor(Mat img, const Vec3f& img_mean, const Vec3f& img_std);
vector<int> get_tensor_shape(const Tensor& tensor);
Tensor ConcatenateTensors(const Tensor& tensor1, const Tensor& tensor2);
