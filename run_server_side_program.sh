~/omz_demos_build/intel64/Release/8_openvino \
--pose_model ~/intel/openvino_2019.2.275/deployment_tools/open_model_zoo/tools/downloader/Transportation/human_pose_estimation/mobilenet-v1/dldt/FP32/human-pose-estimation-0001.xml \
--SaveTransmittedImage=true \
--save_to_directory=/home/yangchihyuan/TransmittedImages/0925 \
--port_number=8895 \
--ShowRenderedImage=true \
--midPointsScoreThreshold=0.5 \
--mode=server_side_program \
--subject_name=chihyuan \
--graph_path=/home/yangchihyuan/pose-tensorflow-cpp/PSN.pb