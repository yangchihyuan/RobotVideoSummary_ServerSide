find_package(TensorflowCC REQUIRED)
find_package(Protobuf REQUIRED)
find_package(Boost COMPONENTS system REQUIRED)
find_package(CUDA)
find_package(OpenCV COMPONENTS highgui REQUIRED)

set(PROTO_FILE AnalyzedResults.proto)
protobuf_generate_cpp(PROTO_SRC PROTO_HDR ${PROTO_FILE})

message(STATUS "PROTO_HDR ${PROTO_HDR}")

set(SOURCES src/human_pose.cpp
            src/human_pose_estimator.cpp
            src/peak.cpp
            src/render_human_pose.cpp
            8_openvino.cpp
            SocketToServer.cpp
            ProcessImage.cpp
            Pose.cpp
            Tensor.cpp
            JPEG.cpp
            ReID.cpp
            TensorMatConversion.cpp
            ${PROTO_SRC})

file (GLOB_RECURSE HEADERS ${CMAKE_CURRENT_SOURCE_DIR}/*.hpp)

ie_add_sample(NAME 8_openvino
              SOURCES ${SOURCES}
              HEADERS ${HEADERS}
              INCLUDE_DIRECTORIES "${CMAKE_CURRENT_SOURCE_DIR}/include"
              OPENCV_DEPENDENCIES highgui)



#enable debug information
#set( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g")   

include_directories(${PROTOBUF_INCLUDE_DIRS})
get_filename_component(PROTO_HDR_DIRECTORY ${PROTO_HDR} DIRECTORY)
include_directories(${PROTO_HDR_DIRECTORY})
#include_directories(OpenCV_INCLUDE_DIR)

link_directories(${LIB_FOLDER} /usr/local/lib)

#set_target_properties(${TARGET_NAME} PROPERTIES "CMAKE_CXX_FLAGS" "${CMAKE_CXX_FLAGS} -fPIE"
#  COMPILE_PDB_NAME ${TARGET_NAME})

#target_include_directories(${TARGET_NAME} PUBLIC "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>")

target_link_libraries(${IE_SAMPLE_NAME} gflags cnpy TensorflowCC::Shared ${PROTOBUF_LIBRARIES} ${Boost_SYSTEM_LIBRARY})

if(CUDA_FOUND)
  target_link_libraries(${IE_SAMPLE_NAME} ${CUDA_LIBRARIES})
endif()

if(UNIX)
  target_link_libraries(${IE_SAMPLE_NAME} ${LIB_DL} pthread)
endif()