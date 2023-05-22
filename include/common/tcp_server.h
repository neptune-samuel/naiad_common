
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
#include <queue>
#include <iostream>
#include <thread>

#include <common/libuv_utils.h>
#include <common/network_client.h>
#include <common/network_frame.h>

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
     * @brief 对队列中接收一个帧数据
     * 
     * @return DataFrame 
     */
    std::optional<DataFrame> receive();

/*
确实，如果队列 `datas` 中没有任何数据，调用 `receive` 函数将会导致程序出现未定义行为。为了避免这种情况，我们可以在 `receive` 函数中添加一些错误处理代码，以确保程序的健壮性。以下是一个修改后的 `receive` 函数：

```cpp
std::optional<MyClass> receive(std::queue<MyClass>& datas) {
    if (datas.empty()) { // 如果队列为空，返回空值
        return std::nullopt;
    }
    MyClass data = std::move(datas.front()); // 使用移动语义获取队首元素
    datas.pop(); // 删除队首元素
    return data; // 返回队首元素
}
```

在这个函数中，我们首先使用 `empty()` 函数判断队列是否为空。如果队列为空，我们返回一个空值 `std::nullopt`。否则，我们使用移动语义获取队首元素，并将其移动到一个 `MyClass` 类型的变量 `data` 中。然后，我们使用 `pop()` 函数将队首元素从队列中删除。最后，我们返回 `data` 变量，即返回队首元素的移动构造函数所创建的对象。

需要注意的是，由于我们使用了 `std::optional` 类型来表示可能为空的返回值，因此在调用 `receive` 函数时，需要使用类似于下面这样的代码来处理返回值：

```cpp
std::optional<MyClass> data = receive(datas);
if (data.has_value()) {
    // 处理数据
    MyClass myData = data.value(); // 获取实际的数据
} else {
    // 处理队列为空的情况
}
```

在这个代码中，我们首先使用 `has_value()` 函数判断返回值是否为空。如果返回值不为空，我们可以使用 `value()` 函数获取实际的数据。否则，我们可以处理队列为空的情况。
*/
    /**
     * @brief 发送数据到指定客户端
     * 
     * @param host 指定主机，如果为std::nullopt， 表示发给所有主机
     * @param data 需要发送的数据
     * @param size 
     * @return true 发送成功
     * @return false 发送失败
     */
    bool send(std::optional<Host> host, uint8_t const *data, int size);


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
    std::vector<std::unique_ptr<ClientInfo>> clients_;
    /// 接收帧FIFO
    std::queue<DataFrame> rx_frames_;
    /// 发送帧FIFO
    std::queue<DataFrame> tx_frames_;

    std::mutex rx_mutex_;
    std::mutex tx_mutex_;

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

} // end nos


#endif // __NOS_TCPSERVER_H__
