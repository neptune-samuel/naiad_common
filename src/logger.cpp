

#include <memory>
#include <mutex>
#include <map>
#include <iostream>

#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/sinks/rotating_file_sink.h>
#include <spdlog/fmt/bin_to_hex.h>

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
        s_default_logger = std::make_shared<Logger>("default", LogLevel::Info);        
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
    for (auto & it : s_logger_registry)
    {
        std::cout << "+++ " << it.first << std::endl; 
    }

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
 * @param level 
 * @param log_file 
 * @param file_size 
 * @param file_num 
 * @return std::shared_ptr<Logger> 
 */
std::shared_ptr<Logger> make_logger(std::string const &name, LogLevel level, std::string const &log_file, int file_size, int file_num)
{
    auto logger = std::make_shared<Logger>(name, level, log_file, file_size, file_num);
    // 
    //register_logger(std::move(logger));         
    register_logger(logger);         

    return logger;
}

/**
 * @brief 创建一个Logger
 * 
 * @param name 
 * @param level 
 * @return std::shared_ptr<Logger> 
 */
std::shared_ptr<Logger> make_logger(std::string const &name, LogLevel level)
{
    return make_logger(name, level, "", 0, 0);
}



Logger::Logger(std::string const &name, LogLevel level, std::string const &log_file, int file_size, int file_num)
{
    std::vector<spdlog::sink_ptr> sinks;

    auto console = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
    console->set_pattern("%Y-%m-%d %H:%M:%S.%e %^[%L] (%n) %v%$");
    sinks.push_back(console);

    if (log_file.size())
    {
        auto file = std::make_shared<spdlog::sinks::rotating_file_sink_mt>(log_file, file_size, file_num);
        file->set_pattern("%Y-%m-%d %H:%M:%S.%e %^[%L] %v%$");
        sinks.push_back(file);
    }

    logger_ = std::make_shared<spdlog::logger>(name, begin(sinks), end(sinks));
    
    logger_->set_level(to_spdlog_level(level));

    // 注册到全局数据中
    //spdlog::register_logger(logger_);  
    //spdlog::set_default_logger(logger);

    //std::cout << "+ create logger:" << name << std::endl;
    
    //add_logger(std::shared_ptr<Logger>(this));      
}


Logger::~Logger()
{
    //spdlog::drop(logger_->name());

    //std::cout << "- destruct:" << name() << std::endl;

    //drop_logger(logger_->name());
}



std::string const &Logger::name()
{
    return logger_->name();
}

// void Logger::set_default()
// {
//     //spdlog::set_default_logger(logger_);
//     std::lock_guard<std::mutex> lock(s_default_logger_mutex);
    
//     // 设置默认Logger
//     s_default_logger = std::make_shared<Logger>(*this);
// }

void Logger::log(LogLevel level, std::string const & msg)
{
    logger_->log(to_spdlog_level(level), msg);
}


void Logger::dump(LogLevel level, void const *data, int size, std::string const &msg)
{
    const char *fmt = (size <= 16) ? "{:Xn}" : "{:X}";
    std::string hex = fmt::format(fmt, spdlog::to_hex((const unsigned char *)data, (const unsigned char *)data + size, 16));
    
    logger_->log(to_spdlog_level(level), msg + hex);
}


spdlog::level::level_enum Logger::to_spdlog_level(LogLevel level)
{
    return static_cast<spdlog::level::level_enum>(level);
}



}

