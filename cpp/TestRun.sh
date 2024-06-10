#!/bin/bash
#Chih-Yuan Yang 2024

if [ $1 == "openpose" ] || [ $# == 0 ]; then
#If I use the FP16 model, the FPS is the same. What is the pros and cons of the FP16 model?
#-i 0 means that I use camera 0 as the input device
    ~/omz_demos_build/intel64/Release/human_pose_estimation_demo -at openpose -i 0 -m ~/Downloads/open_model_zoo/tools/model_tools/build/lib/omz_tools/intel/human-pose-estimation-0001/FP32/human-pose-estimation-0001.xml
elif [ $1 == "ae5" ]; then
    ~/omz_demos_build/intel64/Release/human_pose_estimation_demo -at ae -i 0 -m ~/Downloads/open_model_zoo/tools/model_tools/build/lib/omz_tools/intel/human-pose-estimation-0005/FP32/human-pose-estimation-0005.xml
elif [ $1 == "ae6" ]; then
    ~/omz_demos_build/intel64/Release/human_pose_estimation_demo -at ae -i 0 -m ~/Downloads/open_model_zoo/tools/model_tools/build/lib/omz_tools/intel/human-pose-estimation-0006/FP32/human-pose-estimation-0006.xml
elif [ $1 == "ae7" ]; then
    ~/omz_demos_build/intel64/Release/human_pose_estimation_demo -at ae -i 0 -m ~/Downloads/open_model_zoo/tools/model_tools/build/lib/omz_tools/intel/human-pose-estimation-0007/FP32/human-pose-estimation-0007.xml
fi


#Now I only have a .pth file. How to convert it to the xml+bin files?
#./human_pose_estimation_demo -at higherhrnet -i 0 -m ~/Downloads/open_model_zoo/tools/model_tools/build/lib/omz_tools/intel/human-pose-estimation-0001/FP32/human-pose-estimation-0001.xml

