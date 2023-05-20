
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
 * @note 
 *   增加一个客户端ClientInfo信息容器，
 * 
 *   TCP服务需要以下功能
 *   - 获取当前的连接数量
 *           get_clients() //返回所有连接数据 
 *           get_clients() // 返回当前的连接 
 *   - 往一个或多个客户端写数据               
 *             send(Client, data, size)
 *   - 设定回调函数，当从某客户端接收到数据时
 * 
 */

#include <string>
#include <vector>
#include <iostream>
#include <thread>

#include <common/libuv_utils.h>
#include <common/network_client.h>

namespace nos
{

namespace network
{

/// 声明一个TCP连接内部类型
class TcpConnection;

/**
 * @brief 一个TCP服务端的封装
 * 
 */
class TcpServer : public libuv::TcpServer
{
public:

    /**
     * @brief 创建一个TCP服务端
     * 
     * @param name 名称
     * @param address 地址
     * @param port 端口
     * @param max_clients_num  最大连接数，0 不限制 
     */
    TcpServer(std::string const &name, std::string const &address, int port, int max_clients_num = 10);

    /**
     * @brief 创建一个默认参数的TCP服务
     * 
     * @param name 
     */
    TcpServer(std::string const &name): TcpServer(name, "0.0.0.0", 9600) { }
    /**
     * @brief 析构TCP服务
     * 
     */
    ~TcpServer();

    /**
     * @brief 获取TCP服务的名称
     * 
     * @return const std::string& 
     */
    std::string const & get_name();

    /**
     * @brief 获取一个简称
     * 
     * @return const std::string& 
     */
    std::string const & get_brief();

    /**
     * @brief 启动TCP服务，开始监听指定端口
     * 
     * @return true 
     * @return false 
     */
    bool start();

    /**
     * @brief 停止TCP服务，它将关闭所有连接
     * 
     */
    void stop();

    /**
     * @brief 清空所有连接
     * 
     */
    void close_all_connections();

    /**
     * @brief 返回所有客户端
     * 
     * @return const std::vector<ClientInfo>& 
     */
    //const std::vector<ClientInfo> &get_clients();

    /// 将连接声明为友元类
    friend class TcpConnection;

private:
    /// 地址
    std::string address_;
    /// 端口
    int port_;
    
    /// 名称
    std::string name_;
    /// 最大连接数
    int max_clients_num_;
    /// 是否已启动
    bool started_;
    /// 简要信息
    std::string brief_;

    /// TCP线程
    std::thread thread_;
    bool thread_exit_;

    /// TCP连接队列
    std::vector<std::unique_ptr<TcpConnection>> connections_;
    /// 客户端信息
    std::vector<ClientInfo> clients_;


    // /**
    //  * @brief 连接事件处理
    //  * 
    //  * @param connection 连接对象
    //  * @param event 事件
    //  * @param data 数据
    //  * @param size 数据大小
    //  */
    // void connection_event_handle(TcpConnection &connection, TcpConnection::Event event, unsigned char *data, int size);

    /**
     * @brief 处理新连接
     */
    void setup_connection();

    /**
     * @brief LOOP执行线程
     */
    void loop_thread();
};



} // end network

} // end nos


#endif // __NOS_TCPSERVER_H__
