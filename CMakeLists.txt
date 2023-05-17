
cmake_minimum_required(VERSION 3.10)

project(nos_common)

message(STATUS "=> enter project ${PROJECT_NAME}")
message(STATUS "=> PROJECT_SOURCE_DIR=${PROJECT_SOURCE_DIR}")
message(STATUS "=> PROJECT_BINARY_DIR=${PROJECT_BINARY_DIR}")

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED True)

find_package(spdlog REQUIRED)
find_package(fmt REQUIRED)
find_library(LIB_UV uv REQUIRED)

message(STATUS "=> set LIB_UV=${LIB_UV}")

set(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin)
set(LIBRARY_OUTPUT_PATH ${PROJECT_BINARY_DIR}/lib)

## set default include folder
include_directories(${PROJECT_SOURCE_DIR}/include)

## add src 
add_subdirectory(src)

## add test
add_subdirectory(test)
