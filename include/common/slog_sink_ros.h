


#ifndef __SLOG_SINK_RCLCPP_H__
#define __SLOG_SINK_RCLCPP_H__

/**
 * @file slog_sink_rclcpp.h
 * @author Liu Chuansen (samule@neptune-robotics.com)
 * @brief 这是一个使用ROS的结点实现的SINK
 * @version 0.1
 * @date 2023-05-31
 * 
 * @copyright Copyright (c) 2023
 * 
 */
#include <memory>
#include <string>

#include <rclcpp/rclcpp.hpp>

#include "slog_logger.h"


namespace slog 
{

namespace sink 
{


class RosLogger: public slog::LoggerSink
{
public:
    RosLogger(std::string const &logger_name) : logger_(rclcpp::get_logger(logger_name)) { }

    bool setup([[maybe_unused]]std::string const & name) 
    { 
        return true;  
    }

    void log([[maybe_unused]]slog::LogLevel level, [[maybe_unused]]std::string const &msg) 
    { 
        switch(level)
        {
            case slog::LogLevel::Trace:
            RCLCPP_DEBUG(logger_, msg);
            break;
            case slog::LogLevel::Debug:
            RCLCPP_DEBUG(logger_, msg);
            break;
            case slog::LogLevel::Info:
            RCLCPP_INFO(logger_, msg);
            break;
            case slog::LogLevel::Warning:
            RCLCPP_WARN(logger_, msg);
            break;
            case slog::LogLevel::Error:
            RCLCPP_ERROR(logger_, msg);
            break;

            default:
            break;
        }        
    }


    void set_level([[maybe_unused]]slog::LogLevel level) { }

    const char* name() { return "RosLogger"; }
private:
     rclcpp::Logger logger_;
};


}

}

#endif  // __SLOG_SINK_RCLCPP_H__


