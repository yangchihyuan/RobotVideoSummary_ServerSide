find_package(Protobuf REQUIRED)
find_package(Boost COMPONENTS system REQUIRED)
find_package(OpenCV COMPONENTS highgui REQUIRED)

set(PROTO_FILE AnalyzedResults.proto)
protobuf_generate_cpp(PROTO_SRC PROTO_HDR ${PROTO_FILE})
file (GLOB_RECURSE HEADERS ${CMAKE_CURRENT_SOURCE_DIR}/*.hpp)
set(SOURCES SocketToServer.cpp
            ProcessImage.cpp
            JPEG.cpp
            utility_directory.cpp
            utility_TimeRecorder.cpp
            utility_compute.cpp
            utility_string.cpp
            utility_csv.cpp
            8_openvino.cpp
            human_pose_estimator.cpp
            Pose.cpp
            ${PROTO_SRC}
)

add_demo(NAME 8_openvino
         SOURCES ${SOURCES}
         HEADERS ${HEADERS}
         DEPENDENCIES monitors models pipelines
         INCLUDE_DIRECTORIES "${CMAKE_CURRENT_SOURCE_DIR}/include"
         )

#enable debug information
set( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g")   

include_directories(${PROTOBUF_INCLUDE_DIRS})
get_filename_component(PROTO_HDR_DIRECTORY ${PROTO_HDR} DIRECTORY)
include_directories(${PROTO_HDR_DIRECTORY})
include_directories(OpenCV_INCLUDE_DIR)

link_directories(${LIB_FOLDER} /usr/local/lib)

set_target_properties(${TARGET_NAME} PROPERTIES "CMAKE_CXX_FLAGS" "${CMAKE_CXX_FLAGS}")


target_link_libraries(${OMZ_DEMO_NAME} PRIVATE ${PROTOBUF_LIBRARIES})
