

cmake_minimum_required(VERSION 3.10)

project(nos_common_libs)

get_filename_component(NOS_COMMON_DIR ${PROJECT_SOURCE_DIR}/.. ABSOLUTE)
set(LOCAL_INSTALL_DIR ${NOS_COMMON_DIR}/local-install/${CMAKE_SYSTEM_PROCESSOR})

message(STATUS "=> Local install at ${LOCAL_INSTALL_DIR}")

## 添加一上自定义目标，编译所有依赖库
add_custom_target(build_libs COMMAND python3 ${NOS_COMMON_DIR}/tools/build-libs.py ${LOCAL_INSTALL_DIR})

