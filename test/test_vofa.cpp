

#include <chrono>
#include <string>
#include <map>
#include <mutex>
#include <cmath>

#include "common/logger.h"
#include "common/vofa_service.h"

int main()
{
    // 先初始化日志
    slog::make_stdout_logger("test_vofa", slog::LogLevel::Trace);

    uv::Loop loop(uv::Loop::Type::Default);

    auto signal_handle = [&]([[maybe_unused]]int signum){
            loop.stop();
        };

    loop.signal(SIGINT, signal_handle);
    loop.signal(SIGTERM, signal_handle);
    loop.signal(SIGKILL, signal_handle);

    naiad::network::VofaService vofa("0.0.0.0", 9700, {1, 2, 3, 4}, 50);

    vofa.start();

    uv::Timer timer;

    timer.bind(loop);

    const int k_points = 100;
    int k1 = 0;
    int k2 = 10;
    int k3 = 50;
    int k4 = 80;

    const float k_step = (2 * M_PI) / k_points;

    //std::map<uint32_t, float> datas;

    timer.start(100, [&]{

            /*
            datas[1] = sin(k1 * k_step);
            datas[2] = sin(k2 * k_step);
            datas[3] = sin(k3 * k_step);
            datas[4] = sin(k4 * k_step);
            
            vofa.input(datas);
            */
            float datas[4];
            datas[0] = sin(k1 * k_step);
            datas[1] = sin(k2 * k_step);
            datas[2] = sin(k3 * k_step);
            datas[3] = sin(k4 * k_step);
            vofa.input(datas, 4);

            k1 ++;
            k2 ++;
            k3 ++;
            k4 ++;

            if (k1 >= 100){
                k1 = 0;
            }
            if (k2 >= 100){
                k2 = 0;
            }
            if (k3 >= 100){
                k3 = 0;
            } 
            if (k4 >= 100){
                k4 = 0;
            }                                     
        });



    loop.spin();
    vofa.stop();

    slog::error("test vofa exited");

    return 0;
}





