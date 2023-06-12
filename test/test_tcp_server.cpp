
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

int main()
{
    // 先初始化日志
    slog::make_stdout_logger(APP_NAME, slog::LogLevel::Trace);
    slog::info(APP_NAME " started, build time: {} {}", __DATE__, __TIME__);

    uv::Loop loop(uv::Loop::Type::New);

    auto signal_handle = [&](int signum){
            slog::trace("-> handle {}", signum);
            loop.stop();
        };

    loop.signal(SIGINT, signal_handle);
    loop.signal(SIGTERM, signal_handle);
    loop.signal(SIGKILL, signal_handle);

    naiad::network::TcpServer tcp("test", "0.0.0.0", 9702);

    tcp.start();

    // 测试TCP关闭
    // uv::Timer timer;
    // timer.bind(loop);
    // timer.start(5000, [&tcp](){
    //         slog::info("stop tcp server");
    //         tcp.stop();
    //         timer.stop();
    //     });

    // 接收和发送处理 -- 使用独立线程
    // std::thread tcp_data = std::thread([&tcp](){
    //     while (tcp.is_running())
    //     {
    //         while (tcp.received_frames_num() > 0)
    //         {
    //             auto f = tcp.receive();
    //             if (!f.is_empty())
    //             {
    //                 tcp.send(f);
    //             }
    //         }

    //         naiad::system::mdelay(1);
    //     }
    // });

    // 接收和发送，使用当前loop接收

    tcp.signal_bind(0, loop, [&tcp]([[maybe_unused]]uv::AsyncSignal::SignalId signal){

            while (tcp.received_frames_num() > 0)
            {
                auto f = tcp.receive();
                if (!f.is_empty())
                {
                    tcp.send(f);
                }
            }
    });

    loop.spin();
    tcp.stop();

    //tcp_data.join();

    slog::error(APP_NAME " exited");

    return 0;
}



