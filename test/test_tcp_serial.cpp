


/**
 * @file test_tcp_server.cpp
 * @author Liu Chuansen (samule@neptune-robotics.com)
 * @brief 演示如何使用TCP服务
 * @version 0.1
 * @date 2023-05-17
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#include <iostream>
#include <thread>
#include <chrono>

#include <sys/stat.h>

#include <common/logger.h>
#include <common/sys_time.h>
#include <common/uv_helper.h>
#include <common/tcp_server.h>
#include <common/serial_port.h>

#include "local_option.h"

#define APP_NAME  "test_tcp2serial"

#define APP_VERSION  APP_NAME " v1.0"

class SacpClient
{

public:
    SacpClient(std::string const &name, 
        std::string const & serial, 
        int serial_rate, 
        int tcp_port) 
        : name_(name), 
        serial_rate_(serial_rate), 
        serial_(serial), 
        tcp_("tcp-debug", "0.0.0.0", tcp_port)
    {        

    }

    ~SacpClient() { }


    bool start()
    {
        // 启动TCP调试端口
        if (!tcp_.start())                        
        {
            slog::error("start tcp server({}) failed!!", tcp_.name());
            return false;
        }

        std::string options = std::to_string(serial_rate_);

        // 启动串口
        if (!serial_.open(options.c_str()))
        {
            slog::error("open serial({}) failed!!", serial_.name());

            tcp_.stop();
            return false;
        }
        
        if (!serial_.async_read_start())
        {
            tcp_.stop();
            serial_.close();
            return false;
        }

        main_thread_ = std::thread(&SacpClient::main_task, this);

        started_ = true;

        return true;       
    }

    void stop()
    {
        main_running_ = false;
        main_thread_.join();
    }

private:
    std::string name_;
    nos::driver::SerialPort serial_;
    nos::network::TcpServer tcp_;
    int serial_rate_;
    std::thread main_thread_;
    bool main_running_;

    bool started_;


    void main_task()
    {

        uint8_t buf[256];

        main_running_ = true;

        while(main_running_)
        {        
            // 从串口接收到数据
            int size = serial_.async_read(buf, sizeof(buf));    
            if (size > 0)
            {
                tcp_.send(tcp_.AllClients, buf, size);                
            }

            if (tcp_.received_frames_num() > 0)
            {
                auto f = tcp_.receive();

                if (!f.is_empty())
                {
                    serial_.write(f.data_pointer(), f.size());
                }
            }        
        }
    }
};




/**
 * @brief 主函数
 * 
 * @param argc 
 * @param argv 
 * @return int 
 */
int main(int argc, const char *argv[])
{
    // 解析参数
    auto opt = nos::chassis::LocalOption(APP_VERSION, argc, argv);
    // 显示参数
    //opt.dump();

    if (!opt.check())
    {
        return 1;
    }

    // 先初始化日志
    slog::make_logger(APP_NAME, opt.get_log_level());
    slog::info("{} started, build time: {} {}", APP_VERSION,  __DATE__, __TIME__);

    //spdlog::enable_backtrace(32);
    uv::Loop loop;

    // 注册信号处理函数
    auto signal_handle = [](uv::Loop &loop, int signum){

            slog::warning("-> handle {}", signum);
            // if (signum == SIGABRT)
            // {
            //     spdlog::dump_backtrace();
            // }
            
            loop.stop();
        };

    loop.signal(SIGINT, signal_handle);
    loop.signal(SIGTERM, signal_handle);
    loop.signal(SIGKILL, signal_handle);
    loop.signal(SIGABRT, signal_handle);

    // 创建一个sacp客户端实例
    SacpClient sacp("sacp-client", opt.get_string(opt.SerialPort), opt.get_int(opt.Baudrate), opt.get_int(opt.DebugTcpPort));

    sacp.start();

    // 创建一个定时器，做一些事情 
    uv::Timer timer(loop.get());
    timer.start(1000, 5000, [](uv::Timer &timer){
        slog::info("timer function");
        // int x = 0;
        // int y = 10;
        // int z = y / x;
        // //std::string a = { 0};
        // std::cout << z << std::endl;
        // //abort();
    });

    loop.spin();

    sacp.stop();

    slog::warning(APP_NAME "  exited");

    return 0;
}

