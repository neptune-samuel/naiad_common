
#ifndef __NOS_TCPSERVER_H__
#define __NOS_TCPSERVER_H__

/**
 * @file tcp_server.h
 * @author Liu Chuansen (samule@neptune-robotics.com)
 * @brief 使用libuv封装一个TCP服务
 * @version 0.1
 * @date 2023-05-15
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#include <string>
#include <vector>

#include <common/network_client.h>

#include <uv.h>

namespace nos
{

// 放置于网络的命名空间
namespace network
{

/// 声明一个TCP连接内部类型
class TcpConnection;


/**
 * @brief 一个TCP服务端的封装
 * 
 */
class TcpServer
{

public:
    TcpServer(uv_loop_t *loop);
    TcpServer(uv_loop_t *loop, std::string address, int port);    
    ~TcpServer();

    // 禁止复制构造
    TcpServer(const TcpServer &) = delete;
    TcpServer & operator=(const TcpServer &) = delete;

    /**
     * @brief 返回一个简称
     * 
     * @return std::string 
     */
    std::string brief();

    /**
     * @brief 返回loop对象
     * 
     * @return uv_loop_t* 
     */
    uv_loop_t *get_loop()
    {
        return loop_;
    }

    /**
     * @brief 启动服务
     * 
     * @return true 
     * @return false 
     */
    bool start();

    /**
     * @brief 执行
     * 
     * @return true 
     * @return false 
     */
    bool run();

    /**
     * @brief 清理连接
     * 
     */
    void clean_connections();

    /**
     * @brief 返回客户端数量 
     * 
     * @return int 
     */
    int get_clients_num();

    /**
     * @brief 关闭所有的连接
     * 
     */
    void close_all_connections();

    /**
     * @brief 获取一个客户端列表
     * 
     * @return std::vector<nos::network::ClientInfo> 
     */
    std::vector<nos::network::ClientInfo> get_clients();

private:
    std::string address_;
    int port_;
    int backlog_;
    bool started_;

    uv_loop_t * loop_;
    uv_tcp_t server_;
    uv_stream_t *stream_;
    uv_handle_t *handle_;

    /// TCP连接队列
    std::vector<std::unique_ptr<TcpConnection>> connections_;

    /**
     * @brief 绑定到指定接口
     * 
     * @param ip 
     * @param port 
     * @return true 
     * @return false 
     */
    bool bind(const std::string &ip, int port);

    /**
     * @brief 启用监听
     * 
     * @param backlog 
     * @return true 
     * @return false 
     */
    bool listen(int backlog);

    /**
     * @brief 处理新连接
     * 
     * @param status 
     */
    void on_new_connection(int status);

};



} // end network

} // end nos


#endif // __NOS_TCPSERVER_H__
