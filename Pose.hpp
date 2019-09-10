#include <string>
#include <opencv2/opencv.hpp>

#include "human_pose.hpp"
#include "tensorflow/core/framework/tensor.h"

using namespace std;
using namespace cv;
using namespace human_pose_estimation;   //for HumanPose struct
using namespace tensorflow;

struct MatPosePair
{
    Mat mat;
    HumanPose pose;
};

int ConvertImageToPoseMaps_npy(const string& input_dir, const vector<string>& file_list, const string& output_dir, const string& pose_model, const double midPointsScoreThreshold);
int ConvertImageToPoses(const Mat& input_image, const string& pose_model, const double midPointsScoreThreshold, vector<HumanPose>& poses );
int ConvertPoseToTensor(const HumanPose& pose, Tensor &Tensor);
vector<MatPosePair> CropRegionsFromPoses(const Mat& inputImage, const vector<HumanPose>& poese);
Rect GetPoseRegion(const HumanPose& pose);
vector<int> SortPosesByNeckToNose(const vector<HumanPose>& poses);
vector<array<float,1536>> ConvertMatPosePairsToReIDFeatures(const vector<MatPosePair>& pairs);

