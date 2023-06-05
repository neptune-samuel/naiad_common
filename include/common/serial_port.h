
#ifndef __NAIAD_SERIAL_PORT_H__
#define __NAIAD_SERIAL_PORT_H__
/**
 * @file serial_port.h
 * @author Liu Chuansen (samule@neptune-robotics.com)
 * @brief 串口设备封装
 * @version 0.1
 * @date 2023-05-16
 * 
 * @copyright Copyright (c) 2023
 * 
 */
#include <termios.h>
#include <string>
#include <queue>
#include <thread>
#include <mutex>

// by default ,enable RX_NOTIFY
#ifndef SERIAL_RX_NOTIFY
#define SERIAL_RX_NOTIFY  1
#endif 

#if SERIAL_RX_NOTIFY
#include <common/uv_helper.h>
#endif 


namespace naiad 
{
namespace driver 
{

/**
 * @brief 串口收发统计
 * 
 */
struct SerialStatistics
{
    uint32_t fifo_size;
    uint32_t fifo_peak_size;
    uint64_t rx_bytes;
    uint64_t tx_bytes;
    uint64_t rx_drop_bytes;
};


class SerialPort
{
public:
    SerialPort(const std::string &device);
    ~SerialPort();

    // 禁止复制构造
    SerialPort(const SerialPort &) = delete;
    SerialPort & operator=(const SerialPort &) = delete;

    /**
     * @brief 打开串口
     * 
     * @param options  options 参数字串，
     *   格式为{rate},[5|6|7|8],[n,o,e],[1,1.5,2] 
     *   波特率为必选项。如："115200,8,n,1"
     * @return true 
     * @return false 
     */
    bool open(const char *options);

    /**
     * @brief 是否已打开
     * 
     * @return true 
     * @return false 
     */
    bool is_opened();

    /**
     * @brief 返回设备名称
     * 
     * @return const std::string& 
     */
    const std::string &name();

    /**
     * @brief 非阻塞读取数据
     * 
     * @param buf 
     * @param size 
     * @param timeout 单位为ms，如果为0，不等待，直接返回
     * @return int 
     */
    int read(void *buf, int size, int timeout = 0);

    /**
     * @brief 写入指定的数据
     * 
     * @param buf 
     * @param size 
     * @return int 
     */
    int write(const void *buf, int size);


    /**
     * @brief 启动异步读
     * 
     * @param queue_size 
     * @return true 
     * @return false 
     */
    bool async_read_start(int queue_size = 8192);


#if SERIAL_RX_NOTIFY
    /**
     * @brief 启动异步接收，并注册异步通知
     * 
     * @param uv_loop 接收通知的loop
     * @param signal_handle 异步处理函数
     * @param queue_size 队列大小
     * @return bool  
     */
    bool async_read_start(uv_loop_t *uv_loop, uv::AsyncSignal::Function signal_handle, int queue_size);

    /**
     * @brief 启动异步接收，并注册异步通知
     * 
     * @param uv_loop 接收通知的loop
     * @param signal_handle 异步处理函数
     * @param queue_size 队列大小
     * @return bool  
     */
    bool async_read_start(uv::Loop & loop, uv::AsyncSignal::Function signal_handle, int queue_size = 8192)
    {
        return async_read_start(loop.get(), signal_handle, queue_size);
    }
#endif 

    /**
     * @brief 停止异步读
     * 
     */
    void async_read_stop();

    /**
     * @brief 异步读数据
     * 
     * @param buf 
     * @param size 
     * @return int 
     */
    int async_read(void *buf, int size);

    /**
     * @brief 刷新缓存
     * 
     */
    void flush();

    /// @brief 获取fd
    /// @return 
    int get_fd();

    /// @brief 关闭端口
    void close();

    /**
     * @brief 返回一个统计信息
     * 
     * @param stats 
     */
    void get_statistics(SerialStatistics &stats);

    /**
     * @brief 检测串口参数是否合法
     * 
     * @param options 
     * @return true 
     * @return false 
     */
    static bool check_options(const char *options);

private:
    int fd_;
    std::string path_;
    std::string name_;
    // 串口的操作锁      
    std::mutex mutex_;  
    
    /// 保存默认配置，在关闭时恢复到默认值
    struct termios default_options_;

    /// 接收队列 
    std::queue<unsigned char> rx_queue_;
    /// 队列锁
    std::mutex rx_queue_mutex_;
    /// 接收线程
    std::thread rx_thread_;
    /// 接收线程是否在运行
    bool rx_thread_running_;
#if SERIAL_RX_NOTIFY
    uv::AsyncSignal rx_signal_;
#endif 

    /// 串口统计信息
    SerialStatistics statistics_;
    bool rx_queue_half_alert_ = false;
    bool rx_queue_three_quarter_alert_ = false;
    bool rx_queue_full_alert_ = false;


    int read_with_epoll(int fd, int epoll_fd, void *buf, int size, int timeout);
    int read_with_select(int fd, void *buf, int size, int timeout);

};

} // driver

} // naiad 


#endif // __NAIAD_SERIAL_PORT_H__
