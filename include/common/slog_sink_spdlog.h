

#ifndef __SLOG_SINK_SPDLOG_H__
#define __SLOG_SINK_SPDLOG_H__

/**
 * @file slog_sink_spdlog.h
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

#include "slog_logger.h"

// 声明一下
namespace spdlog
{
    class logger;

    namespace level {
        enum level_enum : int;
    }
}

namespace slog 
{

namespace sink 
{


class SpdlogToConsole: public slog::LoggerSink
{
public:
    explicit SpdlogToConsole(slog::LogLevel level) : level_(level), logger_(nullptr) { }

    bool setup(std::string const & name);
    void log(slog::LogLevel level, std::string const &msg);
    void set_level(slog::LogLevel level);

    const char * name() { return "SpdlogToConsole"; }
private:
    slog::LogLevel level_;
    std::shared_ptr<spdlog::logger> logger_;
};



class SpdlogToFile: public slog::LoggerSink
{
public:
    SpdlogToFile(slog::LogLevel level, 
        std::string const &log_file, 
        size_t file_size = 1024*1024, 
        size_t file_num = 4) :level_(level), file_path_(log_file),
        file_size_(file_size), file_num_(file_num) { }

    bool setup(std::string const & name);
    void log(slog::LogLevel level, std::string const &msg);
    void set_level(slog::LogLevel level);

    const char *name() { return "SpdlogToFile"; }    
private:
    slog::LogLevel level_;
    std::string file_path_;
    size_t file_size_;
    size_t file_num_;
    std::shared_ptr<spdlog::logger> logger_;
};


}

}

#endif  // __SLOG_SINK_SPDLOG_H__

