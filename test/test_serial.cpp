



#include <iostream>
#include <thread>
#include <chrono>

#include <common/logger.h>
#include <common/sys_time.h>
#include <common/uv_helper.h>
#include <common/serial_port.h>

#define APP_NAME  "test-serial"

#define SIMPLE_READ  0
#define ASYNC_READ   1

int main(int argc, const char *argv[])
{
    // 先初始化日志
    slog::make_stdout_logger(APP_NAME, slog::LogLevel::Debug);

    slog::info(APP_NAME " started, build time: {} {}", __DATE__, __TIME__);

    uv::Loop loop;
    uv::Timer timer;

    const char *dev = "/dev/ttyUSB0";
    if (argc > 1)
    {
        dev = argv[1];
    }

    naiad::driver::SerialPort port(dev);

    port.open("115200");

    slog::info("open port({}) {}", port.name(), port.is_opened() ? "success" : "failed");

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
                slog::error("read failed");
            }
            
            naiad::system::mdelay(1);
        }

        #endif 

        #if ASYNC_READ
        port.async_read_start();

        timer.bind(loop);
        timer.start(2000, 1000, [&port]()
            {
                //auto port = static_cast<naiad::driver::SerialPort *>(data);

                //timer.stop();
                naiad::driver::SerialStatistics stats = port.get_statistics();

                slog::info("fifo: {} peak {}", stats.fifo_size, stats.fifo_peak_size);
                slog::info("tx  : {} ", stats.tx_bytes);
                slog::info("rx  : {} drop {}", stats.rx_bytes, stats.rx_drop_bytes);

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
                //slog::info("rx {}", size);
                port.write(buf, size);
            }  

            naiad::system::mdelay(10);              
        }

        slog::info("test thread exit");
    });

    #endif 

    loop.spin();
    // 关闭串口
    port.close();
    
    #if ASYNC_READ
    test.join();
    #endif 

    slog::warning(APP_NAME " exited");

    return 0;
}





