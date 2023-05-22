
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

#include <common/logger.h>
#include <common/sys_time.h>

#include <common/tcp_server.h>

#define APP_NAME  "tcpserver"

int main(int argc, const char *argv[])
{
    // 先初始化日志
    nos::logger_init(APP_NAME, spdlog::level::trace);
    ilog(APP_NAME " started, build time: {} {}", __DATE__, __TIME__);

    nos::libuv::Loop loop(nos::libuv::Loop::Type::New);

    auto signal_handle = [](nos::libuv::Loop &loop, int signum){
            trace("-> handle {}", signum);
            loop.stop();
        };

    loop.signal(SIGINT, signal_handle);
    loop.signal(SIGTERM, signal_handle);
    loop.signal(SIGKILL, signal_handle);

    nos::network::TcpServer tcp("test", "0.0.0.0", 9702);

    tcp.start();

    nos::libuv::Timer timer(loop.get());

    timer.start(1000, 5000, [&tcp](nos::libuv::Timer &timer){
            //tcp.dump_clients();
        });

    // 创建一个TCP处理线程
    std::thread tcp_data = std::thread([&tcp](){
        while (tcp.is_running())
        {
            while (tcp.received_frames_num() > 0)
            {
                auto f = tcp.receive();
                if (!f.is_empty())
                {
                    tcp.send(f);
                }
            }

            nos::system::mdelay(1);
        }
    });


    loop.spin();
    tcp.stop();

    tcp_data.join();

    elog(APP_NAME " exited");

    return 0;
}



