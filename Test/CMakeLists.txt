# IoTClient Sample
project(IoTSample)
cmake_minimum_required(VERSION 2.6)

include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/../IoTClient
    )
    
include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/../ThirdParty/paho_mqtt/include
    ${CMAKE_CURRENT_SOURCE_DIR}/../ThirdParty/jsoncpp/include
    ${CMAKE_CURRENT_SOURCE_DIR}/../ThirdParty/openssl/include
    ${CMAKE_CURRENT_SOURCE_DIR}/../ThirdParty/glog/include
    )

aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR} IoT_SRC)
aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR}/../IoTClient IoT_SRC)

#find_library(LIBMQTT3C NAMES libpaho-mqtt3cs-static.a PATHS ${CMAKE_CURRENT_SOURCE_DIR}/../ThirdParty/paho_mqtt/lib/arm-himix200-linux)
find_library(LIBMQTT3C NAMES libpaho-mqtt3c.a PATHS ${CMAKE_CURRENT_SOURCE_DIR}/../ThirdParty/paho_mqtt/lib/x86_64)
#find_library(LIBJSONCPP NAMES libjsoncpp.a PATHS ${CMAKE_CURRENT_SOURCE_DIR}/../ThirdParty/jsoncpp/lib/arm-himix200-linux)
find_library(LIBJSONCPP NAMES libjsoncpp.a PATHS ${CMAKE_CURRENT_SOURCE_DIR}/../ThirdParty/jsoncpp/lib/x86_64)
#find_library(LIBSSL NAMES libssl.so PATHS ${CMAKE_CURRENT_SOURCE_DIR}/../ThirdParty/openssl/lib/arm-himix200-linux)
#find_library(LIBCRYPTO NAMES libcrypto.so PATHS ${CMAKE_CURRENT_SOURCE_DIR}/../ThirdParty/openssl/lib/arm-himix200-linux)
find_library(LIBGLOG NAMES libglog.a PATHS ${CMAKE_CURRENT_SOURCE_DIR}/../ThirdParty/glog/lib/x86_64)

# add_library(IoTClient STATIC ${IoT_SRC})
add_executable(IoTSample ${IoT_SRC})

target_link_libraries(
    IoTSample

#    ${LIBCRYPTO}
#    ${LIBSSL}
    ${LIBMQTT3C}
    ${LIBJSONCPP}
    ${LIBGLOG}
    -lpthread
    -lgflags
)