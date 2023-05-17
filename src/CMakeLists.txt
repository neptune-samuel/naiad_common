
# message(STATUS "CMAKE_SOURCE_DIR=${CMAKE_SOURCE_DIR}")
# message(STATUS "PROJECT_SOURCE_DIR=${PROJECT_SOURCE_DIR}")
# message(STATUS "PROJECT_BINARY_DIR=${PROJECT_BINARY_DIR}")
# message(STATUS "LIBUV=${LIBUV}")

add_library(noscommon STATIC logger.cpp serial_port.cpp tcp_server.cpp)

target_link_libraries(noscommon PRIVATE spdlog::spdlog ${LIBUV})


