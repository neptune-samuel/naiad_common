
#ifndef __NAIAD_VOFA_SERVICE_H__
#define __NAIAD_VOFA_SERVICE_H__

/**
 * @file vofa_service.h
 * @author Liu Chuansen (samule@neptune-robotics.com)
 * @brief 这个一个数据可视化插件，使用VOFA+软件展示数据流，支持浮点数 (https://www.vofa.plus)
 * @version 0.1
 * @date 2023-06-09
 * 
 * @copyright Copyright (c) 2023
 * 
 */
#include <chrono>
#include <string>
#include <map>
#include <mutex>

#include "common/logger.h"
#include "common/tcp_server.h"


namespace naiad {
namespace network {


class VofaService
{
public:

    using SharedPtr = std::shared_ptr<VofaService>;
    /**
     * @brief IPV4地址
     * 
     * @param ipv4_address 
     * @param ip_port
     * @param data_set 数据集
     * @param period_ms 周期，0 - 使用上报触发，> 0， 周期触发
     */
    VofaService(std::string const & ipv4_address, int ip_port, std::vector<uint32_t> const & data_set, int period_ms = 0)
    {
        name_ = "vofa-" + std::to_string(ip_port);

        report_period_ = period_ms;
        // 实例化一个TCP服务
        tcp_server_ = std::make_unique<naiad::network::TcpServer>(name_, ipv4_address, ip_port, 2);

        // 初始化数据
        for (auto & id : data_set)
        {
            data_cache_[id] = 0.0f;
        }

        slog::info("{}: init with {} datas, {} mode", name_, data_cache_.size(), (report_period_ > 0) ? "period" : "trigger");
    }

    /// 启动该服务
    bool start()
    {
        // 看看周期是否大于0, 如果大于，工作在定期模式
        if (report_period_ > 0)
        {
            // 绑定到LOOP
            timer_.bind(tcp_server_->get_loop());

            timer_.start(report_period_, [&]{
                    send_datas();
                });      
        }

        return tcp_server_->start();
    }

    /// @brief 返回服务是否在运行
    /// @return 
    bool is_running()
    {
        return tcp_server_->is_running();
    }

    void stop()
    {
        timer_.stop();
        tcp_server_->stop();
    }

    /**
     * @brief 输入数据格式为map<uint32_t, float>
     * 
     * @param datas 
     */
    void input(std::map<uint32_t, float> const &datas)
    {
        std::size_t count = 0;

        // 更新数据
        {
            std::lock_guard<std::mutex> lock(data_mutex_);

            for (auto & kv : datas){
                if (data_cache_.count(kv.first) > 0){
                    count ++;
                    data_cache_[kv.first] = kv.second;
                }
            }
        }

        // 数据插入驱动时，只要有一个数据就上报一组数据
        if ((report_period_ <= 0) && (count > 0))
        {
            send_datas();
        }
    }

    /// @brief 返回地址
    /// @return 
    std::string const & get_address()
    {
        return tcp_server_->get_address();
    }

    /// @brief 返回TCP的端口
    /// @return 
    int get_port()
    {
        return tcp_server_->get_port();
    }

    /**
     * @brief 按顺序输入数据，不用带索引
     * 
     * @param datas 
     * @param num 
     */
    void input(float const * datas, int num)
    {
        // 更新数据
        {
            int i = 0;
            std::lock_guard<std::mutex> lock(data_mutex_);
            for (auto & kv : data_cache_)
            {
                if (i < num)
                {
                    data_cache_[kv.first] = datas[i];
                }
                else 
                {
                    break;
                }

                i ++;
            }
        }

        // 数据插入驱动时，只要有一个数据就上报一组数据
        if ((report_period_ <= 0) && (num > 0))
        {
            send_datas();
        }        
    }

private:
    /// 服务名称
    std::string name_;    
    /// 周期
    int report_period_;
    // 指向一个TCP服务
    std::unique_ptr<naiad::network::TcpServer> tcp_server_;

    /// 数据集锁
    std::mutex data_mutex_;

    // 数据缓存
    std::map<uint32_t, float> data_cache_;

    // 创建一个定时器
    uv::Timer timer_;


    void send_datas()
    {
        if (tcp_server_->is_running() && tcp_server_->connections_num() > 0)
        {

            // VOFA使用浮点数据发送
            // 数据格式为
            // [F0][F1][F2][F3]...[END]
            // 其中 [END] 为 0x00, 0x00, 0x80, 0x7f
            float buf[32]; // 最多32个通道，4*8 = 32个字节
            std::size_t num = 0;

            {
                // 转换为字节流
                std::lock_guard<std::mutex> lock(data_mutex_); 
                for (auto & v: data_cache_)
                {
                    if (num > (sizeof(buf)/sizeof(buf[0]) - 1))
                    {
                        break;
                    }

                    buf[num ++] = v.second;
                }

                uint8_t *end = (uint8_t *)&buf[num];

                end[0] = 0x00;
                end[1] = 0x00;
                end[2] = 0x80;
                end[3] = 0x7f;

                num ++;
            }

            // 发送报文
            tcp_server_->send(tcp_server_->AllClients, (uint8_t *)&buf[0], num * sizeof(float));
        }
    }

};

}
}


#endif // __NAIAD_VOFA_SERVICE_H__

