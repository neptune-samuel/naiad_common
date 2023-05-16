


#include <iostream>
#include <thread>
#include <chrono>

#include <common/logger.h>
#include <common/sys_time.h>
#include <common/libuv_utils.h>
#include <common/serial_port.h>
#include <common/tcp_server.h>

#include <uv.h>

#define APP_NAME  "nos-common"


int main(int argc, const char *argv[])
{
    // 先初始化日志
    nos::logger_init(APP_NAME, spdlog::level::trace);

    ilog(APP_NAME " started, build time: {} {}", __DATE__, __TIME__);

    nos::libuv::RunLoop loop(true);

    nos::libuv::Timer timer(loop());

    //nos::network::TcpServer tcp(loop(), "0.0.0.0", 9901);
    //tcp.start();

    nos::driver::SerialPort port("/dev/ttyUSB0");

    port.open("115200");

    ilog("open port {}", port.is_opened() ? "success" : "failed");

    if (port.is_opened())
    {
        // while (true)
        // {
        //     unsigned char buf[1024];

        //     int read_size = port.read(buf, sizeof(buf), 10);
        //     if (read_size > 0)
        //     {
        //         port.write(buf, read_size);
        //     }
        //     //nos::system::mdelay(1);
        // }

        // timer.start(100, 1000, &port, [](nos::libuv::Timer &timer, void *data)
        //     {
        //         auto port = static_cast<nos::driver::SerialPort *>(data);

        //         port->write("Hello world", 11);                
        //     });


        port.async_read_start();

        timer.start(10000, 1000, &port, [](nos::libuv::Timer &timer, void *data)
            {
                auto port = static_cast<nos::driver::SerialPort *>(data);

                timer.stop();

                ilog("timer's up, stop read");
                port->async_read_stop();                
            });
    }


    loop.spin();

    wlog(APP_NAME " exited");

    return 0;
}





