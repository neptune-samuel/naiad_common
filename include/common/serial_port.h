
#ifndef __NOS_SERIAL_PORT_H__
#define __NOS_SERIAL_PORT_H__
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


namespace nos 
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

private:
    int fd_;
    std::string path_;
    std::string name_;        
    
    /// 保存默认配置，在关闭时恢复到默认值
    struct termios default_options_;

    /// 接收队列 
    std::queue<unsigned char> rx_queue_;
    /// 接收线程
    std::thread rx_thread_;
    /// 接收线程是否在运行
    bool rx_thread_running_;
    /// 串口统计信息
    SerialStatistics statistics_;

};

} // driver

} // nos 


#endif // __NOS_SERIAL_PORT_H__
