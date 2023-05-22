
#include <iostream>
#include <uv.h>
#include <common/libuv_utils.h>

#define APP_NAME "test_libuv"

int main(int argc, const char *argv[])
{
    // 先初始化日志
    nos::log::logger_init(APP_NAME, nos::log::LogLevel::Debug);
    ilog(APP_NAME " started, build time: {} {}", __DATE__, __TIME__);

    nos::libuv::Loop loop(nos::libuv::Loop::Type::New);
    //auto loop = nos::libuv::RunLoop(nos::libuv::LoopType::Default);
    //nos::libuv::Loop loop;

    int x = 10;

    auto signal_handle = [](nos::libuv::Loop &loop, int signum){
            trace("-> handle {} {}", signum);
            loop.stop();
        };

    loop.signal(SIGINT, signal_handle);
    loop.signal(SIGTERM, signal_handle);
    loop.signal(SIGKILL, signal_handle);

    nos::libuv::Timer timer(loop.get());

    timer.start(1000, 1000, [](nos::libuv::Timer &timer)
        {
            ilog("do nothing");
        });

    loop.spin();

    elog(APP_NAME " exited");

    return 0;
}

