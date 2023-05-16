

/**
 * @file logger.cc
 * @author Liu Chuansen (samule@neptune-robotics.com)
 * @brief 使用spdlog封装日志函数
 * @version 0.1
 * @date 2023-05-15
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/sinks/rotating_file_sink.h>


namespace nos 
{

/**
 * @brief 初始化logger
 * 
 * @param logger_name 日志名称
 * @param log_level
 * @param file_path 日志路径
 * @param file_size 日志文件最大大小, 默认为1MB
 * @param file_num  日志文件回滚数量 默认为2个
 */
void logger_init(const std::string &logger_name, spdlog::level::level_enum log_level, const std::string &file_path, int file_size = 1024 * 1024, int file_num = 2)
{
    std::vector<spdlog::sink_ptr> sinks;

    auto console = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
    console->set_pattern("%Y-%m-%d %H:%M:%S.%e %^[%L] (%n) %v%$");
    sinks.push_back(console);

    if (file_path.size())
    {
        auto file = std::make_shared<spdlog::sinks::rotating_file_sink_mt>(file_path, file_size, file_num);
        file->set_pattern("%Y-%m-%d %H:%M:%S.%e %^[%L] %v%$");
        sinks.push_back(file);
    }

    auto logger = std::make_shared<spdlog::logger>(logger_name, begin(sinks), end(sinks));
    
    logger->set_level(log_level);

    spdlog::register_logger(logger);
    spdlog::set_default_logger(logger);
}


/**
 * @brief 初始化日志，使用默认文件名，默认关闭文件日志
 * 
 * @param logger_name 
 * @param log_level 
 * @param log_to_file 
 */
void logger_init(const std::string &logger_name, spdlog::level::level_enum log_level, bool log_to_file = false)
{
    std::string log_path = log_to_file ? ("logs" + logger_name + ".log") : "";

    logger_init(logger_name, log_level, log_path);
}


} // end nos
