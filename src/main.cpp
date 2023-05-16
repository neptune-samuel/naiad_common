

#include <iostream>
#include <thread>
#include <chrono>

#include <common/logger.h>
#include <common/sys_time.h>
#include <common/network_client.h>
#include <common/libuv_utils.h>
#include <common/tcp_server.h>

#include <uv.h>

#define APP_NAME  "nos-common"

#include <spdlog/fmt/bin_to_hex.h>

// void hex_example(void)
// {
//     std::vector<char> buf(0);
//     unsigned char bin[80];
//     for (int i = 0; i < 80; i++)
//     {
//         bin[i] = i;
//         buf.push_back(static_cast<char>(i & 0xff));
//     }


//     spdlog::info("buf size:{}", buf.size());
//     // 换行显示
//     spdlog::info("Binary example: {}", spdlog::to_hex(buf, 16));
//     // 不换行
//     spdlog::info("Another binary example:{:n}", spdlog::to_hex(std::begin(buf), std::begin(buf) + 10));

//     spdlog::info("uppercase: {:X}", spdlog::to_hex(buf));
//     spdlog::info("uppercase, no delimiters: {:Xs}", spdlog::to_hex(buf));
//     spdlog::info("uppercase, no delimiters, no position info: {:Xsp}", spdlog::to_hex(buf));    


//     const char *str = "hello world!";
//     spdlog::info("get str: {:n}", spdlog::to_hex(str, str + strlen(str)));

//     spdlog::info("get bin: {:X}", spdlog::to_hex(bin, bin + 60, 16));

// }


int main(int argc, const char *argv[])
{
    // 先初始化日志
    nos::logger_init(APP_NAME, spdlog::level::debug);

    //nos::SysTick now = nos::uptime();
    //nos::TimePoint now = nos::now();

    ilog(APP_NAME " started, build time: {} {}", __DATE__, __TIME__);

    //hex_example();

    // nos::system::SysTime now;
    // nos::system::SysTime prev(nos::system::now() - 60000);

    // ilog("time = {}", prev.to_string());
    // ilog("time = {}", now.to_string());

    
    // nos::system::SysTick t1;
    // nos::system::mdelay(2000);


    // now = nos::system::now();

    // ilog("time={}", now.to_string());



    // nos::system::SysTick t2;

    // auto n1 = t1.to_time();
    // auto n2 = t2.to_time();

    // ilog("t1={} t2={}", t1.get_millisecond(),  t2.get_millisecond());
    // ilog("t1={} t2={}", t1.to_time().to_string(), n2.to_string());

    // nos::network::ClientInfo ci;

    // ci.up_time = nos::system::uptime();

    // ilog("network at {}", ci.up_time.to_time_string());

    //nos::libuv::RunLoop loop(true);
    auto loop = std::make_unique<nos::libuv::RunLoop>(true);


    nos::network::TcpServer server(loop->get_loop(), "0.0.0.0", 9900);
    nos::network::TcpServer server1(loop->get_loop(), "0.0.0.0", 9901);
    server.start();
    server1.start();

    // timer(loop->get_loop());

    // timer = new Timer((*loop)())

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

    // timer.start(1000, 1000, );

    // uv_timer_t timer;
    // uv_timer_init(uv_default_loop(), &timer);
    // uv_handle_set_data((uv_handle_t *)&timer, (void *)&server);

    // uv_timer_start(&timer, [](uv_timer_t *handle)
    //     {
    //         nos::network::TcpServer *server = (nos::network::TcpServer *)handle->data;
    //         auto clients = server->get_clients();

    //         for (auto it = clients.begin(); it != clients.end(); ++ it)
    //         {
    //             ilog("==> {} {} {}", it->brief(), it->connected ? "connected at" : "disconnected at", it->up_time.to_time_string());
    //         }

    //     }, 1000, 1000);


    


    loop->spin();

    elog(APP_NAME "exited");

    return 0;
}



