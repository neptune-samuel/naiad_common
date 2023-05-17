
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
#include <common/network_client.h>
#include <common/libuv_utils.h>
#include <common/tcp_server.h>

#define APP_NAME  "tcpserver"


int main(int argc, const char *argv[])
{
    // 先初始化日志
    nos::logger_init(APP_NAME, spdlog::level::debug);

    ilog(APP_NAME " started, build time: {} {}", __DATE__, __TIME__);

    //nos::libuv::RunLoop loop(true);
    auto loop = std::make_unique<nos::libuv::RunLoop>(true);

    nos::network::TcpServer server(loop->get_loop(), "0.0.0.0", 9900);
    nos::network::TcpServer server1(loop->get_loop(), "0.0.0.0", 9901);
    server.start();
    server1.start();

    nos::libuv::Timer timer(loop->get_loop());

    timer.start(100, 1000, &server, [](nos::libuv::Timer &timer, void *data)
        {
            nos::network::TcpServer *server = static_cast<nos::network::TcpServer *>(data);
            
            auto clients = server->get_clients();
            for (auto it = clients.begin(); it != clients.end(); ++ it)
            {
                ilog("==> {} {} {}", it->brief(), it->connected ? "connected at" : "disconnected at", it->up_time.to_time_string());
            }
        });

    loop->spin();

    elog(APP_NAME "exited");

    return 0;
}



