SET( CMAKE_SYSTEM_NAME          Linux )
SET( CMAKE_BUILD_TYPE           "Debug" CACHE STRING "")
# SET( CMAKE_BUILD_TYPE           "Release" )
SET( CMAKE_C_COMPILER           "arm-himix200-linux-gcc" CACHE STRING "")
SET( CMAKE_CXX_COMPILER         "arm-himix200-linux-g++" CACHE STRING "")

# set( ARCH                       "Hi3516" CACHE STRING "")
# SET( CMAKE_SYSTEM_PROCESSOR     "arm" CACHE STRING "")

SET( CMAKE_CXX_FLAGS           "${CMAKE_CXX_FLAGS} -std=c++11"   CACHE STRING "c++ flags" )
SET( CMAKE_C_FLAGS             "${CMAKE_C_FLAGS}"   CACHE STRING "c flags" )

SET( CMAKE_CXX_FLAGS_DEBUG      "${CMAKE_CXX_FLAGS_DEBUG} -O0 -Wall -g -fopenmp -ldl -lpthread -lrt" CACHE STRING "")
SET( CMAKE_CXX_FLAGS_RELEASE    "${CMAKE_CXX_FLAGS_RELEASE} -O3 -Wall -fopenmp -ldl -lpthread -lrt" CACHE STRING "")
