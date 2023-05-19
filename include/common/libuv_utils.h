
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

#include <uv.h>

namespace nos
{
namespace libuv 
{

/**
 * @brief 建立LOOP的类型
 * 
 */
enum class LoopType : int {
    Default = 0,
    New,
};

enum class RunMode : int 
{
    Default = 0,
    Once,
    NoWait,
};


class RunLoop 
{

public:
    /**
     * @brief 创建一个LOOP
     * 
     * @param type 类型 :: LoopType 
     * 
     * @note 使用 explict 修饰，避免隐式转换 如: RunLoop a = LoopType::Default  将报错
     */
    explicit
    RunLoop(LoopType type = LoopType::Default) 
    {
        if (type == LoopType::Default)
        {
            loop_ = uv_default_loop();
        }
        else 
        {
            loop_ = uv_loop_new();

            // 初始化loop
            uv_loop_init(loop_);
        }

        // TODO: to support more signals

        uv_signal_init(loop_, &signal_);

        uv_signal_start(&signal_, [](uv_signal_t *handle, int signum){
            std::cout << "***Receive signal: " << signum << std::endl;
            // 停止loop
            uv_stop(handle->loop);    

            trace("call uv_stop() done");

        }, SIGINT);

    }

    ~RunLoop()
    {
        // 停止信号
        int ret = uv_signal_stop(&signal_);
        trace("call uv_signal_stop() return {}", uv_strerror(ret));

        // if (loop_ != uv_default_loop())
        // {
        //     trace("not a default loop");

        //     // 关闭loop
        //     uv_loop_close(loop_);

        //     trace("--> uv_loop_delete");
        //     uv_loop_delete(loop_);
        //     trace("<-- uv_loop_delete");

        // }

        trace("--> uv_loop_delete");
        uv_loop_delete(loop_);
        trace("<-- uv_loop_delete");

        trace("loop destroy");
    }

    // 禁止复制构造
    RunLoop(const RunLoop &) = delete;
    RunLoop & operator=(const RunLoop &) = delete;

    /**
     * @brief 直接返回loop
     * 
     * @return uv_loop_t* 
     */
    uv_loop_t *operator()()
    {
        return loop_;
    }

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
     * @brief 运行
     * 
     * @param mode 
     */
    void run(RunMode mode)
    {
        uv_run(loop_, (uv_run_mode)mode);
    }

    /**
     * @brief 默认模式运行
     * 
     */
    void spin()
    {
        uv_run(loop_, UV_RUN_DEFAULT);
    }

private:
    uv_loop_t *loop_;  
    uv_signal_t signal_; 
};

// 声明定时器
class Timer;

// 定义一个定时器回调函数类型
typedef void(*TimerFunction)(Timer &timer, void *data);

class Timer
{

public:
    Timer(uv_loop_t *loop) : data_(nullptr), function_(nullptr)
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
            function_(*this, data_);
        }
    }

    /**
     * @brief 启动一个定时器
     * 
     * @param delay_ms 延时时间
     * @param period_ms 周期时间
     * @param data 传递给回调函数的数据
     * @param function 回调函数
     */
    void start(int delay_ms, int period_ms, void *data, TimerFunction function)
    {
        data_ = data;
        function_ = function;

        uv_timer_start(&timer_, [](uv_timer_t *handle)
            {
                auto timer = static_cast<Timer *>(handle->data);
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
    void *data_;
    TimerFunction function_;
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
    TcpServer(bool default_loop)
    {
        if (default_loop)
        {
            loop_ = uv_default_loop();
        } 
        else 
        {
            // 创建一个新的端口
            loop_ = uv_loop_new();

            // 初始化loop
            uv_loop_init(loop_);
        }

        // 初始化一个TCP的服务
        uv_tcp_init(loop_, &server_);        
    }

    ~TcpServer()
    {
        // 关闭端口
        uv_close((uv_handle_t *)&server_, nullptr);

        if (loop_ != uv_default_loop())
        {
            uv_loop_close(loop_);
            uv_loop_delete(loop_);
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

