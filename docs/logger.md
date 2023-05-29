
### logger

测试函数

```c++


#include <iostream>
#include "logger.h"



int main(int argc, const char *argv[])
{

    //slog::default_logger()->info("this is default logger: {}", "ok");



    auto log = slog::make_logger("test", slog::LogLevel::Trace);


    slog::info("hello");
    slog::debug("this is debug");

    log->info("hello in level {}", 1222);    
    log->trace_data("hellosdfsdfsdfsdfsdfsdfwsd", 27, "this hex dump({}):", 27);

    
    auto another = slog::make_logger("another", slog::LogLevel::Trace);
    another->info("this another log");

    slog::get_logger("xxx")->info("use get_logger()");

    {
        auto tmp = slog::make_logger("sub", slog::LogLevel::Trace);
        tmp->info("log by logger-sub");
    }

    slog::drop_logger("test");

    slog::get_logger("another")->info("use get_logger()");
    slog::get_logger("test")->info("use get_logger()");
    slog::get_logger("sub")->info("use get_logger()");

    return 0;
}




```

CMakeLists.txt

```cmake


cmake_minimum_required(VERSION 3.20)

project(logger_test)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED True)

set(LIBS_INSTALL_DIR ${PROJECT_SOURCE_DIR}/../libs-install/${CMAKE_SYSTEM_PROCESSOR})

set(CMAKE_PREFIX_PATH ${LIBS_INSTALL_DIR})

find_package(spdlog REQUIRED)
find_package(fmt REQUIRED)

add_library(logger STATIC logger.cpp)
target_link_directories(logger PRIVATE ${LIBS_INSTALL_DIR})
target_include_directories(logger PRIVATE ${LIBS_INSTALL_DIR}/include)


add_executable(test test.cpp)
target_link_libraries(test logger fmt::fmt)


```