
#ifndef __LIBUV_UTILS_H__
#define __LIBUV_UTILS_H__

/**
 * @file uv_helper.h
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
#include <memory>

#include <uv.h>

namespace uv 
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
    Loop(Type type = Type::Default);

    ~Loop();

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
    void run(RunMode mode);

    /**
     * @brief 默认模式运行
     * 
     */
    void spin();

    /**
     * @brief 停止loop
     * 
     */
    void stop();

    /**
     * @brief 异步停止loop
     * 
     */
    void async_stop();

    /**
     * @brief 注册信号处理函数
     * 
     * @param signum 
     * @param function 
     * @return true 
     * @return false 
     */
    bool signal(int signum, SignalFunction function);

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

    /// 用来实现异步停止
    uv_async_t async_stop_;
};


class Timer
{
public:

    /// 定义一个信号回调函数
    typedef std::function<void(Timer &)> Function;

    Timer() : loop_(nullptr), timer_handle_(nullptr) { }

    ~Timer() 
    {
        close();
    }

    /// @brief 绑定到指定的loop
    /// @param loop 如果为空，表示使用默认的loop
    /// @param handle 
    /// @return 
    bool bind(uv_loop_t *loop, Function handle = nullptr){

        // 是否已绑定，需要关闭才能再次绑定
        if (loop_ != nullptr)
        {
            return false;
        }

        loop_ = !loop ? uv_default_loop() : loop;

        if (handle)
        {
            timer_handle_ = handle;
        }

        uv_timer_init(loop, &timer_);
        timer_.data = this;

        return true;
    }

    bool bind(Loop &loop, Function handle = nullptr){
        return bind(loop.get(), handle);
    }

    /// 停止定时器
    void stop()
    {
        if (started_)
        {
            uv_timer_stop(&timer_);
            started_ = false;            
        }
    }

    /// @brief 关闭定时器
    void close()
    {
        // 先停止
        stop();

        // handle 不为空，表示已绑定到指定loop
        if (loop_)
        {
            // 从loop中移除
            uv_close((uv_handle_t *)&timer_, nullptr);
            loop_ = nullptr;
        }

        timer_handle_ = nullptr;
    }

    /**
     * @brief 启动一个定时器
     * 
     * @param delay_ms 延时时间
     * @param period_ms 周期时间
     */
    void start(int delay_ms, int period_ms, Function handle = nullptr)
    {
        if (handle)
        {
            timer_handle_ = handle;
        }

        // 如果handle为空，不允许启动
        if (!timer_handle_){
            return ;
        }

        // 如果已启动，就不再启动了
        if (!started_)
        {
            uv_timer_start(&timer_, [](uv_timer_t *handle)
                {
                    auto self = reinterpret_cast<Timer *>(handle->data);
                    if (self && self->timer_handle_)
                    {
                        self->timer_handle_(*self);
                    }                
                }, 
                delay_ms, period_ms);
        }
        else 
        {
            // 只修改周期
            uv_timer_set_repeat(&timer_, period_ms);
        }
        
        period_ = period_ms;
        started_ = true;
    }

    void start(int period_ms, Function handle = nullptr)
    {
        start(period_ms, period_ms, handle);
    }

    // 重新运行定时器
    void restart()
    {
        start(0, period_);
    }

private:
    uv_loop_t *loop_;
    bool started_ = false;
    uv_timer_t timer_;   
    int period_ = 0;
    Function timer_handle_;
};



/// 异步信号, 用于外部进程通知本地loop消息
class AsyncSignal
{

public:

    typedef int SignalId;
    /// 定义一个信号回调函数
    typedef std::function<void(int)> Function;


    AsyncSignal(): id_(0), signal_handle_(nullptr) { };
    AsyncSignal(SignalId id) : id_(id), signal_handle_(nullptr) { };
    ~AsyncSignal() { close(); }

    /// @brief 修改ID值 
    /// @param id 
    void set_id(SignalId id)
    {
        id_ = id;
    }

    /**
     * @brief 绑定到指定的loop和handle上
     * 
     * @param loop 
     * @param handle 
     * @return bool 如果绑定已存在，将返回false
     */
    bool bind(uv_loop_t *loop, Function handle){
        if (signal_handle_){
            return false;
        }

        if (handle == nullptr){
            return false;
        }

        if (loop == nullptr) {
            loop = uv_default_loop();
        }

        signal_handle_ = handle;
        async_.data = this;
        uv_async_init(loop, &async_, [](uv_async_t *handle){
            auto self = reinterpret_cast<AsyncSignal *>(handle->data);
            if (self && self->signal_handle_){
                self->signal_handle_(self->id_);
            }
        });
        return true;
    }

    /**
     * @brief 绑定到指定的Loop中
     * 
     * @param loop 
     * @param handle 
     * @return bool 如果绑定已存在，将返回false
     */
    bool bind(Loop &loop, Function handle){
        return bind(loop.get(), handle);
    }


    /// @brief 关闭信号，可以重新绑定使用
    void close()
    {
        if (signal_handle_)
        {
            uv_close((uv_handle_t *)&async_, nullptr);
            signal_handle_ = nullptr;
        }
    }

    /**
     * @brief 通知等待线程处理当前信号函数
     * 
     */
    void notify()
    {
        if (signal_handle_)
        {
            uv_async_send(&async_);
        }
    }

private:
    SignalId id_;
    uv_async_t async_;
    Function signal_handle_;
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
    TcpServer(Loop::Type type = Loop::Type::Default);

    ~TcpServer();

    // 禁止复制构造
    TcpServer(const TcpServer &) = delete;
    TcpServer & operator=(const TcpServer &) = delete;

protected:
    uv_loop_t *loop_;
    uv_tcp_t server_;
    uv_async_t async_stop_;

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
    int bind(const std::string &ip, int port);  

    /**
     * @brief 启用监听
     * 
     * @param backlog 
     * @return true 
     * @return false 
     */
    bool listen(int backlog, uv_connection_cb callback);


    /**
     * @brief 异步停止TCP服务
     * 
     */
    void async_stop();

};


} // end uv


#endif // __LIBUV_UTILS_H__

