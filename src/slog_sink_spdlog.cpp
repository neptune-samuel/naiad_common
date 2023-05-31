
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

#include <sys/stat.h>


#include <spdlog/logger.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/sinks/rotating_file_sink.h>

#include <common/slog_sink_spdlog.h>

namespace slog 
{

namespace sink 
{

/**
 * @brief 级别转换
 * 
 * @param level 
 * @return spdlog::level::level_enum 
 */
static spdlog::level::level_enum to_spdlog_level(LogLevel level)
{
    switch (level) 
    {
        case LogLevel::Trace:
            return spdlog::level::trace;
        case LogLevel::Debug:
            return spdlog::level::debug;
        case LogLevel::Info:
            return spdlog::level::info;
        case LogLevel::Warning:
            return spdlog::level::warn;
        case LogLevel::Error:
            return spdlog::level::err;
        default:
            return spdlog::level::info;  // fallback to default level
    }
}

/**
 * @brief 配置一个终端日志SINK
 * 
 * @param name 
 * @return true 
 * @return false 
 */
bool SpdlogToConsole::setup(std::string const & name) 
{
    std::vector<spdlog::sink_ptr> sinks;

    auto console = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
    console->set_pattern("%Y-%m-%d %H:%M:%S.%e %^[%L] (%n) %v%$");
    sinks.push_back(console);

    logger_ = std::make_shared<spdlog::logger>(name, begin(sinks), end(sinks));

    logger_->set_level(to_spdlog_level(level_));

    // 注册到全局数据中
    //spdlog::register_logger(logger_);  
    //spdlog::set_default_logger(logger);

    return true;
}

void SpdlogToConsole::log(slog::LogLevel level, std::string const &msg) 
{
    logger_->log(to_spdlog_level(level), msg);
}

void SpdlogToConsole::set_level(LogLevel level) 
{ 
    logger_->set_level(to_spdlog_level(level));
}


/**
 * @brief 配置一个文件日志SINK
 * 
 * @param name 
 * @return true 
 * @return false 
 */
bool SpdlogToFile::setup(std::string const & name) 
{
    std::vector<spdlog::sink_ptr> sinks;

    auto console = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
    console->set_pattern("%Y-%m-%d %H:%M:%S.%e %^[%L] (%n) %v%$");
    sinks.push_back(console);

    if (file_path_.size())
    {
        auto file = std::make_shared<spdlog::sinks::rotating_file_sink_mt>(file_path_, file_size_, file_num_);
        file->set_pattern("%Y-%m-%d %H:%M:%S.%e %^[%L] %v%$");
        sinks.push_back(file);
    }

    logger_ = std::make_shared<spdlog::logger>(name, begin(sinks), end(sinks));
    logger_->set_level(to_spdlog_level(level_));

    // 注册到全局数据中
    //spdlog::register_logger(logger_);  
    //spdlog::set_default_logger(logger);

    return true;
}

void SpdlogToFile::log(slog::LogLevel level, std::string const &msg) 
{
    logger_->log(to_spdlog_level(level), msg);
}

void SpdlogToFile::set_level(LogLevel level) 
{ 
    logger_->set_level(to_spdlog_level(level));
}



}

}



