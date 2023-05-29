
#ifndef __SPDLOG_LOGGER_H__
#define __SPDLOG_LOGGER_H__

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

#include <memory>
#include <string>
#include <type_traits>
#include <cstdio>

#include <fmt/core.h>
#include <fmt/format.h>

namespace spdlog
{
    class logger;

    namespace level {
        enum level_enum : int;
    }
}

namespace slog 
{
 
enum class LogLevel : int 
{
    Trace = 0,
    Debug,
    Info,
    Warning,
    Error,
    Off,
    None = Off + 2,
};


class Logger
{

public:

    Logger(std::string const &name, LogLevel level, std::string const &log_file, int file_size = 1024*1024, int file_num = 4);
    Logger(std::string const &name, LogLevel level) : Logger(name, level, "", 0, 0) { }    
    explicit Logger(std::string const &name) : Logger(name, LogLevel::Info, "", 0, 0) { }

    // 禁止复制构造
    Logger(Logger const &) = delete;
    Logger & operator=(Logger const &) = delete;


    ~Logger();

    /// @brief 返回名称
    /// @return 
    std::string const &name();

    /// @brief 显示指定日志
    /// @param level 日志等级
    /// @param msg 日志消息
    void log(LogLevel level, std::string const &msg);

    /// @brief 显示十六进制数据
    /// @param level 日志等级
    /// @param data 数据地址
    /// @param size 数据大小
    /// @param msg 日志消息
    void dump(LogLevel level, void const *data, int size, std::string const &msg);

    template<typename... Args>
    void trace(fmt::format_string<Args...> fmt, Args &&... args)
    {
        log(LogLevel::Trace, fmt::format(fmt, std::forward<Args>(args)...));
    }

    template<typename... Args>
    void debug(fmt::format_string<Args...> fmt, Args &&... args)
    {
        log(LogLevel::Debug, fmt::format(fmt, std::forward<Args>(args)...));        
    }

    template<typename... Args>
    void info(fmt::format_string<Args...> fmt, Args &&... args)
    {
        log(LogLevel::Info, fmt::format(fmt, std::forward<Args>(args)...));        
    }    

    template<typename... Args>
    void warning(fmt::format_string<Args...> fmt, Args &&... args)
    {
        log(LogLevel::Warning, fmt::format(fmt, std::forward<Args>(args)...));        
    }    

    template<typename... Args>
    void error(fmt::format_string<Args...> fmt, Args &&... args)
    {
        log(LogLevel::Error, fmt::format(fmt, std::forward<Args>(args)...));        
    }

    template<typename... Args>
    void trace_data(void const *data, int size, fmt::format_string<Args...> fmt, Args &&... args)
    {
        dump(LogLevel::Trace, data, size, fmt::format(fmt, std::forward<Args>(args)...));
    }

    template<typename... Args>
    void debug_data(void const *data, int size, fmt::format_string<Args...> fmt, Args &&... args)
    {
        dump(LogLevel::Debug, data, size, fmt::format(fmt, std::forward<Args>(args)...));        
    }

    template<typename... Args>
    void info_data(void const *data, int size, fmt::format_string<Args...> fmt, Args &&... args)
    {
        dump(LogLevel::Info, data, size, fmt::format(fmt, std::forward<Args>(args)...));        
    }    

    template<typename... Args>
    void warning_data(void const *data, int size, fmt::format_string<Args...> fmt, Args &&... args)
    {
        dump(LogLevel::Warning, data, size, fmt::format(fmt, std::forward<Args>(args)...));        
    }    

    template<typename... Args>
    void error_data(void const *data, int size, fmt::format_string<Args...> fmt, Args &&... args)
    {
        dump(LogLevel::Error, data, size, fmt::format(fmt, std::forward<Args>(args)...));        
    }

private:
    std::shared_ptr<spdlog::logger> logger_;    

    /// 转换为spdlog的日志等级
    spdlog::level::level_enum to_spdlog_level(LogLevel level);
};



/**
 * @brief 获取默认的Logger
 * 
 * @return Logger& 
 */
 std::shared_ptr<Logger> default_logger();

/**
 * @brief 是否存在logger
 * 
 * @param name 
 * @return true 
 */
bool has_logger(std::string const & name);

/**
 * @brief 返回一个logger，有可能为空
 * 
 * @param name 
 * @return std::shared_ptr<Logger> 
 */
std::shared_ptr<Logger> get_logger(std::string const & name);

/**
 * @brief 注册到全局使用
 * 
 * @param logger 
 * @return true 
 * @return false 
 */
bool register_logger(std::shared_ptr<Logger> logger);

/**
 * @brief 丢弃一个logger
 * 
 * @param name 
 */
void drop_logger(std::string const &name);

/**
 * @brief 创建一个Logger
 * 
 * @param name 
 * @param level 
 * @param log_file 
 * @param file_size 
 * @param file_num 
 * @return std::shared_ptr<Logger> 
 */
std::shared_ptr<Logger> make_logger(std::string const &name, LogLevel level, std::string const &log_file, int file_size, int file_num);

/**
 * @brief 创建一个Logger
 * 
 * @param name 
 * @param level 
 * @return std::shared_ptr<Logger> 
 */
std::shared_ptr<Logger> make_logger(std::string const &name, LogLevel level);


template<typename... Args>
inline void trace(fmt::format_string<Args...> fmt, Args &&...args)
{
    default_logger()->log(LogLevel::Trace, fmt::format(fmt, std::forward<Args>(args)...));
}

template<typename... Args>
inline void debug(fmt::format_string<Args...> fmt, Args &&...args)
{
    default_logger()->log(LogLevel::Debug, fmt::format(fmt, std::forward<Args>(args)...));
}

template<typename... Args>
inline void info(fmt::format_string<Args...> fmt, Args &&...args)
{
    default_logger()->log(LogLevel::Info, fmt::format(fmt, std::forward<Args>(args)...));
}

template<typename... Args>
inline void warning(fmt::format_string<Args...> fmt, Args &&...args)
{
    default_logger()->log(LogLevel::Warning, fmt::format(fmt, std::forward<Args>(args)...));
}

template<typename... Args>
inline void error(fmt::format_string<Args...> fmt, Args &&...args)
{
    default_logger()->log(LogLevel::Error, fmt::format(fmt, std::forward<Args>(args)...));
}


template<typename... Args>
inline void trace_data(void const *data, int size, fmt::format_string<Args...> fmt, Args &&... args)
{
    default_logger()->dump(LogLevel::Trace, data, size, fmt::format(fmt, std::forward<Args>(args)...));
}

template<typename... Args>
inline void debug_data(void const *data, int size, fmt::format_string<Args...> fmt, Args &&... args)
{
    default_logger()->dump(LogLevel::Debug, data, size, fmt::format(fmt, std::forward<Args>(args)...));        
}

template<typename... Args>
inline void info_data(void const *data, int size, fmt::format_string<Args...> fmt, Args &&... args)
{
    default_logger()->dump(LogLevel::Info, data, size, fmt::format(fmt, std::forward<Args>(args)...));        
}    

template<typename... Args>
inline void warning_data(void const *data, int size, fmt::format_string<Args...> fmt, Args &&... args)
{
    default_logger()->dump(LogLevel::Warning, data, size, fmt::format(fmt, std::forward<Args>(args)...));        
}    

template<typename... Args>
inline void error_data(void const *data, int size, fmt::format_string<Args...> fmt, Args &&... args)
{
    default_logger()->dump(LogLevel::Error, data, size, fmt::format(fmt, std::forward<Args>(args)...));        
}


}

#endif  // __SPDLOG_LOGGER_H__
