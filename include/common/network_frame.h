
#ifndef __NOS_NETWORK_FRAME_H__
#define __NOS_NETWORK_FRAME_H__

/**
 * @file network_frame.h
 * @author Liu Chuansen (179712066@qq.com)
 * @brief 一个网络数据帧的实例
 * @version 0.1
 * @date 2023-05-22
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#include <string>
#include <cstring>

#include <common/network_client.h>
#include <common/sys_time.h>
//#include <common/logger.h>

namespace nos
{

// 放置于网络的命名空间
namespace network
{


class DataFrame
{

public:
    explicit DataFrame(int size): host_({"", 0}), size_(size) , time_stamp_(0)
    { 
        //slog::trace("--> DataFrame(size ={})", size_);

        if (size_ > 0)
        {
            data_ = new uint8_t [size_];
            ::memset(data_, 0, size_);
        }
        else 
        {
            data_ = nullptr;
        }
    }

    DataFrame(Host const &host, int size) : DataFrame(size) {
        //slog::trace("--> DataFrame(Host const &host, int size)");
        host_ = host;
     }

    DataFrame(Host const &host, uint8_t const *data, int size) : DataFrame(host, size)
    {
        //("--> DataFrame(Host const &host, uint8_t *data, int size)");

        // 使用一个时间戳来标记一个帧, 仅在数据创建时给值
        time_stamp_ = nos::system::uptime();

        // 初始化数据
        ::memcpy(data_, data, size);
    }

    // 复制函数
    DataFrame(DataFrame const & other)
    {
        //slog::trace("--> DataFrame() copy construct");

        host_ = other.host_;
        if (other.size_ > 0)
        {
            size_ = other.size_;
            data_ = new uint8_t[size_];
            time_stamp_ = other.time_stamp_;
    
            if (other.data_)
            {
                ::memcpy(data_, other.data_, size_);
            }
            else 
            {   
                ::memset(data_, 0, size_);
            }
        }
        else 
        {
            size_ = 0;
            data_ = nullptr;
            time_stamp_ = 0;
        }
    }

    DataFrame(DataFrame && other)
    {
        //slog::trace("--> DataFrame() move construct");        
        host_ = other.host_;
        data_ = other.data_;
        size_ = other.size_;
        time_stamp_ = other.time_stamp_;

        // 将它清空
        other.data_ = nullptr;
        other.size_ = 0;
        other.time_stamp_ = 0;
    }

    /**
     * @brief 赋值函数，只存在于两个已创建的变量
     * 
     * @param other
     * @return DataFrame& 
     */
    DataFrame & operator=(DataFrame const & other)
    {
        if (this != &other)
        {
            // 如果当前数据为非空，需要先删除
            if (data_ != nullptr)
            {
                delete [] data_;
                data_ = nullptr;
            }

            size_ = other.size_;
            time_stamp_ = 0;
            // 如果对端有数据，则需要复制过来
            if (size_ > 0)
            {
                data_ = new uint8_t [size_];

                if (other.data_)
                {
                    ::memcpy(data_, other.data_, size_);
                    time_stamp_ = other.time_stamp_;
                }
                else 
                {   
                    ::memset(data_, 0, size_);
                }
            }
            else 
            {
                size_ = 0;                
            }
        }

        return *this;
    }

    /**
     * @brief 赋值函数，只存在于两个已创建的变量
     * 
     * @param other
     * @return DataFrame& 
     */
    DataFrame & operator=(DataFrame && other)
    {
        if (this != &other)
        {
            host_ = other.host_;
            data_ = other.data_;
            size_ = other.size_;
            time_stamp_ = other.time_stamp_;

            // 将它清空
            other.data_ = nullptr;
            other.size_ = 0;
            other.time_stamp_ = 0;
        }

        return *this;
    }


    ~DataFrame()
    {
        //slog::trace("--> DataFrame() destruct, size={}, {}", size_,  data_ ? "with data" : "no data");   
        if (data_)
        {
            delete [] data_;
        }
    }

    /**
     * @brief 获取主机信息
     * 
     * @return Host& 
     */
    Host const & get_host() const
    {
        return host_;
    }

    /**
     * @brief 某些常量帧时，需要返回常量的数据指针
     * 
     * @return const uint8_t* 
     */
    const uint8_t * data_pointer() const 
    {
        return data_;
    }

    /**
     * @brief 只允许取值，不允许修改
     * 
     * @param index 
     * @return uint8_t & 
     */
    uint8_t & operator [](int index) 
    {
        if (index < 0 || index >= size_)
        {
            static uint8_t dummy = 0;
            return dummy;
        }
        
        return data_[index];
    }

    /**
     * @brief 返回帧数据大小
     * 
     * @return int 
     */
    int size() const
    {
        return size_;
    }

    /**
     * @brief 返回一个32位的帧ID
     * 
     * @return uint32_t 
     */
    uint32_t id() const 
    {
        return static_cast<uint32_t>(time_stamp_);
    } 

    /**
     * @brief 返回数据指针， 不能修改指针的值
     * 
     * @return uint8_t *const  
     */
    // uint8_t * const data() 
    // {
    //     return data_;
    // }

    /**
     * @brief 是否为空
     * 
     * @return true 
     * @return false 
     */
    bool is_empty() const 
    {
        return ((data_ == nullptr) || (size_ <= 0));
    }

private:
    Host host_;
    uint8_t *data_;
    int size_;
    int64_t time_stamp_;

};


} // end network

} // end nos


#endif // __NOS_NETWORK_FRAME_H__

