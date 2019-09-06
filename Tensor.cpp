#include "Tensor.hpp"
#include "tensorflow/cc/client/client_session.h"
#include "tensorflow/cc/ops/standard_ops.h"

//This function merges all layers of a tensor to a single-channel cv::Mat
cv::Mat ConvertTensorToImageMat(tensorflow::Tensor input_tensor)
{
    std::vector<int> shape = get_tensor_shape(input_tensor);
    int mChannel_tensor = shape[3];
    int mChannel_mat = 1;
    int mInputHeight = shape[1];
    int mInputWidth = shape[2];
    cv::Mat img(mInputHeight, mInputWidth, CV_32FC1);

    auto input_tensor_mapped = input_tensor.tensor<float, 4>();

    float *dst_data = (float *)img.data;

    for (int y = 0; y < mInputHeight; ++y)
    {
        float *dst_row = dst_data + (y * mInputWidth * mChannel_mat);
        for (int x = 0; x < mInputWidth; ++x)
        {
            float *dst_pixel = dst_row + (x * mChannel_mat);
            for (int c = 0; c < mChannel_tensor; ++c)
            {
                *dst_pixel += input_tensor_mapped(0, y, x, c);
                if( input_tensor_mapped(0, y, x, c) > 0 ) 
                    std::cout << input_tensor_mapped(0, y, x, c) << std::endl;
            }
        }
    }
    return img;
}

tensorflow::Tensor ReadTensorFromImageMat(cv::Mat img)
{
    img.convertTo(img,CV_32FC3);
    int mChannel = img.channels();
    int mInputHeight = img.rows;
    int mInputWidth = img.cols;
//    img = (img - mInputMean)/mInputStd;
    tensorflow::Tensor input_tensor(tensorflow::DT_FLOAT, tensorflow::TensorShape({1, mInputHeight, mInputWidth, mChannel}));
    auto input_tensor_mapped = input_tensor.tensor<float, 4>();

    const float *source_data = (float *)img.data;

    for (int y = 0; y < mInputHeight; ++y)
    {
        const float *source_row = source_data + (y * mInputWidth * mChannel);
        for (int x = 0; x < mInputWidth; ++x)
        {
            const float *source_pixel = source_row + (x * mChannel);
            for (int c = 0; c < mChannel; ++c)
            {
                const float *source_value = source_pixel + c;
                input_tensor_mapped(0, y, x, c) = *source_value;
            }
        }
    }
    return input_tensor;
}

Tensor ConvertMatToNormalizedTensor(Mat img, const Vec3f& img_mean, const Vec3f& img_std)
{
    img.convertTo(img,CV_32FC3);
    const int mChannel = img.channels();
    const int mInputHeight = img.rows;
    const int mInputWidth = img.cols;
    //normalize images
    for( int y=0; y<mInputHeight; y++)
    {
	    for( int x=0; x<mInputWidth; x++ )
        {
            for( int c=0; c<mChannel; c++)
            {
                img.at<Vec3f>(Point(x,y))[c] = (img.at<Vec3f>(Point(x,y))[c] - img_mean[c]) / img_std[c];
            }
        }
    }
    tensorflow::Tensor input_tensor(tensorflow::DT_FLOAT, tensorflow::TensorShape({1, mChannel, mInputHeight, mInputWidth}));
    auto input_tensor_mapped = input_tensor.tensor<float, 4>();

    const float *source_data = (float *)img.data;

    for (int y = 0; y < mInputHeight; ++y)
    {
        const float *source_row = source_data + (y * mInputWidth * mChannel);
        for (int x = 0; x < mInputWidth; ++x)
        {
            const float *source_pixel = source_row + (x * mChannel);
            for (int c = 0; c < mChannel; ++c)
            {
                const float *source_value = source_pixel + c;
                input_tensor_mapped(0, c, y, x) = *source_value;
            }
        }
    }
    return input_tensor;
}

std::vector<int> get_tensor_shape(const tensorflow::Tensor& tensor)
{
    std::vector<int> shape;
    int num_dimensions = tensor.shape().dims();
    for(int ii_dim=0; ii_dim<num_dimensions; ii_dim++) {
        shape.push_back(tensor.shape().dim_size(ii_dim));
    }
    return shape;
}

Tensor ConcatenateTensors(const Tensor& tensor1, const Tensor& tensor2)
{
    using namespace tensorflow::ops;
    Scope root = Scope::NewRootScope();

    // Concatenate
    auto concatT1T2 = Concat(root.WithOpName("ConcatT1T2"), { tensor1, tensor2 }, 1);

    // Evaluate
    std::vector<Tensor> outputs;
    ClientSession session(root);
    TF_CHECK_OK(session.Run({concatT1T2}, &outputs));
    // Get output tensor
    Tensor result = outputs[0];
    return result;
}

vector<