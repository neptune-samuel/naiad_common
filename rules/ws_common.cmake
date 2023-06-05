
## 本文件规则用来设定工作空间的环境变量(WS_XXX)
## 生成以下变量
## 工作区相关： WS_ROOT_DIR WS_ROS WS_INSTALL_PREFIX
## ROS相关：ROS_ROOT_DIR ROS_FOXY  ROS_DISTRO

## 得到工作空间的根目录，通过ROS的环境变量判断
if (NOT NO_ROS)
execute_process(COMMAND "printenv" "ROS_DISTRO" OUTPUT_VARIABLE ROS_DISTRO)
string(REPLACE "\n" "" ROS_DISTRO "${ROS_DISTRO}")
endif()

if (ROS_DISTRO STREQUAL "foxy")
set(ROS_FOXY  ON)
set(WS_ROS ON)
# 设置ROS根目录
set(ROS_ROOT_DIR /opt/ros/${ROS_DISTRO})
endif()

## 在编译目录可以设定根目录，这里可以不用设置
if (WS_ROS)
## 在ROS环境下 
get_filename_component(WS_ROOT_DIR ${PROJECT_SOURCE_DIR}/../../ ABSOLUTE)
# 设定安装目录
#set(WS_INSTALL_PREFIX ${WS_ROOT_DIR}/install/${PROJECT_NAME})
set(WS_INSTALL_PREFIX ${WS_ROOT_DIR}/install)
else()
## 不在ROS环境下 
get_filename_component(WS_ROOT_DIR ${PROJECT_SOURCE_DIR}/../ ABSOLUTE)
## 设定安装目录
set(WS_INSTALL_PREFIX ${WS_ROOT_DIR}/install/${CMAKE_SYSTEM_PROCESSOR})
endif()

message(STATUS "=> ROS_DISTRO=${ROS_DISTRO}")
message(STATUS "=> ROS_FOXY=${ROS_FOXY}")
message(STATUS "=> WS_ROOT_DIR=${WS_ROOT_DIR}")
message(STATUS "=> WS_INSTALL_PREFIX=${WS_INSTALL_PREFIX}")

set(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin)
set(LIBRARY_OUTPUT_PATH ${PROJECT_BINARY_DIR}/lib)
