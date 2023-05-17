
cmake_minimum_required(VERSION 3.10)

project(nos-common)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED True)

find_package(spdlog REQUIRED)
find_package(fmt REQUIRED)
find_library(LIBUV uv REQUIRED)

message(STATUS "LIBUV=${LIBUV}")

set(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin)
set(LIBRARY_OUTPUT_PATH ${PROJECT_BINARY_DIR}/lib)

## set default include folder
include_directories(${PROJECT_SOURCE_DIR}/include)

## add src 
add_subdirectory(src)

## add test
add_subdirectory(test)
