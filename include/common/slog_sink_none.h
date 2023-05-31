

#ifndef __SLOG_SINK_NONE_H__
#define __SLOG_SINK_NONE_H__

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

#include "slog_logger.h"

namespace slog 
{

namespace sink 
{

class LogNone: public slog::LoggerSink
{
public:
    LogNone() { }

    bool setup([[maybe_unused]]std::string const & name) { return true;  }
    void log([[maybe_unused]]slog::LogLevel level, [[maybe_unused]]std::string const &msg) { }
    void set_level([[maybe_unused]]LogLevel level) { }
    const char* name() { return "LogNone"; }
};

}

}

#endif  // __SLOG_SINK_NONE_H__

