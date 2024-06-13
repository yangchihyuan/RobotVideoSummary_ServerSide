This project is modified from Intel OpenVINO's Open Model Zoo 2024 demo project human_pose_estimation_demo. It receives frames transmitted from the robot-side program, estimates human pose landmark coordinates, and reports the results to the robot-side program.

# Environment Setting
- Ubuntu 22.04
- Intel OpenVINO toolkit 2024.1.0
- libboost-dev 1.74.0.3ubuntu7
- libprotobuf-dev 3.12.4-1ubuntu7.22.04.1

# Library Setting
Please follow Intel OpenVINO 2024.1.0's instruction to install the library.
We need Intel OpenVINO Toolkit Open Model Zoo as a framework. Please git clone it from its GitHub repository and download its human pose pre-trained model human-pose-estimation-0001.xml and its bin file.

# Installation
Suppose your Open Model Zoo is installed in ~/Downloads/open_model_zoo, and the demo projects are in the directory ~/Downloads/open_model_zoo/demos.
Please git clone my repository into the demos directory.
```
cd ~/Downloads/open_model_zoo/demos
git clone https://github.com/yangchihyuan/RobotVideoSummary_ServerSide.git
```
- run the OpenVINO's build_demos.sh at ~/Downloads/open_model_zoo/demos to build this project, and an executable file 8_openvino should be built at ~/omz_demos_build/intel64/Release/
- set the permissions as executable for the run_server_side_program.sh in the RobotVideoSummary_ServerSide directory
```
chmod +x run_server_side_program.sh
```
- run the shell script
```
./run_server_side_program.sh
```
- To terminate this program, press Ctrl+C
