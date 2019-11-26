find_package(TensorflowCC REQUIRED)
find_package(Protobuf REQUIRED)
find_package(Boost COMPONENTS system REQUIRED)
find_package(CUDA)
#find_package(OpenCV COMPONENTS highgui REQUIRED)

set(PROTO_FILE AnalyzedResults.proto)
protobuf_generate_cpp(PROTO_SRC PROTO_HDR ${PROTO_FILE})
file (GLOB_RECURSE HEADERS ${CMAKE_CURRENT_SOURCE_DIR}/*.hpp)
list(APPEND HEADERS LePi/app/LePiClient/LePiClient.hpp)
set(SOURCES src/human_pose.cpp
            src/human_pose_estimator.cpp
            src/peak.cpp
            src/render_human_pose.cpp
            SocketToServer.cpp
            ProcessImage.cpp
            Pose.cpp
            Tensor.cpp
            JPEG.cpp
            PoseSensitiveEmbedding.cpp
            TensorMatConversion.cpp
            utility_directory.cpp
            utility_TimeRecorder.cpp
            ReID.cpp
            utility_compute.cpp
            utility_string.cpp
            utility_csv.cpp
            8_openvino.cpp
            ${PROTO_SRC}
            LePi/app/LePiClient/LePiClient.cpp)

#add LePi to this project
add_subdirectory(LePi)

ie_add_sample(NAME 8_openvino
              SOURCES ${SOURCES}
              HEADERS ${HEADERS}
              INCLUDE_DIRECTORIES "${CMAKE_CURRENT_SOURCE_DIR}/include"
              OPENCV_DEPENDENCIES highgui)

#enable debug information
set( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g")   

get_filename_component(PROTO_HDR_DIRECTORY ${PROTO_HDR} DIRECTORY)
include_directories(${PROTO_HDR_DIRECTORY} ${PROTOBUF_INCLUDE_DIRS} LePi/lib/utils/include)
target_link_libraries(${IE_SAMPLE_NAME} cnpy TensorflowCC::Shared ${PROTOBUF_LIBRARIES} ${Boost_SYSTEM_LIBRARY} ${utils_LIBRARY})

if(CUDA_FOUND)
  target_link_libraries(${IE_SAMPLE_NAME} ${CUDA_LIBRARIES})
endif()

if(UNIX)
  target_link_libraries(${IE_SAMPLE_NAME} ${LIB_DL} pthread)
endif()