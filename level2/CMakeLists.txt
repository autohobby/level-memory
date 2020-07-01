CMAKE_MINIMUM_REQUIRED(VERSION 2.8.2)
PROJECT(AHM)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_C_STANDARD 11)

SET(CMAKE_CXX_FLAGS_RELEASE "-Wall -Wno-unused-result -Wno-unused-but-set-variable -Wno-unused-variable -O3 -DUSE_GLOG=1 -std=c++11")
SET(CMAKE_CXX_FLAGS_DEBUG "-Wall -Wno-unused-result -Wno-unused-but-set-variable -Wno-unused-variable -g -O0 -std=c++11")

if (CMAKE_C_COMPILER_ID MATCHES "GNU|Clang")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -pedantic -Wall")
endif()

SET(PROJECT_ROOT_PATH "${CMAKE_SOURCE_DIR}")
MESSAGE(WARNING "${CMAKE_SOURCE_DIR}")
# SET(EXECUTABLE_OUTPUT_PATH "${PROJECT_BINARY_DIR}/bin")
SET(EXECUTABLE_OUTPUT_PATH "${PROJECT_BINARY_DIR}")
SET(TESTS_OUTPUT_PATH "${PROJECT_BINARY_DIR}/bin/tests")
SET(LIBRARY_OUTPUT_PATH "${PROJECT_BINARY_DIR}/lib/")
SET(CMAKE_SHARED_LINKER_FLAGS "-Wl,--export-all-symbols")
SET(LINK_FLAGS ${LINK_FLAGS} "-Wl,-whole-archive")
SET(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${EXECUTABLE_OUTPUT_PATH}")
MESSAGE(WARNING "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}")


# SET(THIRDPARTY_PATH "${PROJECT_ROOT_PATH}/thirdparty")
# MESSAGE(WARNING "${THIRDPARTY_PATH}")

INCLUDE_DIRECTORIES(
    "${PROJECT_ROOT_PATH}"
)

MESSAGE(STATUS "platform: LINUX")
SET(OpenCV_DIR ${OPENCV_ROOT_DIR})
FIND_PACKAGE(OpenCV 3 REQUIRED)

INCLUDE_DIRECTORIES(
    "${OpenCV_INSTALL_PATH}/include/"
)

LINK_DIRECTORIES(
    "${OpenCV_INSTALL_PATH}/lib/"
)

add_library(ahm ahm.c)
add_executable(ahm_write ahm_write.c)
target_link_libraries(ahm_write ahm)

add_executable(ahm_read ahm_read.c)
target_link_libraries(ahm_read ahm)

add_executable(e1array_write e1array_write.c)
target_link_libraries(e1array_write ahm)

add_executable(e1array_read e1array_read.c)
target_link_libraries(e1array_read ahm)

ADD_EXECUTABLE(video_write
    video_write.cpp
)

TARGET_LINK_LIBRARIES(video_write
    ahm
    pthread
    opencv_imgcodecs opencv_video opencv_core opencv_imgproc opencv_highgui
)

ADD_EXECUTABLE(video_read
    video_read.cpp
)

TARGET_LINK_LIBRARIES(video_read
    ahm
    pthread
    opencv_imgcodecs opencv_video opencv_core opencv_imgproc opencv_highgui
)