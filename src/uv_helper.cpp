

#include <string>
#include <iostream>
#include <functional>
#include <map>

#include <uv.h>

#include <common/logger.h>
#include <common/uv_helper.h>

namespace uv {  


Loop::Loop(Loop::Type type) 
{
    if (type == Type::Default)
    {
        loop_ = uv_default_loop();

        //slog::trace("use default loop");
    }
    else 
    {
        loop_ = new uv_loop_t;

        // 初始化loop
        uv_loop_init(loop_);

        //slog::trace("create new loop success");
    }

    // 创建一个异步停止的回调函数
    async_stop_.data = (void *)this;
    uv_async_init(loop_, &async_stop_, [](uv_async_t *handle){

        auto loop = reinterpret_cast<Loop *>(handle->data);
        loop->stop();
        //uv_stop(handle->loop);
    });
}


Loop::~Loop()
{
    uv_loop_close(loop_);

    // delete the loop_
    if (loop_ != uv_default_loop())
    {
        delete loop_;
        loop_ = nullptr;
    }

    //slog::trace("loop destroy");
}

/**
 * @brief 运行
 * 
 * @param mode 
 */
void Loop::run(RunMode mode)
{
    uv_run(loop_, (uv_run_mode)mode);

    //slog::trace("uv_run(mode={}) return {}", static_cast<int>(mode), ret);
}

/**
 * @brief 默认模式运行
 * 
 */
void Loop::spin()
{
    uv_run(loop_, UV_RUN_DEFAULT);
    //slog::trace("uv_run(spin) return {}", ret);
}

/**
 * @brief 停止loop
 * 
 */
void Loop::stop()
{
    // 停止信号
    for (auto &s : signals_)
    {
        auto signal = s.second.get();
        // stop it
        //slog::trace("--> uv_signal_stop({})", signal->signum);
        uv_signal_stop(&signal->object);
    }

    uv_stop(loop_);
}

/**
 * @brief 异步停止loop
 * 
 */
void Loop::async_stop()
{
    uv_async_send(&async_stop_);
}


/**
 * @brief 注册信号处理函数
 * 
 * @param signum 
 * @param function 
 * @return true 
 * @return false 
 */
bool Loop::signal(int signum, SignalFunction function)
{
    auto check = signals_.find(signum);
    if (check != signals_.end())
    {
        slog::warning("signal({}) already exist", signum);
        return false;
    }

    auto signal = std::make_unique<Signal>();
    signal->signum = signum;
    signal->function = function;

    // 开始初始化
    uv_signal_init(loop_, &signal->object);
    signal->object.data = this;

    uv_signal_start(&signal->object, [](uv_signal_t *handle, int signum){

        std::cout << "***Receive signal: " << signum << std::endl;

        Loop *lp =  reinterpret_cast<Loop*>(handle->data);
        //Loop *l = (Loop *)handle->data;
        auto signal = *(lp->signals_[signum]);
        if (signal.function)
        {
            signal.function(*lp, signum);
        }

    }, signum);  

    // insert to map
    signals_.insert(std::make_pair(signum, std::move(signal)));

    slog::trace("signal:{} registered", signum);

    return true;
}

/**
 * @brief Construct a new Tcp Server object
 * 
 * @param address TCP地址 
 * @param port TCP端口
 */
TcpServer::TcpServer(Loop::Type type)
{
    if (type == Loop::Type::Default)
    {
        loop_ = uv_default_loop();
    } 
    else 
    {
        // 创建一个新的端口
        loop_ = new uv_loop_t;

        // 初始化loop
        uv_loop_init(loop_);
    }   

    uv_async_init(loop_, &async_stop_, [](uv_async_t *handle){
        uv_stop(handle->loop);
    });
}

TcpServer::~TcpServer()
{
    uv_loop_close(loop_);

    // 关闭loop
    if (loop_ != uv_default_loop())
    {
        delete loop_;
    }
}

/**
 * @brief 绑定到指定接口
 * 
 * @param ip 
 * @param port 
 * @return true 
 * @return false 
 */
int TcpServer::bind(const std::string &ip, int port)
{
    struct sockaddr_in addr;        
    
    // 初始化一个TCP的服务
    uv_tcp_init(loop_, &server_);     
    
    int ret = uv_ip4_addr(ip.c_str(), port, &addr);    
    if (ret != 0)
    {
        slog::warning("Invalid tcp server address({}) or port({})", ip, port);
        return ret;
    }
    
    return uv_tcp_bind(&server_, (const struct sockaddr *)&addr, 0);
}    

/**
 * @brief 启用监听
 * 
 * @param backlog 
 * @return true 
 * @return false 
 */
bool TcpServer::listen(int backlog, uv_connection_cb callback)
{
    return uv_listen((uv_stream_t *)&server_, backlog, callback);
}

void TcpServer::async_stop()
{
    uv_async_send(&async_stop_);
}


} // uv

