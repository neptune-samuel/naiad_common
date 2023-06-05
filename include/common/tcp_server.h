
#ifndef __NAIAD_TCPSERVER_H__
#define __NAIAD_TCPSERVER_H__

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
#include <queue>
#include <iostream>
#include <thread>
#include <optional>
#include <mutex>

#include <common/uv_helper.h>
#include <common/network_client.h>
#include <common/network_frame.h>

namespace naiad
{

namespace network
{

/// 声明一个TCP连接内部类型
class TcpConnection;



/**
 * @brief 一个TCP服务端的封装
 * 
 */
class TcpServer : public uv::TcpServer
{
public:

    /// 创建一个常量，表示所有主机，发送时使用
    static const Host AllClients;

    /// TCP信号
    /// 后续完善，增加更多的信号，如断连？
    static const uv::AsyncSignal::SignalId SignalReceiveFrame;
    static const uv::AsyncSignal::SignalId SignalConnectionLost;

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


    // 禁止复制构造
    TcpServer(const TcpServer &) = delete;
    TcpServer & operator=(const TcpServer &) = delete;


    /**
     * @brief 获取TCP服务的名称
     * 
     * @return const std::string& 
     */
    std::string const & name();

    /**
     * @brief 获取一个简称
     * 
     * @return const std::string& 
     */
    std::string const & brief();

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
     * @brief TCP服务是否正在运行中
     * 
     * @return true 
     * @return false 
     */
    bool is_running();

    /**
     * @brief 返回当前接收的帧数量
     * 
     * @return int 
     */
    int received_frames_num();

    /**
     * @brief 对队列中接收一帧数据
     * 
     * @return DataFrame 如果DataFrame.is_empty() 为真，表示未接收到数据
     * @note 不使用C++17的特性
     */
    DataFrame receive();

    /**
     * @brief 接收通知绑定到外部Loop
     * 
     * @param uv_loop 
     */
    void signal_bind(int signal, uv_loop_t *uv_loop, uv::AsyncSignal::Function signal_handle);

    /**
     * @brief 绑定一个信号
     * 
     * @param loop 
     * @param signal_handle 
     */
    void signal_bind(int signal, uv::Loop &loop, uv::AsyncSignal::Function signal_handle)
    {
        signal_bind(signal, loop.get(), signal_handle);
    }


    /**
     * @brief 发送数据到指定客户端
     * 
     * @param host 指定主机，如果host = AllClients 表示发给所有客户端
     * @param data 需要发送的数据
     * @param size 
     * @return true 发送成功
     * @return false 发送失败
     */
    bool send(Host const & host, uint8_t const * const data, int size);


    /**
     * @brief 发送一个准备好帧
     * 
     * @param frame 
     * @return true 
     * @return false 
     */
    bool send(DataFrame const &frame);

    /// 返回连接数量 
    int connections_num();

    /**
     * @brief 清空所有连接
     * 
     */
    void close_all_connections();

    /**
     * @brief 以info级别显示客户端状态信息
     * 
     */
    void dump_clients();
    

private:
    /// 地址
    std::string address_;
    /// 端口
    int port_ = 0;
    
    /// 名称
    std::string name_;
    /// 最大连接数
    int max_clients_num_;
    /// 是否已启动
    bool started_ = false;
    /// 简要信息
    std::string brief_;

    /// TCP线程
    std::thread thread_;
    bool thread_exit_ = false;

    /// TCP连接队列
    std::vector<std::unique_ptr<TcpConnection>> connections_;
    /// 客户端信息
    std::vector<std::unique_ptr<ClientInfo>> clients_;
    /// 接收帧FIFO
    std::queue<DataFrame> rx_frames_;
    /// 发送帧FIFO
    std::queue<DataFrame> tx_frames_;

    std::mutex rx_mutex_;
    std::mutex tx_mutex_;

    uv::AsyncSignal tx_notify_;

    // 给外部线程使用，通知外部线程数据准备好
    uv::AsyncSignal rx_notify_;

    /**
     * @brief 处理新连接
     */
    void setup_connection();

    /**
     * @brief 获取一个客户端信息对象
     * 
     * @param address 
     * @param port 
     * @return ClientInfo& 
     */
    ClientInfo & get_client_info(std::string const & address, int port);

    /**
     * @brief LOOP执行线程
     */
    void loop_thread();
};



} // end network

} // end naiad


#endif // __NAIAD_TCPSERVER_H__
