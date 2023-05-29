
cmake_minimum_required(VERSION 3.10)

project(nos_common)

message(STATUS "=> enter project ${PROJECT_NAME}")
message(STATUS "=> PROJECT_SOURCE_DIR=${PROJECT_SOURCE_DIR}")
message(STATUS "=> PROJECT_BINARY_DIR=${PROJECT_BINARY_DIR}")

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED True)

set(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin)
set(LIBRARY_OUTPUT_PATH ${PROJECT_BINARY_DIR}/lib)

set(LIBS_INSTALL_DIR ${PROJECT_SOURCE_DIR}/libs-install/${CMAKE_SYSTEM_PROCESSOR})
set(LOCAL_INSTALL_DIR ${PROJECT_SOURCE_DIR}/local-install/${CMAKE_SYSTEM_PROCESSOR})

message(STATUS "=> Local install at ${LOCAL_INSTALL_DIR}")

# CMAKE_PREFIX_PATH是一个环境变量，用于指定CMake可以搜索软件或库的目录列表。
# 它可以被find_package()、find_program()、find_library()、find_file()和find_path()等命令使用。
# 这些命令将根据自己的文档添加bin、lib或include等子目录。CMAKE_PREFIX_PATH可以包含单个前缀或多个前缀，
# UNIX上用冒号分隔，Windows上用分号分隔。默认情况下，它为空。它旨在由项目设置。
set(CMAKE_PREFIX_PATH ${LIBS_INSTALL_DIR})

find_package(libuv REQUIRED)
find_package(spdlog REQUIRED)
find_package(fmt REQUIRED)
find_package(docopt REQUIRED)

# message(STATUS "=> libuv:${libuv_DIR}")
# message(STATUS "=> libuv:${libuv_INCLUDE_DIRS}")
# message(STATUS "=> libuv:${libuv_LIBS}")

## 安装目录
set(CMAKE_INSTALL_PREFIX ${LOCAL_INSTALL_DIR})

## set default include folder
include_directories(
    ${PROJECT_SOURCE_DIR}/include
    ${LIBS_INSTALL_DIR}/include
    )
## add local libs dir
link_directories(${LIBS_INSTALL_DIR}/lib)

## add src 
add_subdirectory(src)

## add test
add_subdirectory(test)

## 添加一上自定义目标，编译所有依赖库
#add_custom_target(build_libs COMMAND python3 ${PROJECT_SOURCE_DIR}/tools/build-libs.py ${CMAKE_INSTALL_PREFIX})
## 生成一个版本 - 
#add_custom_command(TARGET install POST_BUILD COMMAND ${PROJECT_SOURCE_DIR}/tools/setlocalversion ${PROJECT_SOURCE_DIR} > ${LOCAL_INSTALL_DIR}/version)
#