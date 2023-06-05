
#ifndef __NAIAD_NETWORK_CLIENT_H__
#define __NAIAD_NETWORK_CLIENT_H__

/**
 * @file network_client.h
 * @author Liu Chuansen (179712066@qq.com)
 * @brief 定义一些通用的网络客户端类型
 * @version 0.1
 * @date 2023-05-21
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#include <string>
#include <common/sys_time.h>


namespace naiad
{

// 放置于网络的命名空间
namespace network
{

struct ClientInfo
{
    /// 客户端地址
    std::string address;
    /// 客户端端口
    int port;
    /// 是否连接
    bool connected;
    /// 连接时间
    naiad::system::SysTick up_time;
    /// 断开时间
    naiad::system::SysTick down_time;
};

/**
 * @brief 一个网络主机
 * 
 */
struct Host
{
    std::string address;
    int port;
};


} // end network

} // end naiad


#endif // __NAIAD_NETWORK_CLIENT_H__

