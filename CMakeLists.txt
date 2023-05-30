
#
# cmake rules for nos_common
# Author: LiuChuansen
# Date: 20230512
# 
# 
cmake_minimum_required(VERSION 3.13)

project(nos_common)

# Default to C99
if(NOT CMAKE_C_STANDARD)
  set(CMAKE_C_STANDARD 99)
endif()

# Default to C++14
if(NOT CMAKE_CXX_STANDARD)
  set(CMAKE_CXX_STANDARD 14)
endif()

if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()

## 得到工作空间的根目录，通过ROS的环境变量判断
if (NOT ROS_DISABLE)
execute_process(COMMAND "printenv" "ROS_DISTRO" OUTPUT_VARIABLE ROS_DISTRO)
string(REPLACE "\n" "" ROS_DISTRO "${ROS_DISTRO}")
endif()

if (ROS_DISTRO STREQUAL "foxy")
set(ROS_FOXY  ON)
set(ROS2 ON)
get_filename_component(WS_ROOT_DIR ${PROJECT_SOURCE_DIR}/../../ ABSOLUTE)
else()
get_filename_component(WS_ROOT_DIR ${PROJECT_SOURCE_DIR}/../ ABSOLUTE)
endif()

message(STATUS "=> WS_ROOT_DIR=${WS_ROOT_DIR}")
message(STATUS "=> ROS_DISTRO=${ROS_DISTRO}")
message(STATUS "=> ROS_FOXY=${ROS_FOXY}")

## 设定工作空间安装目录
if (NOT WS_INSTALL_PREFIX)
if (ROS2)
set(WS_INSTALL_PREFIX      ${WS_ROOT_DIR}/install/${PROJECT_NAME})
else()
set(WS_INSTALL_PREFIX      ${WS_ROOT_DIR}/install/${CMAKE_SYSTEM_PROCESSOR})
endif()
endif()

## 添加依赖的第三方库目录
if (ROS2)
set(WS_LIBRARY_PREFIX ${WS_ROOT_DIR}/install/nos_library)
else()
set(WS_LIBRARY_PREFIX ${WS_INSTALL_PREFIX})
endif()

## 设定默认的安装目录
## 如果没有设定INSTALL_PREFIX, 默认安装到工作空间
if (NOT ROS2)
if (NOT INSTALL_PREFIX)
set(CMAKE_INSTALL_PREFIX ${WS_INSTALL_PREFIX})
else()
set(CMAKE_INSTALL_PREFIX ${INSTALL_PREFIX})
endif()
endif()

message(STATUS "=> WS_INSTALL_PREFIX=${WS_INSTALL_PREFIX}")
message(STATUS "=> WS_LIBRARY_PREFIX=${WS_LIBRARY_PREFIX}")
message(STATUS "=> CMAKE_INSTALL_PREFIX=${CMAKE_INSTALL_PREFIX}")

if (ROS2)
# find dependencies
find_package(ament_cmake REQUIRED)
find_package(rclcpp REQUIRED)

if(BUILD_TESTING)
  find_package(ament_lint_auto REQUIRED)
  # the following line skips the linter which checks for copyrights
  # uncomment the line when a copyright and license is not present in all source files
  #set(ament_cmake_copyright_FOUND TRUE)
  # the following line skips cpplint (only works in a git repo)
  # uncomment the line when this package is not in a git repo
  #set(ament_cmake_cpplint_FOUND TRUE)
  ament_lint_auto_find_test_dependencies()
endif()

endif()

find_package(libuv REQUIRED PATHS ${WS_LIBRARY_PREFIX})
find_package(spdlog REQUIRED PATHS ${WS_LIBRARY_PREFIX})
find_package(fmt REQUIRED PATHS ${WS_LIBRARY_PREFIX})
find_package(docopt REQUIRED PATHS ${WS_LIBRARY_PREFIX})

message(STATUS "spdlog_FOUND: ${spdlog_FOUND}")
message(STATUS "spdlog_VERSION: ${spdlog_VERSION}")
message(STATUS "spdlog_INCLUDE_DIRS: ${spdlog_INCLUDE_DIRS}")
message(STATUS "spdlog_LIBRARIES: ${spdlog_LIBRARIES}")
message(STATUS "spdlog_LIBRARY: ${spdlog_LIBRARY}")
message(STATUS "spdlog_DEFINITIONS: ${spdlog_DEFINITIONS}")

message(STATUS "libuv_FOUND: ${libuv_FOUND}")
message(STATUS "libuv_VERSION: ${libuv_VERSION}")
message(STATUS "libuv_INCLUDE_DIRS: ${libuv_INCLUDE_DIRS}")
message(STATUS "libuv_LIBRARIES: ${libuv_LIBRARIES}")
message(STATUS "libuv_LIBRARY: ${libuv_LIBRARY}")
message(STATUS "libuv_DEFINITIONS: ${libuv_DEFINITIONS}")

message(STATUS "docopt_FOUND: ${docopt_FOUND}")
message(STATUS "docopt_VERSION: ${docopt_VERSION}")
message(STATUS "docopt_INCLUDE_DIRS: ${docopt_INCLUDE_DIRS}")
message(STATUS "docopt_LIBRARIES: ${docopt_LIBRARIES}")
message(STATUS "docopt_LIBRARY: ${docopt_LIBRARY}")
message(STATUS "docopt_DEFINITIONS: ${docopt_DEFINITIONS}")

message(STATUS "fmt_FOUND: ${fmt_FOUND}")
message(STATUS "fmt_VERSION: ${fmt_VERSION}")
message(STATUS "fmt_INCLUDE_DIRS: ${fmt_INCLUDE_DIRS}")
message(STATUS "fmt_LIBRARIES: ${fmt_LIBRARIES}")
message(STATUS "fmt_LIBRARY: ${fmt_LIBRARY}")
message(STATUS "fmt_DEFINITIONS: ${fmt_DEFINITIONS}")

## set default include folder
include_directories(
    ${PROJECT_SOURCE_DIR}/include
    ${WS_LIBRARY_PREFIX}/include
    )

## add local libs dir
link_directories(${WS_LIBRARY_PREFIX}/lib)

## add src 
add_subdirectory(src)

# add test
add_subdirectory(test)

if (ROS2)
ament_package()
endif()
