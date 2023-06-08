
#include <iostream>
#include <uv.h>
#include <thread>

#include <common/uv_helper.h>

int main(int argc, const char *argv[])
{

    uv::Loop loop(uv::Loop::Type::New);

    auto signal_handle = [](uv::Loop &loop, int signum){
            std::cout << "get signal" << signum << std::endl;
            loop.stop();
        };

    loop.signal(SIGINT, signal_handle);
    loop.signal(SIGTERM, signal_handle);
    loop.signal(SIGKILL, signal_handle);

    uv::Timer timer;

    // 在绑定时指定函数
    // timer.bind(loop, [](){
    //     std::cout << "timer test" << std::endl;
    // });
    //timer.start(2000, 1000);

    // 也可以在启动时绑定函数
    timer.bind(loop);
    timer.start(1000, 1000, [](){
        std::cout << "timer test" << std::endl;
    });

    uv::Timer do_stop;
    do_stop.bind(loop, [&](){
        std::cout << "stop timer1" << std::endl;
        timer.stop();
        do_stop.stop();
    });

    do_stop.start(5000);

    uv::Timer restart_timer;

    restart_timer.bind(loop);
    restart_timer.start(8000, [&](){
        std::cout << "restart timer1" << std::endl;
        timer.restart();
    });


    uv::AsyncSignal event(10);
    event.bind(loop, [](int id){
        std::cout << "get event:" << id << std::endl;
    });

    auto th = std::thread([&event]{
        std::this_thread::sleep_for(std::chrono::seconds(5));
        event.notify();
    });


    loop.spin();

    th.join();

    std::cout << "test done" << std::endl;

    return 0;
}
