#include "TensorMatConversion.hpp"
#include "Tensor.hpp"

//This function merges all layers of a tensor to a single-channel cv::Mat
Mat ConvertTensorToImageMat(Tensor input_tensor)
{
    vector<int> shape = get_tensor_shape(input_tensor);
    int mChannel_tensor = shape[3];
    int mChannel_mat = 1;
    int mInputHeight = shape[1];
    int mInputWidth = shape[2];
    Mat img(mInputHeight, mInputWidth, CV_32FC1);

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
                    cout << input_tensor_mapped(0, y, x, c) << endl;
            }
        }
    }
    return img;
}

Tensor ReadTensorFromImageMat(Mat img)
{
    img.convertTo(img,CV_32FC3);
    int mChannel = img.channels();
    int mInputHeight = img.rows;
    int mInputWidth = img.cols;
//    img = (img - mInputMean)/mInputStd;
    Tensor input_tensor(DT_FLOAT, TensorShape({1, mInputHeight, mInputWidth, mChannel}));
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
