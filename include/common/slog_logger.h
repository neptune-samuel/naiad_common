
#ifndef __SIMPLE_LOGGER_MAIN_H__
#define __SIMPLE_LOGGER_MAIN_H__

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
#include <vector>
#include <type_traits>
#include <cstdio>

#include <fmt/core.h>
#include <fmt/format.h>

namespace slog 
{

/// 日志等级
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

class LoggerSink
{
public:    
    virtual ~LoggerSink() {};

    virtual bool setup(std::string const & logger_name) = 0;
    virtual void log(LogLevel level, std::string const & msg) = 0;
    virtual void set_level(LogLevel level) = 0;
    virtual const char * name() = 0;
};


class Logger
{

public:

    Logger(std::string const &name, std::shared_ptr<LoggerSink> sink = nullptr);

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
    void dump(LogLevel level, void const *data, size_t size, std::string const &msg);

    /// @brief 显示vector中的数据
    /// @param level 
    /// @param data 
    /// @param msg 
    void dump(LogLevel level, std::vector<uint8_t> const & data, std::string const &msg);

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
    void trace_data(void const *data, size_t size, fmt::format_string<Args...> fmt, Args &&... args)
    {
        dump(LogLevel::Trace, data, size, fmt::format(fmt, std::forward<Args>(args)...));
    }

    template<typename... Args>
    void debug_data(void const *data, size_t size, fmt::format_string<Args...> fmt, Args &&... args)
    {
        dump(LogLevel::Debug, data, size, fmt::format(fmt, std::forward<Args>(args)...));        
    }

    template<typename... Args>
    void info_data(void const *data, size_t size, fmt::format_string<Args...> fmt, Args &&... args)
    {
        dump(LogLevel::Info, data, size, fmt::format(fmt, std::forward<Args>(args)...));        
    }    

    template<typename... Args>
    void warning_data(void const *data, size_t size, fmt::format_string<Args...> fmt, Args &&... args)
    {
        dump(LogLevel::Warning, data, size, fmt::format(fmt, std::forward<Args>(args)...));        
    }    

    template<typename... Args>
    void error_data(void const *data, size_t size, fmt::format_string<Args...> fmt, Args &&... args)
    {
        dump(LogLevel::Error, data, size, fmt::format(fmt, std::forward<Args>(args)...));        
    }

    template<typename... Args>
    void trace_data(std::vector<uint8_t> const &data, fmt::format_string<Args...> fmt, Args &&... args)
    {
        dump(LogLevel::Trace, data, fmt::format(fmt, std::forward<Args>(args)...));
    }

    template<typename... Args>
    void debug_data(std::vector<uint8_t> const &data, fmt::format_string<Args...> fmt, Args &&... args)
    {
        dump(LogLevel::Debug, data, fmt::format(fmt, std::forward<Args>(args)...));        
    }

    template<typename... Args>
    void info_data(std::vector<uint8_t> const &data, fmt::format_string<Args...> fmt, Args &&... args)
    {
        dump(LogLevel::Info, data, fmt::format(fmt, std::forward<Args>(args)...));        
    }    

    template<typename... Args>
    void warning_data(std::vector<uint8_t> const &data, fmt::format_string<Args...> fmt, Args &&... args)
    {
        dump(LogLevel::Warning, data, fmt::format(fmt, std::forward<Args>(args)...));        
    }    

    template<typename... Args>
    void error_data(std::vector<uint8_t> const &data, fmt::format_string<Args...> fmt, Args &&... args)
    {
        dump(LogLevel::Error, data, fmt::format(fmt, std::forward<Args>(args)...));        
    }

private:
    std::string name_;
    std::shared_ptr<LoggerSink> sink_;
    bool valid_;
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
 * @param sink 指定一个sink
 * @return std::shared_ptr<Logger> 
 */
std::shared_ptr<Logger> make_logger(std::string const &name, std::shared_ptr<LoggerSink> sink);


// 以下是模板函数，实现动态参数

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
inline void trace_data(void const *data, size_t size, fmt::format_string<Args...> fmt, Args &&... args)
{
    default_logger()->dump(LogLevel::Trace, data, size, fmt::format(fmt, std::forward<Args>(args)...));
}

template<typename... Args>
inline void debug_data(void const *data, size_t size, fmt::format_string<Args...> fmt, Args &&... args)
{
    default_logger()->dump(LogLevel::Debug, data, size, fmt::format(fmt, std::forward<Args>(args)...));        
}

template<typename... Args>
inline void info_data(void const *data, size_t size, fmt::format_string<Args...> fmt, Args &&... args)
{
    default_logger()->dump(LogLevel::Info, data, size, fmt::format(fmt, std::forward<Args>(args)...));        
}    

template<typename... Args>
inline void warning_data(void const *data, size_t size, fmt::format_string<Args...> fmt, Args &&... args)
{
    default_logger()->dump(LogLevel::Warning, data, size, fmt::format(fmt, std::forward<Args>(args)...));        
}    

template<typename... Args>
inline void error_data(void const *data, size_t size, fmt::format_string<Args...> fmt, Args &&... args)
{
    default_logger()->dump(LogLevel::Error, data, size, fmt::format(fmt, std::forward<Args>(args)...));        
}

template<typename... Args>
inline void trace_data(std::vector<uint8_t> const &data, fmt::format_string<Args...> fmt, Args &&... args)
{
    default_logger()->dump(LogLevel::Trace, data, fmt::format(fmt, std::forward<Args>(args)...));
}

template<typename... Args>
inline void debug_data(std::vector<uint8_t> const &data, fmt::format_string<Args...> fmt, Args &&... args)
{
    default_logger()->dump(LogLevel::Debug, data, fmt::format(fmt, std::forward<Args>(args)...));        
}

template<typename... Args>
inline void info_data(std::vector<uint8_t> const &data, fmt::format_string<Args...> fmt, Args &&... args)
{
    default_logger()->dump(LogLevel::Info, data, fmt::format(fmt, std::forward<Args>(args)...));        
}    

template<typename... Args>
inline void warning_data(std::vector<uint8_t> const &data, fmt::format_string<Args...> fmt, Args &&... args)
{
    default_logger()->dump(LogLevel::Warning, data, fmt::format(fmt, std::forward<Args>(args)...));        
}    

template<typename... Args>
inline void error_data(std::vector<uint8_t> const &data, fmt::format_string<Args...> fmt, Args &&... args)
{
    default_logger()->dump(LogLevel::Error, data, fmt::format(fmt, std::forward<Args>(args)...));        
}


}

#endif  // __SIMPLE_LOGGER_MAIN_H__
