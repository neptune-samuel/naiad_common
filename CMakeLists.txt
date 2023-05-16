
cmake_minimum_required(VERSION 3.10)

project(nos-common)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED True)

find_package(spdlog REQUIRED)
find_package(fmt REQUIRED)
find_library(LIBUV uv REQUIRED)


message(STATUS "LIBUV=${LIBUV}")

set(LOCAL_SOURCE_DIR ${CMAKE_SOURCE_DIR}/src)
set(LOCAL_INCLUDE_DIR ${CMAKE_SOURCE_DIR}/include)
#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -lpthread")

list(APPEND LOCAL_SOURCES ${LOCAL_SOURCE_DIR}/logger.cpp)
list(APPEND LOCAL_SOURCES ${LOCAL_SOURCE_DIR}/tcp_server.cpp)
list(APPEND LOCAL_SOURCES ${LOCAL_SOURCE_DIR}/serial_port.cpp)
#list(APPEND LOCAL_SOURCES ${LOCAL_SOURCE_DIR}/main.cpp)

add_executable(main_test ${LOCAL_SOURCES} ${LOCAL_SOURCE_DIR}/main.cpp)

target_include_directories(main_test
    PRIVATE ${LOCAL_INCLUDE_DIR}
)

target_link_libraries(main_test PRIVATE spdlog::spdlog ${LIBUV})


add_executable(serial_test ${LOCAL_SOURCES} ${LOCAL_SOURCE_DIR}/test_serial.cpp)

target_include_directories(serial_test
    PRIVATE ${LOCAL_INCLUDE_DIR}
)

target_link_libraries(serial_test PRIVATE spdlog::spdlog ${LIBUV})

# add_executable(libuv_test ${LOCAL_SOURCE_DIR}/test_libuv.cpp ${LOCAL_SOURCE_DIR}/logger.cpp)
# target_include_directories(libuv_test
#     PRIVATE ${LOCAL_INCLUDE_DIR}
# )
# target_link_libraries(libuv_test PRIVATE spdlog::spdlog  ${LIBUV})


