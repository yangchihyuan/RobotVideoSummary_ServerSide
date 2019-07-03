This project is modified from the sample project human_pose_estimation_demo of Intel OpenVINO toolkit 2018.5.455. It receives frames transmitted from the robot-side program, estimates human pose landmark coordinates, and reports the results to the robot-side program.

# Environment Setting
- Ubuntu 16.04
- Intel OpenVINO toolkit 2018.5.455
- boost library 1.58
- protobuf 3.5.1
- OpenCV 4.0.0

# Installation
- clone this repository to OpenVINO's sample directory, e.g. /opt/intel/computer_vision_sdk/inference_engine/samples
- run the OpenVINO's build_samples.sh to build this project, and an executable file 8_openvino is supposed to be built at ~/inference_engine_samples_build/intel64/Release
- set the permissions as executable for the run_server_side_program.sh in the RobotVideoSummary_ServerSide directory:
    chmod u+x RobotVideoSummary_ServerSide/run_server_side_program.sh
- run the shell script:
    ./RobotVideoSummary_ServerSide/run_server_side_program.sh
