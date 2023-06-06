
#ifndef __SIMPLE_LOGGER_H__
#define __SIMPLE_LOGGER_H__

/**
 * @file logger.h
 * @author Liu Chuansen (samule@neptune-robotics.com)
 * @brief 重新封装Spdlog为简单的类实现
 * @version 0.1
 * @date 2023-05-25
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#include "slog_logger.h"
#include "slog_sink_stdout.h"
#include "slog_sink_none.h"

#ifdef SLOG_SINK_SPDLOG
#include "slog_sink_spdlog.h"
#endif 

#ifdef SLOG_SINK_ROS
#include "slog_sink_ros.h"
#endif 


namespace slog 
{


/**
 * @brief 创建一个空SINK，不输出任何信息
 * 
 * @param name 
 * @return std::shared_ptr<Logger> 
 */
static inline std::shared_ptr<Logger> make_none_logger(std::string const &name)
{
    return make_logger(name, std::make_shared<sink::LogNone>());
}

/**
 * @brief 创建一个标准输出的日志，线程安全
 * 
 * @param name 
 * @param level 日志等级
 * @return std::shared_ptr<Logger> 
 */
static inline std::shared_ptr<Logger> make_stdout_logger(std::string const &name, LogLevel level)
{
    return make_logger(name, std::make_shared<sink::Stdout>(level));
}

#ifdef SLOG_SINK_SPDLOG
/**
 * @brief 创建一个Logger，输出到console
 * 
 * @param name 
 * @param sink 指定一个sink
 * @return std::shared_ptr<Logger> 
 */
static inline std::shared_ptr<Logger> make_spdlog_logger(std::string const &name, LogLevel level)
{
    return make_logger(name, std::make_shared<sink::SpdlogToConsole>(level));
}


/**
 * @brief 创建一个Logger,使用Spdlog，支持文件
 * 
 * @param name 
 * @param sink 指定一个sink
 * @return std::shared_ptr<Logger> 
 */
static inline std::shared_ptr<Logger> make_spdlog_logger(std::string const &name, LogLevel level,
    std::string const &log_file, size_t file_size = 1024 * 1024, size_t file_num = 4)
{
    return make_logger(name, std::make_shared<sink::SpdlogToFile>(level, log_file, file_size, file_num));
}
#endif 

#ifdef SLOG_SINK_ROS
static inline std::shared_ptr<Logger> make_ros_logger(std::string const &name)
{
    return make_logger(name, std::make_shared<sink::RosLogger>(name));
}
#endif 

}

#endif  // __SIMPLE_LOGGER_H__
