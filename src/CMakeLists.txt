option(SLOG_SINK_ROS "logger for ros" OFF)
option(SLOG_SINK_SPDLOG "logger with spdlog" OFF)

if (WS_ROS)
set(SLOG_SINK_ROS ON)
endif()

set(SLOG_SRCS slog_logger.cpp slog_sink_stdout.cpp)

if(SLOG_SINK_SPDLOG)
list(APPEND SLOG_OPTIONS -DSLOG_SINK_SPDLOG)
list(APPEND SLOG_SRCS slog_sink_spdlog.cpp)
endif()

if(SLOG_SINK_ROS)
list(APPEND SLOG_OPTIONS -DSLOG_SINK_ROS)
endif()

## build liblogger
add_library(logger STATIC ${SLOG_SRCS})
## build libcommon
add_library(common STATIC uv_helper.cpp serial_port.cpp tcp_server.cpp ${SLOG_SRCS})
## 指定编译选项
target_compile_options(logger PUBLIC ${SLOG_OPTIONS})
target_compile_options(common PUBLIC ${SLOG_OPTIONS})


# 安装目标，并导出targets
install(TARGETS logger common
    EXPORT common-targets
    DESTINATION lib)

install(EXPORT common-targets
    FILE common-targets.cmake 
    DESTINATION lib/cmake/common)

## 安装头文件
install(DIRECTORY ${PROJECT_SOURCE_DIR}/include/ DESTINATION include)

include(CMakePackageConfigHelpers)

## 输出配置文件，供find_package使用
configure_package_config_file(
    ${RULES_DIR}/common-config.cmake.in 
    ${CMAKE_CURRENT_BINARY_DIR}/common-config.cmake 
    INSTALL_DESTINATION  lib/cmake/common
    NO_SET_AND_CHECK_MACRO
    NO_CHECK_REQUIRED_COMPONENTS_MACRO)

# 输出CMAKE PACKAGE版本号
write_basic_package_version_file(
    ${CMAKE_CURRENT_BINARY_DIR}/common-config-version.cmake 
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY AnyNewerVersion)

## 安装生成的版本文件和配置文件
install(FILES
    ${CMAKE_CURRENT_BINARY_DIR}/common-config.cmake
    ${CMAKE_CURRENT_BINARY_DIR}/common-config-version.cmake 
    DESTINATION lib/cmake/common)
