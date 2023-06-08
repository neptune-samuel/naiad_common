
/**
 * @file slog_logger.cpp
 * @author Liu Chuansen (samule@neptune-robotics.com)
 * @brief SLOG主函数接口
 * @version 0.1
 * @date 2023-05-31
 * 
 * @copyright Copyright (c) 2023
 * 
 */
#include <memory>
#include <mutex>
#include <map>
#include <iostream>

#include <common/logger.h>

namespace slog 
{


/*
    Logger管理    
*/
static std::shared_ptr<Logger> s_default_logger;
static std::map<std::string, std::shared_ptr<Logger>> s_logger_registry;
static std::mutex s_registry_mutex;


/**
 * @brief 获取默认的Logger
 * 
 * @return Logger& 
 */
static std::shared_ptr<Logger> __default_logger()
{
    // 看看是否有注册有loggers， 有的话，使用第一个
    if (s_default_logger == nullptr)
    {
        for (auto &it : s_logger_registry)
        {
            s_default_logger = it.second;
        }
    }

    if (s_default_logger == nullptr)
    {
        s_default_logger = std::make_shared<Logger>("default");        
    }

    return s_default_logger;
}

std::shared_ptr<Logger> default_logger()
{
    std::lock_guard<std::mutex> lock(s_registry_mutex);  
    return __default_logger();
}

/**
 * @brief 是否存在logger
 * 
 * @param name 
 * @return true 
 */
bool has_logger(std::string const & name)
{
    return (s_logger_registry.find(name) != s_logger_registry.end()); 
}


/**
 * @brief 返回一个logger，有可能为空
 * 
 * @param name 
 * @return std::shared_ptr<Logger> 
 */
std::shared_ptr<Logger> get_logger(std::string const & name)
{
    std::lock_guard<std::mutex> lock(s_registry_mutex);   

    // prints
    // for (auto & it : s_logger_registry)
    // {
    //     std::cout << "+++ " << it.first << std::endl; 
    // }

    auto registry = s_logger_registry.find(name);    
    return (registry == s_logger_registry.end()) ? __default_logger() : registry->second;
}

/**
 * @brief 注册到全局使用
 * 
 * @param logger 
 * @return true 
 * @return false 
 */
bool register_logger(std::shared_ptr<Logger> logger)
{
    std::lock_guard<std::mutex> lock(s_registry_mutex);    

    // 如果有一个名字相同的在了，无法注册
    if (s_logger_registry.find(logger->name()) != s_logger_registry.end())
    {
        std::cout << "logger(" << logger->name() << ") already exist" << std::endl;        
        return false;
    }

    s_logger_registry[logger->name()] = std::move(logger);

    //std::cout << "add, total loggers:" << s_logger_registry.size() << std::endl;

    // prints
    // for (auto & it : s_logger_registry)
    // {
    //     std::cout << "+++ " << it.first << std::endl; 
    // }

    return true;
}

/**
 * @brief 丢弃一个logger
 * 
 * @param name 
 */
void drop_logger(std::string const &name)
{
    std::lock_guard<std::mutex> lock(s_registry_mutex);    

    s_logger_registry.erase(name);

    // std::cout << "drop->" << name << std::endl;
    // for (auto & it : s_logger_registry)
    // {
    //     std::cout << "+++ " << it.first << std::endl; 
    // }

    // 如果默认日志指向它，将它复位
    if (s_default_logger && s_default_logger->name() == name)
    {
        s_default_logger.reset();
    }
}


/**
 * @brief 创建一个Logger
 * 
 * @param name 
 * @param sink 指定一个sink
 * @return std::shared_ptr<Logger> 
 */
std::shared_ptr<Logger> make_logger(std::string const &name, std::shared_ptr<LoggerSink> sink)
{
    auto logger = std::make_shared<Logger>(name, std::move(sink));
    register_logger(logger);         
    return logger;
}

Logger::Logger(std::string const &name, std::shared_ptr<LoggerSink> sink) : name_(name), valid_(false)
{
    if (sink == nullptr)
    {
        // 如果为空，使用一个默认的空的SINK
        sink = std::make_shared<sink::LogNone>();        
    }

    sink_ = std::move(sink);

    // 建立sink
    valid_ = sink_->setup(this->name_);

    if (!valid_)
    {
        std::cout << "setup logger("<< sink_->name() << ") failed" << std::endl;
    }

    //std::cout << "+ create logger:" << name << std::endl;
}


Logger::~Logger()
{    
    //std::cout << "- destruct:" << name() << std::endl;
}



std::string const &Logger::name()
{
    return name_;
}


void Logger::log(LogLevel level, std::string const & msg)
{
    if (valid_)
    {
        sink_->log(level, msg);
    }
}


// 如果长度小于16个，打印在一行
// 否则换行打印, 每隔十六个换行打印
void Logger::dump(LogLevel level, void const *data, size_t size, std::string const &msg)
{
    std::string hex;
    int size_per_line = 16;
    if (size >= size_per_line)
    {
        hex = "\r\n";
    }
    const uint8_t *array = static_cast<const uint8_t *>(data);
    for (size_t i = 0; i < size; ++ i)
    {
        if (!(i % size_per_line))
        {
            if (i > 0)
            {
                hex += "\r\n";
            }

            hex += fmt::format("{:04X}: ", i);
        }

        hex += fmt::format("{:02X} ", array[i]);
    }

    this->log(level, msg + hex);
}

void Logger::dump(LogLevel level, std::vector<uint8_t> const & data, std::string const &msg)
{
    std::string hex;
    int size_per_line = 16;
    if (data.size() >= size_per_line)
    {
        hex = "\r\n";
    }
    for (size_t i = 0; i < data.size(); ++ i)
    {
        if (!(i % size_per_line))
        {
            if (i > 0)
            {
                hex += "\r\n";
            }

            hex += fmt::format("{:04X}: ", i);
        }

        hex += fmt::format("{:02X} ", data[i]);
    }

    this->log(level, msg + hex);
}


} // slog

