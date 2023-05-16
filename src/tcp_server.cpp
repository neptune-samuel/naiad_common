

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
#include <uv.h>
#include <string>

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
     * @param parent 
     */
    TcpConnection(TcpServer &parent) : parent_(parent), connected_(false), address_(""), port_(0)
    {
        uv_tcp_init(parent.get_loop(), &client_);

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
     * @brief 获取连接简称
     * 
     * @return std::string 
     */
    std::string brief() const
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

    nos::network::ClientInfo client_info()
    {
        nos::network::ClientInfo ci;

        dlog("uptime={}", up_time_.to_time_string());
        ci.address = address_;
        ci.port = port_;
        ci.connected = connected_;
        ci.up_time = up_time_;
        ci.down_time = down_time_;

        dlog("ret uptime={}", ci.up_time.to_time_string());

        return ci;
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
            
            ilog("connection({}) setup success", brief());
            return true;
        }
        else 
        {
            connected_ = false;
            wlog("TCP connection accept failed, ret={}", ret);
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
     * @brief 返回父对象，在回调函数中使用
     * 
     * @return TcpServer* 
     */
    TcpServer &get_parent()
    {
        return parent_;
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
            auto conn = static_cast<TcpConnection*>(stream->data);
            
            if (nread < 0)
            {
                if (nread != UV_EOF)
                {
                    elog("connection({}) read error: {}", conn->brief(), uv_err_name(nread));    
                }
                else 
                {
                    wlog("connection({}) reset by remote", conn->brief());
                }
                
                // 关闭客户端
                free(buf->base);

                conn->close();
                conn->get_parent().clean_connections();
            }
            else 
            {
                free(buf->base);

                ilog("connection({}) read {} bytes", conn->brief(), nread);
            }
        });

        return true;
    }

 



private:
    /// TCP服务端
    TcpServer & parent_;
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
 * @brief 构造函数 
 * 
 * @param loop 
 */
TcpServer::TcpServer(uv_loop_t *loop, std::string address, int port) : loop_(loop), address_(address), port_(port)
{
    if (loop == nullptr)
    {
        loop_ = uv_default_loop();
    }

    uv_tcp_init(loop_, &server_);

    stream_ = (uv_stream_t *)&server_;
    handle_ = (uv_handle_t *)&server_;

    started_ = false;
    backlog_ = 128;

    /// 设置对象数据？
    uv_handle_set_data(handle_, this);
}

/**
 * @brief 代理构造函数
 * 
 * @param loop 
 */
TcpServer::TcpServer(uv_loop_t *loop) : TcpServer(loop, "0.0.0.0", 9999) { }


/**
 * @brief 析构TCP服务
 * 
 */
TcpServer::~TcpServer()
{    
    // 先关闭所有的连接
    close_all_connections();

    // 关闭自己的
    uv_close(handle_, nullptr);   
}

/**
 * @brief 返回一个简称
 * 
 * @return std::string 
 */
std::string TcpServer::brief()
{
    return address_ + ":" + std::to_string(port_);
}

/**
 * @brief 绑定到指定接口
 * 
 * @param ip 
 * @param port 
 * @return true 
 * @return false 
 */
bool TcpServer::bind(const std::string &ip, int port)
{
    struct sockaddr_in addr;
    
    uv_ip4_addr(ip.c_str(), port, &addr);
    
    int ret = uv_tcp_bind(&server_, (const struct sockaddr *)&addr, 0);
    dlog("call uv_tcp_bind() return {}", ret);

    return (ret == 0);
}

/**
 * @brief 开启监听
 * 
 * @param backlog 
 * @return true 
 * @return false 
 */
bool TcpServer::listen(int backlog)
{
    int ret = uv_listen(stream_, backlog, [](uv_stream_t *server, int status){
        auto self = static_cast<TcpServer*>(server->data);
        self->on_new_connection(status);            
    });

    dlog("call uv_listen() return {}", ret);

    if (!ret)
    {
        ilog("TCP server listened on {}:{}", address_, port_);
    }

    return (ret == 0);
}


bool TcpServer::start()
{
    if (!bind(address_, port_))
    {
        return false;
    }

    if (!listen(backlog_))
    {
        return false;
    }

    started_ = true;

    return true;
}


bool TcpServer::run()
{
    if (!started_)
    {
        start();
    }

    return uv_run(loop_, UV_RUN_DEFAULT);
} 


void TcpServer::clean_connections()
{
    for (auto it = connections_.begin(); it != connections_.end();)
    {
        if (!(*it)->is_connected())
        {
            ilog("find offline connection, delete it");
            it = connections_.erase(it);
        }
        else 
        {
            ++ it;
        }
    }
}

/**
 * @brief 清空所有连接
 * 
 */
void TcpServer::close_all_connections()
{
    if (connections_.size() > 0)
    {
        decltype(connections_)().swap(connections_);
    }    
}


/**
 * @brief 获取连接列表
 * 
 * @return std::vector<nos::network::ClientInfo> 
 */
std::vector<nos::network::ClientInfo> TcpServer::get_clients()
{
    std::vector<nos::network::ClientInfo> clients;

    for (auto it = connections_.begin(); it != connections_.end(); ++it)
    {
        clients.push_back((*it)->client_info());
    }

    return clients;
}


/**
 * @brief 获取连接数量 
 * 
 * @return int 
 */
int TcpServer::get_clients_num()
{
    return connections_.size();
}


/**
 * @brief 当有新的连接请求时的处理函数
 * 
 * @param status uv提供的状态， < 0 表示 错误
 */
void TcpServer::on_new_connection(int status)
{
    if (status < 0)
    {
        elog("callback error: {}", uv_strerror(status));
        return ;
    }

    auto conn = std::make_unique<TcpConnection>(*this);

    if (conn->accept(stream_))
    {
        // 启动读操作
        conn->read_start();
        // 添加到连接列表
        connections_.emplace_back(std::move(conn));
    }
}


} // network

} // nos
