
#ifndef __NOS_UPTIME_H__
#define __NOS_UPTIME_H__

/**
 * @file uptime.h
 * @author Liu Chuansen (samule@neptune-robotics.com)
 * @brief 提供一个uptime的定义
 * @version 0.1
 * @date 2023-05-15
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#include <chrono>

namespace nos
{

namespace system 
{

/// 定义一个新类型 时间戳
using SysTick = uint64_t;

using SysTime = uint64_t;


/**
 * @brief 获取当前时间戳
 * 
 * @return sysTick_t 
 */
static inline SysTick uptime(void)
{
    auto now = std::chrono::steady_clock::now();
    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch());
    return ms.count();
}

/**
 * @brief 返回当前时间
 * 
 * @return SysTime 
 */
static inline SysTime now(void)
{
    auto now = std::chrono::system_clock::now();
    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch());
    return ms.count();
}

/**
 * @brief 将SysTick转换为系统时间
 * 
 * @param tick 
 * @return int64_t 
 */
static inline SysTime to_sys_time(SysTick tick)
{
    SysTick now_tick = uptime();
    SysTime now_time = now();

    return now_time - (now_tick - tick);
}


static inline 



} // system

} // nos

#endif // __NOS_UPTIME_H__

