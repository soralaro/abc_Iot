cmake_minimum_required(VERSION 3.5)


set(CMAKE_CXX_STANDARD 11)
include_directories(/usr/local/include)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../IoTClient)


set (TARGET_NAME "DGIoTSDK")

file(GLOB_RECURSE cpp_files "*.cpp")
file(GLOB_RECURSE IoTClient_files ${CMAKE_CURRENT_SOURCE_DIR}/../IoTClient/*.cpp)
add_library(${TARGET_NAME} ${cpp_files} ${IoTClient_files})

#target_link_libraries(${TARGET_NAME} PRIVATE -lavcodec -lavformat -lavutil -lswscale  -lopencv_core -lopencv_highgui  -lopencv_imgproc -lopencv_imgcodecs)