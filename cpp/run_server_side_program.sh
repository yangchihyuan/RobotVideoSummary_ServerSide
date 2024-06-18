#!/bin/bash
#Chih-Yuan Yang 2024

if [ $# == 0 ]; then
~/omz_demos_build/intel64/Release/8_openvino \
--pose_model ~/open_model_zoo/models/intel/human-pose-estimation-0001/FP32/human-pose-estimation-0001.xml \
--SaveTransmittedImage=true \
--save_to_directory=/home/chihyuan/Downloads \
--port_number=8895 \
--ShowRenderedImage=true
elif [ $1 == "debug" ]; then
gdb --args ~/omz_demos_build/intel64/Release/8_openvino \
--pose_model ~/open_model_zoo/models/intel/human-pose-estimation-0001/FP32/human-pose-estimation-0001.xml \
--SaveTransmittedImage=true \
--save_to_directory=/home/chihyuan/Downloads \
--port_number=8895 \
--ShowRenderedImage=true
fi
