
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
#include "slog_sink_spdlog.h"
#include "slog_sink_none.h"


namespace slog 
{

/**
 * @brief 创建一个Logger
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
 * @brief 创建一个Logger
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

}

#endif  // __SIMPLE_LOGGER_H__
