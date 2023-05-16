#ifndef __NOS_LOGGER_H__
#define __NOS_LOGGER_H__

/**
 * @file logger.h
 * @author Liu Chuansen (samule@neptune-robotics.com)
 * @brief 使用spdlog封装日志函数
 * @version 0.1
 * @date 2023-05-15
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#include <spdlog/spdlog.h>
#include <spdlog/fmt/bin_to_hex.h>

namespace nos
{

/**
 * @brief 初始化logger
 * 
 * @param logger_name 日志名称
 * @param log_level
 * @param file_path 日志路径
 * @param file_size 日志文件最大大小
 * @param file_num  日志文件回滚数量
 */
void logger_init(const std::string &logger_name, spdlog::level::level_enum log_level, const std::string &file_path, int file_size = 1024 * 1024, int file_num = 2);

/**
 * @brief 初始化日志，使用默认文件名，默认关闭文件日志
 * 
 * @param logger_name 日志名称
 * @param log_level 日志等级
 * @param log_to_file 是否启用文件日志
 */
void logger_init(const std::string &logger_name, spdlog::level::level_enum log_level, bool log_to_file = false);

} // end nos


// 提供公共函数. 使用全局的命名空间
// namespace nos
// {

template<typename... Args>
inline void trace(fmt::format_string<Args...> fmt, Args &&...args)
{
    spdlog::default_logger_raw()->trace(fmt, std::forward<Args>(args)...);
}

template<typename... Args>
inline void dlog(fmt::format_string<Args...> fmt, Args &&...args)
{
    spdlog::default_logger_raw()->debug(fmt, std::forward<Args>(args)...);
}

template<typename... Args>
inline void ilog(fmt::format_string<Args...> fmt, Args &&...args)
{
    spdlog::default_logger_raw()->info(fmt, std::forward<Args>(args)...);
}

template<typename... Args>
inline void wlog(fmt::format_string<Args...> fmt, Args &&...args)
{
    spdlog::default_logger_raw()->warn(fmt, std::forward<Args>(args)...);
}

template<typename... Args>
inline void elog(fmt::format_string<Args...> fmt, Args &&...args)
{
    spdlog::default_logger_raw()->error(fmt, std::forward<Args>(args)...);
}


//}

#endif // __NOS_LOGGER_H__
