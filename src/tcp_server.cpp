


/**
 * @file tcp_server.cc
 * @author Liu Chuansen (samule@neptune-robotics.com)
 * @brief 使用libuv实现的TCP服务对象
 * @version 0.1
 * @date 2023-05-15
 * 
 * @copyright Copyright (c) 2023
 * 
 */
#include <string>
#include <thread>

#include <sys/socket.h>

#include <common/logger.h>
#include <common/sys_time.h>
#include <common/tcp_server.h>

namespace nos 
{

namespace network
{

/**
 * @brief TCP 连接对象
 * 
 */
class TcpConnection
{
public:
    /**
     * @brief 创建一个TCP连接
     * 
     * @param server 
     */
    TcpConnection(TcpServer &server) : server_(server), connected_(false), address_(""), port_(0)
    {
        uv_tcp_init(server.get_loop(), &client_);

        /// 设置对象数据？
        uv_handle_set_data((uv_handle_t *)&client_, this);
    }

    /**
     * @brief 析构一个连接
     * 
     */
    ~TcpConnection()
    {
        close();        
    }

    /**
     * @brief 接受一个连接
     * 
     * @param stream 
     * @return true 
     * @return false 
     */
    bool accept(uv_stream_t *stream)
    {
        int ret = uv_accept(stream, (uv_stream_t *)&client_);
        if (!ret)
        {
            connected_ = true;
            up_time_ = nos::system::uptime();
            
            update_address_info();            
            
            ilog("{}: connection({}) accept success", server_.get_name(), brief());
            return true;
        }
        else 
        {
            connected_ = false;
            wlog("{}: connection accept failed, ret={}", server_.get_name(), ret);
            return false;
        }
    }

    /**
     * @brief 关闭连接
     * 
     */
    void close()
    {
        if (connected_)
        {
            uv_close((uv_handle_t *)&client_, nullptr);
            connected_ = false;
        }
    }


    /**
     * @brief 启用读回调
     * 
     * @return true 
     * @return false 
     */
    bool read_start()
    {
        uv_read_start((uv_stream_t*)&client_, [](uv_handle_t *handle, size_t suggested_size, uv_buf_t *buf) {            
            // 申请空间
            buf->base = (char *)malloc(suggested_size);
            buf->len = suggested_size;

        }, [](uv_stream_t* stream, ssize_t nread, const uv_buf_t* buf) {
            
            // 获得连接实例
            auto conn = static_cast<TcpConnection*>(stream->data);
            
            // 调用服务端的数据处理函数
            if (nread > 0)
            {
                conn->server_.on_client_receive(*conn, (unsigned char *)buf->base, nread);
            }
            else if (nread != UV_EOF)
            {
                // client read error ?
                elog("{}: client({}) read failed, ret={}", conn->server_.get_name(), conn->brief(), uv_strerror(nread));
            }

            // 释放空间
            free(buf->base); 
        });

        return true;
    }

    /**
     * @brief 获取连接简称
     * 
     * @return std::string 
     */
    const std::string brief() const
    {
        return address_ + ":" + std::to_string(port_);
    }

    /**
     * @brief 是否连接
     * 
     * @return true 
     * @return false 
     */
    bool is_connected() const
    {
        return connected_;
    }

    // nos::network::ClientInfo client_info()
    // {
    //     nos::network::ClientInfo ci;

    //     dlog("uptime={}", up_time_.to_time_string());
    //     ci.address = address_;
    //     ci.port = port_;
    //     ci.connected = connected_;
    //     ci.up_time = up_time_;
    //     ci.down_time = down_time_;

    //     dlog("ret uptime={}", ci.up_time.to_time_string());

    //     return ci;
    // }

private:
    /// TCP服务端
    TcpServer & server_;
    /// 连接对象
    uv_tcp_t client_;
    /// 连接状态
    bool connected_;
    // 连接地址
    std::string address_;
    // 连接端口
    int port_;

    nos::system::SysTick up_time_;
    nos::system::SysTick down_time_;
    
    /**
     * @brief 更新地址信息
     * 
     */
    void update_address_info()
    {
        struct sockaddr_storage addr;
        int len = sizeof(addr);
        uv_tcp_getpeername(&client_, reinterpret_cast<struct sockaddr*>(&addr), &len);

        char ip[INET6_ADDRSTRLEN];
        if (addr.ss_family == AF_INET) 
        {
            struct sockaddr_in* s = reinterpret_cast<struct sockaddr_in*>(&addr);
            uv_inet_ntop(AF_INET, &s->sin_addr, ip, sizeof(ip));

            address_ = ip;
            port_ = ntohs(s->sin_port);
        } 
        else if (addr.ss_family == AF_INET6) 
        {
            struct sockaddr_in6* s = reinterpret_cast<struct sockaddr_in6*>(&addr);
            uv_inet_ntop(AF_INET6, &s->sin6_addr, ip, sizeof(ip));

            address_ = ip;
            port_ = ntohs(s->sin6_port);
        } 
        else 
        {
            elog("Unknown address family: {}", addr.ss_family);
        } 
    }
};




/**
 * @brief 创建一个TCP服务端
 * 
 * @param name 名称
 * @param address 地址
 * @param port 端口
 * @param max_clients_num  最大连接数，0 不限制 
 */
TcpServer::TcpServer(const std::string &name, 
    const std::string &address, 
    int port, 
    int max_clients_num) : libuv::TcpServer(false), name_(name), address_(address), port_(port)
{
    started_ = false;

    /// 设置对象数据？
    uv_handle_set_data((uv_handle_t*)&server_, this);

    // 设置brief
    brief_ = address_ + ":" + std::to_string(port_);
}


/**
 * @brief 析构TCP服务
 * 
 */
TcpServer::~TcpServer()
{    
    // 先关闭所有的连接
    stop();
}

/**
 * @brief 获取TCP服务的名称
 * 
 * @return const std::string& 
 */
const std::string & TcpServer::get_name()
{
    return name_;
}


/**
 * @brief 获取一个简称
 * 
 * @return const std::string& 
 */
const std::string & TcpServer::get_brief()
{
    return brief_;
}



bool TcpServer::start()
{
    if (started_)
    {
        wlog("start {} failed, it seems already started", name_);
        return false;
    }

    int ret = bind(address_, port_);
    if (ret != 0)
    {
        elog("{}: bind to {} failed: {}", name_, brief_, uv_strerror(ret));
        return false;
    }

    int backlog = 128;

    ret = uv_listen((uv_stream_t *)&server_, backlog, [](uv_stream_t *server, int status){
            auto self = static_cast<TcpServer*>(server->data);
            self->on_new_connection(status);            
        });

    if (ret != 0)
    {
        elog("{}: listen failed: {}", name_, uv_strerror(ret));
        return false;
    }

    ilog("{}: listen on {} success", name_, brief_);

    // 创建一个线程，运行uv loop
    thread_ = std::thread(&TcpServer::loop_thread, this);

    return true;
}


/**
 * @brief 停止TCP服务
 * 
 */
void TcpServer::stop()
{
    if (started_)
    {
        // TODO: close all connections
        close_all_connections();

        // 停止线程
        uv_stop(get_loop());
        // wait for thread done
        thread_.join();

        started_ = false;
    }
}


/**
 * @brief LOOP 线程
 * 
 */
void TcpServer::loop_thread()
{
    started_ = true;
    ilog("{}: loop thread started", name_);
    // 启动UV LOOP
    uv_run(get_loop(), UV_RUN_DEFAULT);    

    wlog("{}: loop thread exited", name_);    

    started_ = false;
}

/**
 * @brief 清空所有连接
 * 
 */
void TcpServer::close_all_connections()
{
    /// 删除所有连接
    if (connections_.size() > 0)
    {
        decltype(connections_)().swap(connections_);
    }    
}

/**
 * @brief 处理新连接
 * 
 * @param status 
 */
void TcpServer::on_new_connection(int status)
{
    if (status < 0)
    {
        elog("{}: connection callback error: {}", name_, uv_strerror(status));
        return ;
    }

    auto conn = std::make_unique<TcpConnection>(*this);

    if (conn->accept((uv_stream_t *)&server_))
    {
        // 启动读操作
        conn->read_start();
        // 添加到连接列表
        connections_.emplace_back(std::move(conn));
    }
}

/**
 * @brief 当从客户端接收到数据时
 * 
 * @param conn 
 * @param buf 
 * @param size 
 */
void TcpServer::on_client_receive(TcpConnection &conn, unsigned char *buf, ssize_t size)
{
    dlog("{}: read {} bytes from client({})", name_, size, conn.brief());
}


} // network

} // nos

