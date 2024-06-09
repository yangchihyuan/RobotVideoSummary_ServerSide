// Copyright (C) 2018-2019 Intel Corporation
// SPDX-License-Identifier: Apache-2.0
//
//2024/6/8 This file no longer work in open_model_zoo 2024 because there is no InferenceEngine namespace.

#pragma once

#include <string>
#include <vector>

//#include <inference_engine.hpp>    //This is the 2018-2019 demo file. The file inference_engine.hpp no longer exist in open_model_zoo 2024.
#include <opencv2/core/core.hpp>

#include "human_pose.hpp"

namespace human_pose_estimation {
class HumanPoseEstimator {       //2024/6/8 This class seems out-of-dated. If I want to use open_model_zoo 2024, I need to update the class.
//The new class is either HPEOpenPose or HpeAssociativeEmbedding.
public:
    static const size_t keypointsNumber;

    HumanPoseEstimator(const std::string& modelPath,
                       const std::string& targetDeviceName,
                       bool enablePerformanceReport);
    std::vector<HumanPose> estimate(const cv::Mat& image);
    ~HumanPoseEstimator();

private:
    void preprocess(const cv::Mat& image, float* buffer) const;
    std::vector<HumanPose> postprocess(
            const float* heatMapsData, const int heatMapOffset, const int nHeatMaps,
            const float* pafsData, const int pafOffset, const int nPafs,
            const int featureMapWidth, const int featureMapHeight,
            const cv::Size& imageSize) const;
    std::vector<HumanPose> extractPoses(const std::vector<cv::Mat>& heatMaps,
                                        const std::vector<cv::Mat>& pafs) const;
    void resizeFeatureMaps(std::vector<cv::Mat>& featureMaps) const;
    void correctCoordinates(std::vector<HumanPose>& poses,
                            const cv::Size& featureMapsSize,
                            const cv::Size& imageSize) const;
    bool inputWidthIsChanged(const cv::Size& imageSize);

    int minJointsNumber;
    int stride;
    cv::Vec4i pad;
    cv::Vec3f meanPixel;
    float minPeaksDistance;
    float midPointsScoreThreshold;
    float foundMidPointsRatioThreshold;
    float minSubsetScore;
    cv::Size inputLayerSize;
    int upsampleRatio;
    InferenceEngine::Core ie;  //2024/6/8 Here is a problem. We no longer have the InferenceEngine namespace in open_model_zoo 2024.
    std::string targetDeviceName;
    InferenceEngine::CNNNetwork network;
    InferenceEngine::ExecutableNetwork executableNetwork;
    InferenceEngine::InferRequest request;
    InferenceEngine::CNNNetReader netReader;
    std::string pafsBlobName;
    std::string heatmapsBlobName;
    bool enablePerformanceReport;
    std::string modelPath;
};
}  // namespace human_pose_estimation
