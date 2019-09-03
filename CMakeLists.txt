cmake_minimum_required(VERSION 3.5)
if(CMAKE_TOOLCHAIN_FILE)
    set(LIBRARY_OUTPUT_PATH_ROOT ${CMAKE_BINARY_DIR} CACHE PATH "root for library output, set this to change where android libs are compiled to")
    # get absolute path, but get_filename_component ABSOLUTE only refer with source dir, so find_file here :(
    get_filename_component(CMAKE_TOOLCHAIN_FILE_NAME ${CMAKE_TOOLCHAIN_FILE} NAME)
    find_file(CMAKE_TOOLCHAIN_FILE ${CMAKE_TOOLCHAIN_FILE_NAME} PATHS ${CMAKE_SOURCE_DIR} NO_DEFAULT_PATH)
    message(STATUS "CMAKE_TOOLCHAIN_FILE = ${CMAKE_TOOLCHAIN_FILE}")
endif()


project(DGIoTSDK)

set(CMAKE_C_COMPILER gcc)
set(CMAKE_CXX_COMPILER g++)

SET( CMAKE_CXX_FLAGS           "${CMAKE_CXX_FLAGS} -std=c++11"   CACHE STRING "c++ flags" )
SET( CMAKE_C_FLAGS             "${CMAKE_C_FLAGS}"   CACHE STRING "c flags" )

include_directories(
        ${CMAKE_CURRENT_SOURCE_DIR}/ThirdParty/paho_mqtt/include
        ${CMAKE_CURRENT_SOURCE_DIR}/ThirdParty/jsoncpp/include
        ${CMAKE_CURRENT_SOURCE_DIR}/ThirdParty/openssl/include
        ${CMAKE_CURRENT_SOURCE_DIR}/ThirdParty/glog/include
        ${CMAKE_CURRENT_SOURCE_DIR}/sdkApi
)

add_subdirectory(sdkApi)
add_subdirectory(Test)