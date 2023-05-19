
#ifndef __NOS_SYS_TIME_H__
#define __NOS_SYS_TIME_H__

/**
 * @file sys_time.h
 * @author Liu Chuansen (samule@neptune-robotics.com)
 * @brief 提供一个时间相关的定义
 * @version 0.1
 * @date 2023-05-15
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#include <chrono>
#include <string>
#include <thread>

namespace nos
{
namespace system 
{

/**
 * @brief 返回一个时间戳
 * 
 * @return int64_t 
 */
static inline int64_t uptime(void)
{
    auto now = std::chrono::steady_clock::now();
    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch());
    return ms.count();
}

/**
 * @brief 返回当前系统时间
 * 
 * @return int64_t 
 */
static inline int64_t now(void)
{
    auto now = std::chrono::system_clock::now();
    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch());
    return ms.count();
}


/**
 * @brief 延时微秒
 * 
 * @param us 
 */
static inline void udelay(int us)
{
    std::this_thread::sleep_for(std::chrono::microseconds(us));
}

/**
 * @brief 延时指毫秒
 * 
 * @param ms 
 */
static inline void mdelay(int ms)
{
    std::this_thread::sleep_for(std::chrono::milliseconds(ms));
}

/**
 * @brief 一个系统时钟
 * 
 */
class SysTime
{

public:
    SysTime(int64_t count) : count_(count) { }
    SysTime() : count_(now()) { }

    ~SysTime() = default;

    SysTime(const SysTime &) = default;
    SysTime & operator=(const SysTime &) = default;

    SysTime & operator=(int64_t t)
    {
        count_ = t;
        return *this;
    }

    std::string to_string()
    {
        char buf[100] = { 0 };
        std::time_t t = count_ / 1000;
        std::tm tm = *std::localtime(&t);

        std::sprintf(buf, "%04d-%02d-%02d %02d:%02d:%02d.%03d", tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday,
            tm.tm_hour, tm.tm_min, tm.tm_sec, (int)(count_ % 1000));

        return std::string(buf);
    }

    int64_t get_millisecond()
    {
        return count_;
    }

    int64_t get_second()
    {
        return (count_ / 1000);
    }

private:
    int64_t count_;  
};


/**
 * @brief 系统TICK
 * 
 */
class SysTick
{
public:
    SysTick(int64_t count) : count_(count) { }

    SysTick() : count_(uptime()) { }

    ~SysTick() = default;

    SysTick(const SysTick &) = default;
    SysTick & operator=(const SysTick &) = default;

    SysTick & operator=(int64_t t)
    {
        count_ = t;
        return *this;
    }

    /**
     * @brief 转换成系统时间
     * 
     * @return SysTime 
     */
    SysTime to_time()
    {
        int64_t diff = uptime() - count_;
        return SysTime(now() - diff);
    }

    /**
     * @brief 返回系统时间格式
     * 
     * @return std::string 
     */
    std::string to_time_string()
    {
        SysTime t = to_time();
        return t.to_string();
    }

    int64_t get_millisecond()
    {
        return count_;
    }

    int64_t get_second()
    {
        return (count_ / 1000);
    }

private:
    int64_t count_;
};


} // end system

} // end nos

#endif // __NOS_SYS_TIME_H__
