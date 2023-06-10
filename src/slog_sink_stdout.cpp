

/**
 * @file logger_sink_spdlog.cpp
 * @author Liu Chuansen (samule@neptune-robotics.com)
 * @brief 这是一个使用SPDLOG实现的SINK
 * @version 0.1
 * @date 2023-05-31
 * 
 * @copyright Copyright (c) 2023
 * 
 */
#include <memory>
#include <string>

#include <chrono>
#include <ctime>
#include <iomanip>
#include <iostream>

#include <common/slog_sink_stdout.h>

namespace slog 
{

namespace sink 
{

// 全局的stdout的锁
static std::mutex s_mutex;


std::ostringstream time_stamp()
{
    auto now = std::chrono::system_clock::now();
    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(
        now.time_since_epoch()).count();
    std::time_t time = std::chrono::system_clock::to_time_t(now);
    std::tm tm = *std::localtime(&time);
    std::ostringstream oss;
    oss << std::put_time(&tm, "%Y-%m-%d %H:%M:%S.");
    oss << std::setfill('0') << std::setw(3) << ms % 1000;
    return oss;
}


bool Stdout::setup(std::string const & name)
{
    name_ = name;
    return true;
}

void Stdout::log(slog::LogLevel level, std::string const &msg)
{    
    // 等级不允许输出
    if (level < level_)
    {
        return;
    }
    auto now = std::chrono::system_clock::now();
    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(
        now.time_since_epoch()).count();
    std::time_t time = std::chrono::system_clock::to_time_t(now);
    std::tm tm = *std::localtime(&time);
    std::ostringstream oss;
    // output timestamp
    oss << std::put_time(&tm, "%Y-%m-%d %H:%M:%S.");
    oss << std::setfill('0') << std::setw(3) << ms % 1000;

#if 1
    // 颜色 
    #define _RESET   "\033[0m"
    #define _RED     "\033[0;31m"      /* Red */
    #define _GREEN   "\033[0;32m"      /* Green */
    #define _YELLOW  "\033[0;33m"      /* Yellow */
    #define _BLUE    "\033[0;34m"      /* Blue */
    #define _MAGENTA "\033[0;35m"      /* Magenta */
    #define _CYAN    "\033[0;36m"      /* Cyan */
    #define _WHITE   "\033[0;37m"      /* White */

    #define _SET_RESET  _RESET <<

    const char *color = _RESET;
    switch(level)
    {
        case LogLevel::Debug:
        color = _BLUE;
        break;
        case LogLevel::Info:
        color = _GREEN;
        break;
        case LogLevel::Warning:
        color = _YELLOW;
        break;
        case LogLevel::Error:
        color = _RED;
        break;

        default:    
        color = _RESET;    
        break;
    }
    oss << color;
#else 
    #define _SET_RESET 
#endif 

    // output log level  [T]
    oss << " [" << slog::log_level_short_name(level) << "]";

    // output logger name
    oss << " (" << name_ << ") ";

    //std::lock_guard<std::mutex> lock(mutex_);
    std::lock_guard<std::mutex> lock(s_mutex);
    std::cout << oss.str() << msg << _SET_RESET std::endl;
}

void Stdout::set_level(LogLevel level)
{
    level_ = level;
}

}

}
