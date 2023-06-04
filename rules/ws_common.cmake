
## 本文件规则用来设定工作空间的环境变量(WS_XXX)
## 生成以下变量
## WS_ROOT_DIR  WS_ROS WS_ROS_FOXY  WS_ROS_DISTRO WS_INSTALL_PREFIX
## 
## 得到工作空间的根目录，通过ROS的环境变量判断
if (NOT NO_ROS)
execute_process(COMMAND "printenv" "ROS_DISTRO" OUTPUT_VARIABLE WS_ROS_DISTRO)
string(REPLACE "\n" "" WS_ROS_DISTRO "${ROS_DISTRO}")
endif()

if (WS_ROS_DISTRO STREQUAL "foxy")
set(WS_ROS_FOXY  ON)
set(WS_ROS ON)
endif()

## 在编译目录可以设定根目录，这里可以不用设置
if (WS_ROS)
## 在ROS环境下 
get_filename_component(WS_ROOT_DIR ${PROJECT_SOURCE_DIR}/../../ ABSOLUTE)
# 设定安装目录
set(WS_INSTALL_PREFIX ${WS_ROOT_DIR}/install/${PROJECT_NAME})
else()
## 不在ROS环境下 
get_filename_component(WS_ROOT_DIR ${PROJECT_SOURCE_DIR}/../ ABSOLUTE)
## 设定安装目录
set(WS_INSTALL_PREFIX ${WS_ROOT_DIR}/install/${CMAKE_SYSTEM_PROCESSOR})
endif()

message(STATUS "=> WS_ROOT_DIR=${WS_ROOT_DIR}")
message(STATUS "=> WS_ROS_DISTRO=${WS_ROS_DISTRO}")
message(STATUS "=> WS_ROS_FOXY=${WS_ROS_FOXY}")
message(STATUS "=> WS_INSTALL_PREFIX=${WS_INSTALL_PREFIX}")

set(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin)
set(LIBRARY_OUTPUT_PATH ${PROJECT_BINARY_DIR}/lib)
