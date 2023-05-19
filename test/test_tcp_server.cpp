
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

    nos::libuv::RunLoop loop(nos::libuv::LoopType::New);


    // nos::network::TcpServer tcp("test");

    // tcp.start();

    // nos::libuv::Timer timer(loop());

    // timer.start(1000, 1000, nullptr, [](nos::libuv::Timer &timer, void *data)
    //     {
    //         ilog("do nothing");
    //     });

    loop.spin();

    //ilog("try to stop tcp");

    //tcp.stop();

    // nos::network::TcpServer server;

    // server.test();



    // //nos::libuv::RunLoop loop(true);
    // auto loop = std::make_unique<nos::libuv::RunLoop>(true);

    // nos::network::TcpServer server(loop->get_loop(), "0.0.0.0", 9900);
    // nos::network::TcpServer server1(loop->get_loop(), "0.0.0.0", 9901);
    // server.start();
    // server1.start();


    elog(APP_NAME " exited");

    return 0;
}



