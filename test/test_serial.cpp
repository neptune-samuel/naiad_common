



#include <iostream>
#include <thread>
#include <chrono>

#include <common/logger.h>
#include <common/sys_time.h>
#include <common/libuv_utils.h>
#include <common/serial_port.h>

#define APP_NAME  "test-serial"

#define SIMPLE_READ  0
#define ASYNC_READ   1

int main(int argc, const char *argv[])
{
    // 先初始化日志
    nos::logger_init(APP_NAME, spdlog::level::trace);

    ilog(APP_NAME " started, build time: {} {}", __DATE__, __TIME__);

    nos::libuv::Loop loop;
    nos::libuv::Timer timer(loop.get());

    const char *dev = "/dev/ttyUSB0";
    if (argc > 1)
    {
        dev = argv[1];
    }

    nos::driver::SerialPort port(dev);

    port.open("115200");

    ilog("open port({}) {}", port.name(), port.is_opened() ? "success" : "failed");

    if (port.is_opened())
    {
        #if SIMPLE_READ
        while (true)
        {
            unsigned char buf[1024];

            int read_size = port.read(buf, sizeof(buf), 0);
            if (read_size > 0)
            {
                port.write(buf, read_size);
            }

            if (read_size < 0)
            {
                elog("read failed");
            }
            
            nos::system::mdelay(1);
        }

        // timer.start(100, 1000, &port, [](nos::libuv::Timer &timer, void *data)
        //     {
        //         auto port = static_cast<nos::driver::SerialPort *>(data);

        //         port->write("Hello world", 11);                
        //     });

        #endif 

        #if ASYNC_READ
        port.async_read_start();


        timer.start(2000, 1000, &port, [](nos::libuv::Timer &timer, void *data)
            {
                auto port = static_cast<nos::driver::SerialPort *>(data);

                //timer.stop();
                nos::driver::SerialStatistics stats = { 0 };
                port->get_statistics(stats);

                ilog("fifo: {} peak {}", stats.fifo_size, stats.fifo_peak_size);
                ilog("tx  : {} ", stats.tx_bytes);
                ilog("rx  : {} drop {}", stats.rx_bytes, stats.rx_drop_bytes);

                //port->async_read_stop();                
            });

        #endif 
    }

    #if ASYNC_READ
    // 创建一个接收线程，从fifo中接收数据，并返回指定的值
    std::thread test = std::thread([&port](){

        uint8_t buf[256];
        while(port.is_opened())
        {
            int size = port.async_read(buf, sizeof(buf));
            if (size > 0)
            {
                //ilog("rx {}", size);
                port.write(buf, size);
            }  

            nos::system::mdelay(10);              
        }

        ilog("test thread exit");
    });

    #endif 

    loop.spin();
    // 关闭串口
    port.close();
    
    #if ASYNC_READ
    test.join();
    #endif 

    wlog(APP_NAME " exited");

    return 0;
}





