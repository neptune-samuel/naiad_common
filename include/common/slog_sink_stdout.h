
#ifndef __SLOG_SINK_STDOUT_H__
#define __SLOG_SINK_STDOUT_H__

/**
 * @file logger_sink_none.h
 * @author Liu Chuansen (samule@neptune-robotics.com)
 * @brief 这是一个空SINK，不做任何事情 
 * @version 0.1
 * @date 2023-05-31
 * 
 * @copyright Copyright (c) 2023
 * 
 */
#include <memory>
#include <string>
#include <mutex>

#include "slog_logger.h"

namespace slog 
{

namespace sink 
{

class Stdout: public slog::LoggerSink
{
public:
    Stdout(LogLevel level) : level_(level) { }

    bool setup(std::string const & name);
    void log(slog::LogLevel level, std::string const &msg);
    void set_level(LogLevel level);
    const char* name() { return "Stdout"; }

private:
    std::string name_;
    LogLevel level_;
    // 线程安全需使用全局的锁
    //std::mutex mutex_;
};

}

}



#endif  // __SLOG_SINK_STDOUT_H__
