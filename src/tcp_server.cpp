


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
#include <memory>
#include <algorithm>

#include <sys/socket.h>

#include <common/logger.h>
#include <common/sys_time.h>
#include <common/tcp_server.h>

namespace nos 
{

namespace network
{


/**
 * @brief 创建一个常量，表示所有主机，发送时使用
 * 
 */
const Host TcpServer::AllClients {"", 0};

/// 后续完善，增加更多的信号，如断连？
const uv::AsyncSignal::SignalId TcpServer::SignalReceiveFrame(0);
const uv::AsyncSignal::SignalId TcpServer::SignalConnectionLost(1);


/**
 * @brief TCP 连接对象
 * 
 * @note 一个TCP连接，需要实现以下功能
 *   连接初始化
 *   检测到连接断开时
 *      通知主服务删除自己
 *   检测到有可读数据时
 *      调用主服务提供的数据处理函数
 *   
 *   发送数据接口
 */
class TcpConnection
{
public:

    /// 事件类型
    enum class Event : int {ReadAvailable = 0, ConnectionLost};

    /// 事件回调函数 
    typedef std::function<void(TcpConnection &, Event, uint8_t const * const , int)> EventHandle;

    /**
     * @brief 创建一个TCP连接
     * 
     * @param server 
     */
    TcpConnection(uv_loop_t *loop, EventHandle handle) : address_(""), event_handle_(handle)
    {
        // 先初始化一个TCP连接
        uv_tcp_init(loop, &client_);

        /// 设置对象数据？
        //uv_handle_set_data((uv_handle_t *)&client_, this);
        client_.data = this;
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
    bool accept(std::string & server_name, uv_tcp_t &server)
    {
        // 如果已连接，不能进行accept
        if (connected_)
        {
            slog::warning("{}: accept duplicated", brief());
            return false;
        }

        int ret = uv_accept((uv_stream_t *)&server, (uv_stream_t *)&client_);
        if (ret != 0)
        {
            connected_ = false;
            slog::warning("{}: connection accept failed, ret={}", server_name, ret);
            return false;
        }

        connected_ = true;
        up_time_ = nos::system::uptime();

        // set tcp optoins
        {
            // enable 
            int ret = uv_tcp_nodelay(&client_, 1);
            slog::trace("{}: uv_tcp_nodelay() return {}", server_name, ret);

            // KeepAlive 时间 TODO
            ret = uv_tcp_keepalive(&client_, 1, 10);
            slog::trace("{}: uv_tcp_keepalive() return {}", server_name, ret);
        }

        // 获取地址及端口信息
        update_client_info();            
        
        slog::debug("{}: connection({}) accept success", server_name, brief());

        // 启动读函数
        uv_read_start((uv_stream_t*)&client_, []([[maybe_unused]]uv_handle_t *handle, size_t suggested_size, uv_buf_t *buf) {            
            // 申请空间
            buf->base = (char *)malloc(suggested_size);
            buf->len = suggested_size;

        }, [](uv_stream_t* stream, ssize_t nread, const uv_buf_t* buf) {
            
            // 获得连接实例
            auto conn = static_cast<TcpConnection*>(stream->data);

            // 如果有回调函数
            // 调用服务端的数据处理函数
            if (nread > 0)
            {                
                //slog::trace("receive {} bytes from ({}): {:X}", nread, conn->brief(), spdlog::to_hex((const unsigned char *)buf->base, (const unsigned char *)buf->base + nread, 16));                
                slog::trace_data(buf->base, nread, "receive {} bytes from ({}):", nread, conn->brief());


                if (conn->event_handle_)
                {
                    conn->event_handle_(*conn, Event::ReadAvailable, (unsigned char *)buf->base,  static_cast<int>(nread));
                }
            }
            else if (nread == UV_EOF)
            {
                conn->connected_ = false;
                conn->down_time_ = nos::system::uptime();

                slog::debug("tcp client({}) connection lost", conn->brief());
                if (conn->event_handle_)
                {
                    conn->event_handle_(*conn, Event::ConnectionLost, nullptr, 0);
                }
            }
            else 
            {
                // client read error ?
                slog::error("tcp client({}) read failed, ret={}", conn->brief(), uv_strerror(nread));
            }

            // 释放空间
            free(buf->base); 
        });

        return true;

    }

    /**
     * @brief 关闭连接
     * 
     */
    void close()
    {
        if (connected_)
        {
            uv_read_stop((uv_stream_t *)&client_);
            connected_ = false;
        }

        uv_close((uv_handle_t *)&client_, nullptr);
    }

    /**
     * @brief 获取连接简称
     * 
     * @return std::string 
     */
    std::string const brief() const
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

    std::string const & get_address() const 
    {
        return address_;
    }

    int get_port() const 
    {
        return port_;
    }

    /**
     * @brief 返回一个主机对象
     * 
     * @return Host const 
     */
    Host const get_host() const 
    {
        return Host{address_, port_};
    }

    /**
     * @brief 获取客户端信息
     * 
     * @param info 
     */
    void update_client_info(ClientInfo &info)
    {
        info.address = address_;
        info.port = port_;
        info.up_time = up_time_;
        info.down_time = down_time_;
        info.connected = connected_;
    }

    /**
     * @brief 发送数据到客户端
     * 
     * @param data 数据 
     * @param size 长度
     * @return int 
     */
    int send(const uint8_t * const data, int size)
    {
        if (connected_ && data && (size > 0))
        {
            uv_write_t *req = new uv_write_t;
            uv_buf_t buf = uv_buf_init((char *)data, size);

            slog::trace_data(data, size, "send {} bytes to ({}):", size, brief());

            int ret = uv_write(req, (uv_stream_t *)&client_, &buf, 1, [](uv_write_t *req, int status){
                    if (status < 0)
                    {
                        slog::warning("{uv_write() callback error:{}", uv_strerror(status));
                    }

                    // 删除uv_write
                    delete req;
                });

            slog::trace("{}: uv_write(size={}) return {}", brief(), size, ret);
            return ret;
        }

        return 0;
    }

    /**
     * @brief 发送一个数据帧
     * 
     * @param frame 
     * @return int 
     */
    int send(DataFrame const &frame)
    {
        if (frame.is_empty())
        {
            return 0;
        }

        return send(frame.data_pointer(), frame.size());        
    }

private:
    /// 连接对象
    uv_tcp_t client_;
    /// 连接状态
    bool connected_ = false;
    // 连接地址
    std::string address_;
    // 连接端口
    int port_ = 0;

    nos::system::SysTick up_time_;
    nos::system::SysTick down_time_;
    
    /// 事件处理函数
    EventHandle event_handle_;

    /**
     * @brief 更新地址信息
     * 
     */
    void update_client_info()
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
            slog::error("Unknown address family: {}", addr.ss_family);
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
TcpServer::TcpServer(std::string const &name, 
    std::string const &address, 
    int port, 
    int max_clients_num) : 
    uv::TcpServer(uv::Loop::Type::New),     
    address_(address), 
    port_(port),
    name_(name),     
    max_clients_num_(max_clients_num)
{
    /// 设置对象数据？
    //uv_handle_set_data((uv_handle_t*)&server_, this);
    server_.data = this;

    // 设置brief
    brief_ = address_ + ":" + std::to_string(port_);

    slog::debug("create tcp server({}) with {}", name_, brief_);
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
std::string const & TcpServer::name()
{
    return name_;
}


/**
 * @brief 获取一个简称
 * 
 * @return const std::string& 
 */
std::string const & TcpServer::brief()
{
    return brief_;
}



bool TcpServer::start()
{
    if (started_)
    {
        slog::warning("start {} failed, it seems already started", name_);
        return false;
    }

    int ret = bind(address_, port_);
    if (ret != 0)
    {
        slog::error("{}: bind to {} failed: {}", name_, brief_, uv_strerror(ret));
        return false;
    }

    int backlog = 128;

    ret = listen(backlog, [](uv_stream_t *server, int status)
        {
            auto self = static_cast<TcpServer*>(server->data);

            if (status < 0)
            {
                slog::error("{}: listen callback return unexpected error: {}", self->name(), uv_strerror(status));
                return ;
            }
            
            // 建立一个新连接
            self->setup_connection();      
        });

    if (ret != 0)
    {
        slog::error("{}: listen failed: {}", name_, uv_strerror(ret));
        return false;
    }

    slog::info("{}: listen on {} success", name_, brief_);

    // 创建一个线程，运行uv loop
    thread_ = std::thread(&TcpServer::loop_thread, this);

    return true;
}


/**
 * @brief 停止TCP服务
 * 
 * @note 实现原则， stop后，还能使用start再次运行
 */
void TcpServer::stop()
{

    if (started_)
    {
        // 关闭关送通知
        tx_notify_.close();

        close_all_connections();

        // 清空接收和发送FIFO
        {
            std::lock_guard<std::mutex> lock(rx_mutex_);
            decltype(rx_frames_)().swap(rx_frames_);
        }

        {
            std::lock_guard<std::mutex> lock(tx_mutex_);
            decltype(tx_frames_)().swap(tx_frames_);
        }

        uv_tcp_close_reset(&server_, nullptr);

        thread_exit_ = true;
        
        //uv_stop(loop_);
        async_stop();

        // 停止线程
        slog::trace("-> wait for thread exit");

        thread_.join();
    }
}

/**
 * @brief 绑定一个信号到外部loop
 * 
 * @param signal 信号
 * @param uv_loop uv_loop
 * @param signal_handle 信号处理函数，在外部loop中调用
 */
void TcpServer::signal_bind(int signal, uv_loop_t *uv_loop, uv::AsyncSignal::Function signal_handle)
{
    if (signal == SignalReceiveFrame)
    {
        rx_notify_.bind(uv_loop, signal_handle);
    }
    else 
    {
        slog::warning("unsupported signal:{}", signal);
    }
}


/**
 * @brief TCP服务是否正在运行中
 * 
 * @return true 
 * @return false 
 */
bool TcpServer::is_running()
{
    return started_;
}

/**
 * @brief LOOP 线程
 * 
 */
void TcpServer::loop_thread()
{
    started_ = true;

    thread_exit_ = false;

    slog::trace("{}: loop thread started", name_);

    tx_notify_.bind(loop_, [this]([[maybe_unused]]int id){

        slog::trace("{}: get tx notify", name_);
        // 检查发送队列，是否有数据需要发送
        int pendings = 0;

        {
            std::lock_guard<std::mutex> lock(tx_mutex_);
            pendings = tx_frames_.size();
        }

        while (pendings > 0)
        {
            DataFrame frame(0);

            {
                std::lock_guard<std::mutex> lock(tx_mutex_);                
                frame = std::move(tx_frames_.front());
                tx_frames_.pop();                    
                pendings = tx_frames_.size();
            }

            slog::debug("{}: pop tx frame-{}, pending:{}", name_, frame.id(), pendings);
                        
            if (!frame.is_empty())
            {
                Host const & host = frame.get_host();
                // 如果port 为0，表示发给所有的客户端
                if (host.port == 0)
                {
                    for (auto &it : connections_)
                    {
                        slog::debug("{}: send frame-{} to host:{}", name_, frame.id(), (*it).brief());
                        (*it).send(frame);
                    }
                }
                else 
                {
                    // 从连接中找到这个客户端
                    auto it = std::find_if(connections_.begin(), connections_.end(), [&](const std::unique_ptr<TcpConnection>& conn) {
                        return (((*conn).get_address() == host.address) && ((*conn).get_port() == host.port)); 
                    });

                    if (it != connections_.end())
                    {
                        slog::debug("{}: send frame-{} to host:{}", name_, frame.id(), it->get()->brief());
                        it->get()->send(frame);                    
                    }
                    else 
                    {
                        slog::warning("{}: send failed, not such host({}:{})", name_, host.address, host.port);
                    }
                }            
            }            
        }
    });

    uv_run(loop_, UV_RUN_DEFAULT);   

    slog::trace("{}: loop thread exited", name_);    

    started_ = false;
}


/**
 * @brief 获取当前连接数
 * 
 * @return int 
 */
int TcpServer::connections_num()
{
    return connections_.size();
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
 */
void TcpServer::setup_connection()
{
    auto conn = std::make_unique<TcpConnection>(get_loop(), 
        // 连接事件处理函数
        [this](TcpConnection &connection, TcpConnection::Event event, uint8_t const * const data, int size){

            slog::debug("{}: client({}) event: {}", name_, connection.brief(), static_cast<int>(event));

            if (event == TcpConnection::Event::ConnectionLost)
            {
                slog::warning("{}: connection({}) lost, removed", name_, connection.brief());

                auto it = std::find_if(connections_.begin(), connections_.end(), [&connection](const std::unique_ptr<TcpConnection>& conn) {
                        // 使用brief()这个不可靠
                        //return conn->brief() == connection.brief();
                        // 使用指针指向它比较好
                        return conn.get() == &connection;
                    });

                if (it != connections_.end())
                {
                    slog::trace("find connection({}), remove it", connection.brief());

                    // 更新客户端信息
                    connection.update_client_info(get_client_info(connection.get_address(), connection.get_port()));

                    connections_.erase(it);
                }
            }
            else if (event == TcpConnection::Event::ReadAvailable) 
            {
                // 创建一个队列 
                DataFrame frame(connection.get_host(), data, size);
                slog::debug("{}: queue rx frame-{}(size:{}, from:{}) pending:{}", name_, frame.id(), size, connection.brief(), rx_frames_.size());

                // 入队列 
                std::lock_guard<std::mutex> lock(rx_mutex_);
                rx_frames_.emplace(std::move(frame));

                // 通知外部线程读取数据
                rx_notify_.notify();
            }
        }
    );

    if (conn->accept(name_, server_))
    {
        // 添加到连接列表
        std::string const & client = conn->brief();

        // 更新客户端信息
        //ClientInfo info;
        conn->update_client_info(get_client_info(conn->get_address(), conn->get_port()));
        
        // 加入容器
        connections_.emplace_back(std::move(conn));

        // give a log
        slog::info("{}: connection({}) setup success, total: {}", name_, client, connections_.size());
    }
}

/**
 * @brief 获取一个客户端信息对象
 * 
 * @param address 
 * @param port 
 * @return ClientInfo& 
 */
ClientInfo & TcpServer::get_client_info(std::string const & address, int port)
{
    // 查找这个客户端在不在，如果不在就创建一个新的。    
    auto it = std::find_if(clients_.begin(), clients_.end(), [&](const std::unique_ptr<ClientInfo>& client) {
            return (((*client).address == address) && ((*client).port == port)); 
        });
    
    if (it != clients_.end())
    {
        return *(it->get());
    }

    // 没有找到，创建一个新的
    auto client = std::make_unique<ClientInfo>();
    (*client).address = address;
    (*client).port = port;

    ClientInfo & ret = *client;
    clients_.emplace_back(std::move(client));

    return ret;
}

/**
 * @brief 以info级别显示客户端状态信息
 * 
 */
void TcpServer::dump_clients()
{
    for (auto &it : clients_)
    {
        auto &client = *it;
        slog::info("{}: {}:{} {} at {}", name_, client.address, client.port, 
            client.connected ? "connected" : "disconnected", 
            client.connected ? nos::system::SysTick(client.up_time).to_time_string() \
                : nos::system::SysTick(client.down_time).to_time_string());
    }
}

/**
 * @brief 返回当前接收的帧数量
 * 
 * @return int 
 */
int TcpServer::received_frames_num()
{
    std::lock_guard<std::mutex> lock(rx_mutex_);    
    return rx_frames_.size();    
}


/**
 * @brief 对队列中接收一帧数据
 * 
 * @return DataFrame 如果DataFrame.is_empty() 为真，表示未接收到数据
 * @note 不使用C++17的特性
 */
DataFrame TcpServer::receive()
{
    std::lock_guard<std::mutex> lock(rx_mutex_);    
    
    if (rx_frames_.empty())
    {
        return std::move(DataFrame(0));
    }

    DataFrame frame = std::move(rx_frames_.front());
    rx_frames_.pop();
    
    slog::debug("{}: pop rx frame-{}, pending:{}", name_, frame.id(), rx_frames_.size());

    return frame;
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
bool TcpServer::send(Host const & host, uint8_t const * const data, int size)
{
    if (data == nullptr || size <= 0)
    {
        return false;
    }

    DataFrame frame(host, data, size);

    slog::debug("{}: queue tx frame-{}(size:{}, to:{}:{}) pending:{}", name_, frame.id(), size, host.address, host.port, tx_frames_.size());

    std::lock_guard<std::mutex> lock(tx_mutex_);
    tx_frames_.emplace(std::move(frame));

    // notify tx ready
    tx_notify_.notify();

    return true;
}

    /**
 * @brief 发送一个准备好帧
 * 
 * @param frame 
 * @return true 
 * @return false 
 */
bool TcpServer::send(DataFrame const &frame)
{
    return send(frame.get_host(), frame.data_pointer(), frame.size());    
}

// /**
//  * @brief 对队列中接收一帧数据
//  * 
//  * @return DataFrame 
//  */
// std::optional<DataFrame> TcpServer::receive()
// {
//     return std::nullopt;
// }


// /**
//  * @brief 发送数据到指定客户端
//  * 
//  * @param host 指定主机，如果为std::nullopt， 表示发给所有主机
//  * @param data 需要发送的数据
//  * @param size 
//  * @return true 发送成功
//  * @return false 发送失败
//  */
// bool TcpServer::send(std::optional<Host> host, uint8_t const *data, int size)
// {
//     return false;
// }



} // network

} // nos

