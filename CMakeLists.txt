# Copyright (C) 2018 Intel Corporation
# SPDX-License-Identifier: Apache-2.0
#

set(TARGET_NAME "8_openvino")

set(PROTO_FILE AnalyzedResults.proto)

#enable debug information
#set( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g")   

# Find OpenCV components if exist
find_package(OpenCV COMPONENTS highgui QUIET)
if(NOT(OpenCV_FOUND))
    message(WARNING "OPENCV is disabled or not found, " ${TARGET_NAME} " skipped")
    return()
endif()

include(FindProtobuf)
find_package(Boost COMPONENTS system REQUIRED)
find_package(TensorflowCC REQUIRED COMPONENTS Shared)

protobuf_generate_cpp(PROTO_SRC PROTO_HDR ${PROTO_FILE})
get_filename_component(PROTO_HDR_DIRECTORY ${PROTO_HDR} DIRECTORY)
include_directories(${PROTO_HDR_DIRECTORY})
include_directories(OpenCV_INCLUDE_DIR)

set(SOURCES include/human_pose.hpp
            include/8_openvino.hpp
            include/human_pose_estimator.hpp
            include/peak.hpp
            include/render_human_pose.hpp
            include/SocketToServer.hpp
            src/human_pose.cpp
            src/human_pose_estimator.cpp
            src/peak.cpp
            src/render_human_pose.cpp
            8_openvino.cpp
            SocketToServer.cpp
            ProcessImage_tensorflow.cpp
            filter.cpp
            ${PROTO_SRC})

add_executable(${TARGET_NAME} ${SOURCES})

link_directories(${LIB_FOLDER} /usr/local/lib)

set_target_properties(${TARGET_NAME} PROPERTIES "CMAKE_CXX_FLAGS" "${CMAKE_CXX_FLAGS} -fPIE"
  COMPILE_PDB_NAME ${TARGET_NAME})

target_include_directories(${TARGET_NAME} PUBLIC "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>")
target_link_libraries(${TARGET_NAME} ${InferenceEngine_LIBRARIES} ${OpenCV_LIBS} gflags ${PROTOBUF_LIBRARIES} ${Boost_SYSTEM_LIBRARY} IE::ie_cpu_extension libopencv_tracking.so libopencv_dnn.so TensorflowCC::Static)

if(UNIX)
  target_link_libraries(${TARGET_NAME} ${LIB_DL} pthread)
endif()
