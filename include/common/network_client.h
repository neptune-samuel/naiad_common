
#ifndef __NOS_NETWORK_CLIENT_H__
#define __NOS_NETWORK_CLIENT_H__

#include <string>
#include <common/sys_time.h>


namespace nos
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
    nos::system::SysTick up_time;
    /// 断开时间
    nos::system::SysTick down_time;

    /// @brief 返回一个简称
    /// @return std::string
    std::string brief()
    {
        return address + ":" + std::to_string(port);
    }
};

} // end network

} // end nos


#endif // __NOS_NETWORK_CLIENT_H__

