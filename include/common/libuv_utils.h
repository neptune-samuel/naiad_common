
#ifndef __LIBUV_UTILS_H__
#define __LIBUV_UTILS_H__

/**
 * @file libuv_utils.h
 * @author Liu Chuansen (samule@neptune-robotics.com)
 * @brief 提供一些libuv的选项
 * @version 0.1
 * @date 2023-05-15
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#include <string>
#include <iostream>
#include <functional>
#include <map>

#include <common/logger.h>

#include <uv.h>


namespace nos
{
namespace libuv 
{

/**
 * @brief Runnloop
 * 
 */
class Loop 
{

public:

    /// 类型
    enum class Type : int {
        Default = 0,
        New,
    };

    /// 运行模式
    enum class RunMode : int 
    {
        Default = 0,
        Once,
        NoWait,
    };

    /// 定义一个信号回调函数
    typedef std::function<void(Loop &, int)> SignalFunction;

    /**
     * @brief 创建一个LOOP
     * 
     * @param type 类型 :: LoopType 
     * 
     * @note 使用 explict 修饰，避免隐式转换 如: RunLoop a = LoopType::Default  将报错
     */
    //explicit
    Loop(Type type = Type::Default) 
    {
        if (type == Type::Default)
        {
            loop_ = uv_default_loop();

            trace("use default loop");
        }
        else 
        {
            loop_ = new uv_loop_t;

            // 初始化loop
            uv_loop_init(loop_);

            trace("create new loop success");
        }

    }

    ~Loop()
    {

        uv_loop_close(loop_);

        // delete the loop_
        if (loop_ != uv_default_loop())
        {
            delete loop_;
            loop_ = nullptr;
        }

        trace("loop destroy");
    }

    // 禁止复制构造
    Loop(const Loop &) = delete;
    Loop & operator=(const Loop &) = delete;

    /**
     * @brief 返回loop对象
     * 
     * @return uv_loop_t* 
     */
    uv_loop_t *get()
    {
        return loop_;
    }    

    /**
     * @brief 运行
     * 
     * @param mode 
     */
    void run(RunMode mode)
    {
        int ret = uv_run(loop_, (uv_run_mode)mode);

        trace("uv_run(mode={}) return {}", static_cast<int>(mode), ret);
    }

    /**
     * @brief 默认模式运行
     * 
     */
    void spin()
    {
        int ret = uv_run(loop_, UV_RUN_DEFAULT);
        trace("uv_run(spin) return {}", ret);
    }

    /**
     * @brief 停止loop
     * 
     */
    void stop()
    {
        // 停止信号
        for (auto &s : signals_)
        {
            auto signal = s.second.get();
            // stop it
            trace("--> uv_signal_stop({})", signal->signum);
            uv_signal_stop(&signal->object);
        }

        uv_stop(loop_);
    }

    /**
     * @brief 注册信号处理函数
     * 
     * @param signum 
     * @param function 
     * @return true 
     * @return false 
     */
    bool signal(int signum, SignalFunction function)
    {
        auto check = signals_.find(signum);
        if (check != signals_.end())
        {
            wlog("signal({}) already exist", signum);
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

        trace("signal:{} registered", signum);

        return true;
    }

private:

    struct Signal
    {
        int signum;
        SignalFunction function;
        uv_signal_t object;
    };

    uv_loop_t *loop_;  

    /// 信号处理队列
    std::map<int, std::unique_ptr<Signal>> signals_; 
};


class Timer
{

public:

    /// 定义一个信号回调函数
    typedef std::function<void(Timer &)> Function;

    /// 构造函数
    /// @param loop 
    Timer(uv_loop_t *loop) : function_(nullptr)
    {
        if (loop == nullptr)
        {
            loop = uv_default_loop();            
        }

        uv_timer_init(loop, &timer_);
        // set data to this object
        timer_.data = this;
    }

    ~Timer() 
    {
        uv_timer_stop(&timer_);
    }

    // 调用一次函数
    void call_function()
    {
        if (function_)
        {
            function_(*this);
        }
    }

    /**
     * @brief 启动一个定时器
     * 
     * @param delay_ms 延时时间
     * @param period_ms 周期时间
     * @param function 回调函数
     */
    void start(int delay_ms, int period_ms, Function function)
    {
        function_ = function;

        uv_timer_start(&timer_, [](uv_timer_t *handle)
            {
                auto timer = reinterpret_cast<Timer *>(handle->data);
                if (timer != nullptr)
                {
                    timer->call_function();
                }                
            }, 
            delay_ms, period_ms);
    }

    /**
     * @brief 停止定时器
     * 
     */
    void stop()
    {
        uv_timer_stop(&timer_);
    }

private:
    uv_timer_t timer_;   
    Function function_;
};



/**
 * @brief 一个基于libuv的TCP服务端的封装
 * 
 */
class TcpServer
{

public:
    /**
     * @brief Construct a new Tcp Server object
     * 
     * @param address TCP地址 
     * @param port TCP端口
     */
    TcpServer(Loop::Type type = Loop::Type::Default)
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
    }

    ~TcpServer()
    {
        uv_loop_close(loop_);

        // 关闭loop
        if (loop_ != uv_default_loop())
        {
            delete loop_;
        }
    }

    // 禁止复制构造
    TcpServer(const TcpServer &) = delete;
    TcpServer & operator=(const TcpServer &) = delete;


protected:
    uv_loop_t *loop_;
    uv_tcp_t server_;


    /**
     * @brief 返回loop对象
     * 
     * @return uv_loop_t* 
     */
    uv_loop_t *get_loop() const 
    {
        return loop_;
    }

    /**
     * @brief 绑定到指定接口
     * 
     * @param ip 
     * @param port 
     * @return true 
     * @return false 
     */
    int bind(const std::string &ip, int port)
    {
        struct sockaddr_in addr;        
     
        // 初始化一个TCP的服务
        uv_tcp_init(loop_, &server_);     
     
        uv_ip4_addr(ip.c_str(), port, &addr);        
        return uv_tcp_bind(&server_, (const struct sockaddr *)&addr, 0);
    }    

    /**
     * @brief 启用监听
     * 
     * @param backlog 
     * @return true 
     * @return false 
     */
    bool listen(int backlog, uv_connection_cb callback)
    {
        return uv_listen((uv_stream_t *)&server_, backlog, callback);
    }

};


} // end libuv

} // end nos

#endif // __LIBUV_UTILS_H__

