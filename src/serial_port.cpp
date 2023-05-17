

#include <fcntl.h>
#include <termios.h>
#include <unistd.h>
#include <errno.h>
#include <sys/epoll.h>

#include <string>
#include <thread>
#include <queue>

#include <common/logger.h>
#include <common/serial_port.h>
#include <common/sys_time.h>

#define ASYNC_READ_WITH_EPOLL  1


namespace nos 
{

namespace driver 
{


/*
struct termios 是 Linux 下串口编程中的一个结构体，用于描述终端的属性。它包含以下成员：
1. tcflag_t c_iflag：输入模式标志，用于设置终端输入模式。
2. tcflag_t c_oflag：输出模式标志，用于设置终端输出模式。
3. tcflag_t c_cflag：控制模式标志，用于设置终端控制模式。
4. tcflag_t c_lflag：本地模式标志，用于设置终端本地模式。
5. cc_t c_cc[NCCS]：控制字符数组，用于设置终端控制字符。
其中，c_iflag、c_oflag、c_cflag 和 c_lflag 都是位标志，用于设置终端的各种属性。cc_t 类型是一个字符类型，c_cc 数组中存储了一些特殊字符，如终端输入字符的起始字符、结束字符、删除字符等等。
具体来说，这些成员的含义如下：

1. c_iflag：输入模式标志。包含以下标志：
 IGNBRK：忽略 BREAK 信号。
 BRKINT：将 BREAK 信号转换为中断信号，并且在输入队列中丢弃输入数据。
 IGNPAR：忽略奇偶校验错误的输入字节。
 PARMRK：在输入队列中添加奇偶校验错误的标记。
 INPCK：启用奇偶校验。
 ISTRIP：去除输入字节的第 8 位。
 INLCR：将输入的 NL（换行符）转换为 CR（回车符）。
 IGNCR：忽略输入的 CR（回车符）。
 ICRNL：将输入的 CR（回车符）转换为 NL（换行符）。
 IXON：启用输出流控制。
 IXOFF：启用输入流控制。

2. c_oflag：输出模式标志。包含以下标志：
 OPOST：启用输出处理。
 ONLCR：将输出的 NL（换行符）转换为 CR（回车符）和 NL（换行符）。

3. c_cflag：控制模式标志。包含以下标志：
 CSIZE：字符大小掩码。
 PARENB：启用奇偶校验。
 PARODD：启用奇偶校验并使用奇校验。
 CSTOPB：使用两个停止位。
 CRTSCTS：启用硬件流控制。

4. c_lflag：本地模式标志。包含以下标志：
 ICANON：启用规范模式。
 ECHO：启用回显。
 ECHOE：启用 ERASE 字符。
 ISIG：启用信号。
 IEXTEN：启用扩展输入处理。

5. c_cc[NCCS]：控制字符数组。包含以下控制字符：
 VINTR：中断字符。
 VQUIT：退出字符。
 VERASE：擦除字符。
 VKILL：删除字符。
 VEOF：文件结束字符。
 VTIME：非规范模式读取时的超时时间。
 VMIN：非规范模式读取时的最小字符数。
 VSWTC：启用字符控制。
 VSTART：输出流控制字符。
 VSTOP：输出流控制字符。
 VSUSP：挂起字符。
 VEOL：行结束字符。
 VREPRINT：重新打印字符。
 VDISCARD：丢弃字符。
 VWERASE：字擦除字符。
 VLNEXT：下一个字符。
 VEOL2：第二行结束字符。
*/



static int to_sys_baudrate(int baudrate)
{

#define __case(x)  case x: return B##x

    switch(baudrate)
    {
        __case(50);
        __case(75);
        __case(110);
        __case(134);
        __case(150);
        __case(200);
        __case(300);
        __case(600);
        __case(1200);
        __case(1800);
        __case(2400);
        __case(4800);
        __case(9600);
        __case(19200);
        __case(38400);
        __case(57600);
        __case(115200);
        __case(230400);
        __case(460800);
        __case(500000);
        __case(576000);
        __case(921600);
        __case(1000000);
        __case(1152000);
        __case(1500000);
        __case(2000000);
        __case(2500000);
        __case(3000000);
        __case(3500000);
        __case(4000000); 
        default:
            return 0;
    }
    return 0;
}

#if ASYNC_READ_WITH_EPOLL

static int read_with_epoll(int fd, int epoll_fd, void *buf, int size, int timeout)
{
    if (timeout <= 0)
    {
        return read(fd, buf, size);
    }
    else 
    {     
        #define MAX_EVENTS 10        
        struct epoll_event ev, events[MAX_EVENTS];
        int64_t expired = nos::system::uptime() + timeout;
        int offset = 0;

        while ((offset < size) && (timeout > 0))
        {
            int nfds = epoll_wait(epoll_fd, events, MAX_EVENTS, timeout);
            if (nfds < 0)
            {
                wlog("epoll wait() error");
                return -1;
            }

            for (int i = 0; i < nfds; ++ i)
            {
                if (events[i].data.fd == fd)
                {
                    int ret = ::read(events[i].data.fd, (unsigned char *)buf + offset, size - offset);
                    if (ret < 0)
                    {
                        return ret;
                    }
                    else if (ret > 0)
                    {
                        offset += ret;
                    }
                }                
            }

            // 
            timeout = expired - nos::system::uptime();
        }

        return offset;
    }
}

#endif // ASYNC_READ_WITH_EPOLL

/**
 * @brief 使用串口接收，可以设定等待时间
 * 
 * @param fd 
 * @param buf 
 * @param size 
 * @param timeout ms
 * @return int 
 */
static int read_wiih_select(int fd, void *buf, int size, int timeout)
{
    if (timeout <= 0)
    {
        return read(fd, buf, size);
    }
    else 
    {      
        struct timeval tv;
        int offset = 0;
        int ret = 0;
        fd_set rfds;

        tv.tv_sec = timeout / 1000;
        tv.tv_usec = (timeout % 1000) * 1000;

        while ((tv.tv_sec > 0 || tv.tv_usec > 0) && (offset < size))
        {
            FD_ZERO(&rfds);
            FD_SET(fd, &rfds);

            ret = select(fd + 1, &rfds, NULL, NULL, &tv);

            if (ret > 0)
            {
                ssize_t len = ::read(fd, (unsigned char*)buf + offset, size - offset);

                if (len > 0)
                {
                    offset += len;
                }
                else if (len < 0) 
                {
                    // 读失败，有可能串口设备文件出错？
                    return len;
                }
            }
            else if (ret == 0)
            {
                // timeout                
                break;
            }
            else 
            {
                if (errno == EINTR)
                {
                    continue;
                }
                break;
            }
        }
        return offset;
    }
}


/// 一个串口设置示例
struct SerialSetting
{
    int baudrate;
    int data_bits;
    int stop_bits;
    bool parity_odd;
    bool parity_even;
};


/**
 * @brief 解析参数
 * 
 * @param options 
 * @param opts 
 * @return true 
 * @return false 
 */
static bool parse_options(const char *options, SerialSetting &set)
{
    if (options == nullptr)
    {
        return false;
    }

    set.baudrate = B115200;
    set.data_bits = 8;
    set.stop_bits = 1;
    set.parity_odd = false;
    set.parity_even = false;

    {
        std::unique_ptr<char []> ptr(strdup(options));        
        char *p = ptr.get();
        char *end;
        //保存结果指针
        const char *result[4];
        int index = 0;

        // 必须要清空，允许不完整输入
        memset(result, 0, sizeof(result));

        do 
        {
            result[index ++] = p;

            end = strchr(p, ',');

            // 如果找到，说明还有下一个
            if (end)
            {
                *end = '\0';
                p = end + 1;
            }
        }while ((end != NULL) && (index < sizeof(result)/sizeof(result[0])));

        //解析
        if (result[0])
        {
            set.baudrate = to_sys_baudrate(strtoul(result[0], NULL, 10));

            if (set.baudrate == 0)
            {
                wlog("invalid serial baudrate: {}", result[0]);
                return false;
            }
        }

        if (result[1])
        {
            set.data_bits = strtoul(result[1], NULL, 10);

            if ((set.data_bits != 5) && (set.data_bits != 6)
                && (set.data_bits != 7) && (set.data_bits != 8))
            {
                wlog("invalid serial data-bits: {}", result[1]);
                return false;                
            }
        }

        if (result[2])
        {
            if (!strcmp(result[2], "o"))
            {
                set.parity_odd = true;
            }
            else if (!strcmp(result[2], "e"))
            {
                set.parity_even = true;
            }
            else if (!strcmp(result[2], "n"))
            {
                set.parity_even = false;
                set.parity_odd = false;
            }
            else 
            {
                wlog("invalid serial parity option: {}", result[2]);
                return false;
            }
        }

        if (result[3])
        {
            set.stop_bits = strtoul(result[3], NULL, 10);

            if ((set.stop_bits != 1) && (set.stop_bits != 2))
            {
                wlog("invalid serial stop-bits: {}", result[3]);
                return false;
            }
        }

    }

    return true;
}



SerialPort::SerialPort(const std::string &device) : fd_(-1), path_(device)
{
    // set name
    auto pos = path_.find("/dev/");
    if (pos == 0)
    {
        name_ = path_.substr(pos + 5);
    }
    else 
    {
        name_ = path_;
    }

    statistics_ = { 0 };
}


SerialPort::~SerialPort()
{
    // 关闭端口
    if (is_opened())
    {
        close();
    }
}


const std::string & SerialPort::name()
{
    return name_;
}


bool SerialPort::open(const char *options)
{
    SerialSetting cfg = { 0 };

    if (is_opened())
    {
        wlog("port({}) is already opened", name_);
        return false;
    }

    if (!parse_options(options, cfg))
    {
        return false;
    }

    fd_ = ::open(path_.c_str(), O_NONBLOCK | O_RDWR | O_NOCTTY);
    if (fd_ == -1)
    {
        wlog("open({}) failed: {}", path_, strerror(errno));
        return false;
    }

    // 备份参数
    if (tcgetattr(fd_, &default_options_) != 0)
    {
        wlog("tcgeattr() failed: {}", strerror(errno));

        ::close(fd_);
        fd_ = -1;
        return false;
    }

    struct termios set;
    memcpy(&set, &default_options_, sizeof(set));

    set.c_iflag = 0; //ICRNL | IXON | IXOFF*/; // do not use XON XOFF CTRL - S and CTRL - Q   
    set.c_oflag &= 0;//~(ONLCR | OCRNL); //~(ONLCR | OCRNL)
    set.c_cflag = CREAD | CLOCAL; 

    // 数据位 
    switch(cfg.data_bits)
    {
        case 5:
            set.c_cflag |= CS5;
            break;
        case 6:
            set.c_cflag |= CS6;
            break;
        case 7:
            set.c_cflag |= CS7;
            break;
        default:
            set.c_cflag |= CS8;
            break;
    }

    //校验位
    if (cfg.parity_odd)
    {
        set.c_cflag |= PARENB;
        set.c_cflag |= PARODD;
    }
    else if (cfg.parity_even)
    {
        set.c_cflag |= PARENB;
    }

    //停止位,默认是1个停止位，
    // 如何支持1.5个停止位？
    if (cfg.stop_bits == 2)
    {
        set.c_cflag |= CSTOPB;
    }

    cfsetispeed(&set, cfg.baudrate);
    cfsetospeed(&set, cfg.baudrate);

    // 其他控制位
    set.c_lflag &= ~ICANON;    
    set.c_lflag &= ~ECHO;    
    set.c_lflag &= ~ISIG;

    set.c_cc[VMIN] = 1;    
    set.c_cc[VTIME] = 0;    

    if (tcsetattr(fd_, TCSANOW, &set) != 0)
    {
        wlog("tcsetattr() failed: {}", strerror(errno));
        ::close(fd_);
        fd_ = -1;
        return false;
    }

    dlog("serial({}) open success", name_);

    return true;
}


int SerialPort::get_fd()
{
    return fd_;
}


bool SerialPort::is_opened()
{
    return (fd_ != -1);
}


void SerialPort::close()
{
    // 先停止接收线程
    async_read_stop();

    if (fd_ != -1)
    {
        // 尝试恢复到默认
        if (tcsetattr(fd_, TCSANOW, &default_options_) != 0)
        {
            wlog("tcsetattr() failed: {}", strerror(errno));
        }

        ::close(fd_);
        fd_ = -1;

        dlog("serial({}) close", name_);
    }
}

void SerialPort::get_statistics(SerialStatistics &stats)
{
    stats = statistics_;
}


void SerialPort::flush()
{
    if (fd_ != -1)
    {
        tcflush(fd_, TCIOFLUSH);
    }
}


int SerialPort::read(void *buf, int size, int timeout)
{
    if (fd_ < 0)
    {
        return -1;
    }

    int rx_size = read_wiih_select(fd_, buf, size, timeout);

    if (rx_size > 0)
    {
        statistics_.rx_bytes += rx_size;

        trace("serial({}) read: {:X}", name_, spdlog::to_hex((const unsigned char *)buf, (const unsigned char *)buf + rx_size, 16));
    }

    return rx_size;
}

int SerialPort::write(const void *buf, int size)
{
    if (fd_ < 0)
    {
        return -1;
    }

    trace("serial({}) write: {:X}", name_, spdlog::to_hex((const unsigned char *)buf, (const unsigned char *)buf + size, 16));

    int ret;

    int offset = 0;
    int retry = 10;
    do 
    {

        ret = ::write(fd_, (unsigned char*)buf + offset, size - offset);  

        if (ret > 0)
        {
            offset += ret;
            retry --;
        }

        if ((ret < 0) && (errno == EINTR))
        {
            // try again 
            retry --;
            ret = 0;
        }

    } while( (offset < size) && (ret >= 0) && (retry > 0));

    if (ret < 0)
    {
        return ret;
    }

    if (offset != size)
    {
        wlog("serial({}) write {} bytes, but expect {} bytes", name_, offset, size);
    }

    statistics_.tx_bytes += offset;

    return offset;
}


/**
 * @brief 启动异步读
 * 
 * @param queue_size 
 * @return true 
 * @return false 
 */
bool SerialPort::async_read_start(int queue_size)
{
    if (fd_ < 0)
    {
        return false;
    }

    if (rx_thread_running_)
    {
        wlog("serial({}) aync-read is running", name_);
        return false;
    }

    // 清除fifo数据
    decltype(rx_queue_)().swap(rx_queue_);

    // 设定fifo大小
    statistics_.fifo_size = queue_size;

    rx_thread_ = std::thread([this]() {

        this->rx_thread_running_ = true;
        uint8_t buf[1024];

        #if ASYNC_READ_WITH_EPOLL
        int epoll_fd = epoll_create1(0);
        if (epoll_fd < 0)
        {
            elog("epoll_create1() failed");
            return ;
        }

        struct epoll_event ev;
        ev.events = EPOLLIN;
        ev.data.fd = fd_;

        if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, fd_, &ev) < 0)
        {
            elog("epoll_ctl() failed");

            ::close(epoll_fd);
            return ;
        }

        #endif // ASYNC_READ_WITH_EPOLL

        while(this->rx_thread_running_)
        {            
            // 将数据放入队列中
            #if ASYNC_READ_WITH_EPOLL
            int rx_size = read_with_epoll(fd_, epoll_fd, buf, sizeof(buf), 10);
            #else 
            int rx_size = read_with_select(fd_, buf, sizeof(buf), 10);
            #endif 
            if (rx_size > 0)
            {                
                statistics_.rx_bytes += rx_size;

                // 将数据放放队列中
                // 如果接收队列的数据小于设定最大小
                if (this->rx_queue_.size() < this->statistics_.fifo_size)
                {
                    // 将这一包数据放在队列，多一点没有关系
                    for (int i = 0; i < rx_size ; ++i)
                    {
                        this->rx_queue_.push(buf[i]);
                    }

                    // 计算峰值 
                    if (this->rx_queue_.size() > this->statistics_.fifo_peak_size)
                    {                        
                        dlog("serial({}) rx fifo peak rise: {} -> {}", this->name_, this->statistics_.fifo_peak_size, this->rx_queue_.size());
                        this->statistics_.fifo_peak_size = this->rx_queue_.size();
                    }
                }
                else 
                {
                    statistics_.rx_drop_bytes += rx_size;
                    dlog("serial({}) fifo full, drop {} bytes", this->name_, rx_size);                    
                }

                trace("serial({}) read: {:X}", name_, spdlog::to_hex((const unsigned char *)buf, (const unsigned char *)buf + rx_size, 16));
            } 
            else 
            {
                if (rx_size < 0) 
                {
                    wlog("serial({}) rx failed,ret={}", this->name_, rx_size);
                }
                
            }           

            //std::this_thread::sleep_for(std::chrono::milliseconds(1000));
        }

        #if ASYNC_READ_WITH_EPOLL
        ::close(epoll_fd);
        #endif 
    });

    return true;
}

/**
 * @brief 停止异步读
 * 
 */
void SerialPort::async_read_stop()
{
    if (rx_thread_running_)
    {
        rx_thread_running_ = false;
        // wait for end
        rx_thread_.join();
    }
}

/**
 * @brief 异步读数据
 * 
 * @param buf 
 * @param size 
 * @return int 
 */
int SerialPort::async_read(void *buf, int size)
{
    int rx_size = 0;

    unsigned char *data = (unsigned char *)buf;

    while(!rx_queue_.empty() && (rx_size < size))
    {
        data[rx_size] = rx_queue_.front();
        rx_queue_.pop();

        ++ rx_size;
    }

    return rx_size;
}


} // driver

} // nos 
