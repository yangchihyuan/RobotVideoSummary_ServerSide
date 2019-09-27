This project is modified from the sample project human_pose_estimation_demo of Intel OpenVINO toolkit 2018.5.455. It receives frames transmitted from the robot-side program, estimates human pose landmark coordinates, and reports the results to the robot-side program.

# Environment Setting
- Ubuntu 16.04
- python 3.5.2
- python3-pip
- Intel OpenVINO toolkit 2019 R2
- libboost-dev 1.58
- libboost-system-dev 1.58
- tensorflow_CC 1.14
- bazel 0.24.1
- cuda 10.0
- cudnn 7.6.3
- libprotobuf-dev 3.8.0
- protobuf-compiler 3.8.0
- [cnpy](https://github.com/rogersce/cnpy)

# Library Setting
After installing OpenVINO 2019 R2, we have to donwload a human pose model pre-trained by Intel.
```
sudo pip3 install pyyaml
cd ~/intel/openvino/deployment_tools/open_model_zoo/tools/downloader
./downloader.py --name human-pose*
```


# Installation
- clone this repository to OpenVINO's sample directory. For example, mine is ~/intel/openvino/deployment_tools/open_model_zoo/demos because I did not use sudo to install OpenVINO. If you use sudo, the directory is likely to be /opt/intel/openvino/deployment_tools/open_model_zoo/demos. 
```
cd ~/intel/openvino/deployment_tools/open_model_zoo/demos
git clone https://github.com/yangchihyuan/RobotVideoSummary_ServerSide.git
```
- run the OpenVINO's build_demos.sh at ~/intel/openvino/deployment_tools/open_model_zoo/demos to build this project, and an executable file 8_openvino is supposed to be built at ~/omz_demos_build/intel64/Release/
```
cd RobotVideoSummary_ServerSide
../build_samples.sh
```
- set the permissions as executable for the run_server_side_program.sh in the RobotVideoSummary_ServerSide directory
```
chmod +x run_server_side_program.sh
```
- run the shell script
```
./run_server_side_program.sh
```
- To terminate this program, press Ctrl+C
