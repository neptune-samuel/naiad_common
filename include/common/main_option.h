


#ifndef __MAIN_OPTIONS_H__
#define __MAIN_OPTIONS_H__

/**
 * @file main_options.h
 * @author Liu Chuansen (samule@neptune-robotics.com)
 * @brief 提供一个解析命令的选项
 * @version 0.1
 * @date 2023-05-19
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#include <string>
#include <iostream>
#include <map>
#include <docopt/docopt.h>

namespace nos 
{


/**
 * @brief 一个简单的主函数选项解析器基类
 * 
 */
class MainOption
{
public:
    MainOption(const std::string &usage, const std::string &version, int argc, const char *argv[])
    {
        args_ = docopt::docopt(usage, {argv + 1, argv + argc}, true, version);
    }

    ~MainOption() { }

    /**
     * @brief 检查所有的选项是否准确
     * 
     * @return true 
     * @return false 
     */
    virtual bool check()
    {
        return true;
    }

    /**
     * @brief 显示所有选项
     * 
     */
    virtual void dump()
    {
        for(auto const& arg : args_) 
        {
            std::cout << "args[" << arg.first << "] = " << arg.second << std::endl;
        }
    }

    /**
     * @brief 返回一个bool的参数
     * 
     * @param option 
     * @return true 
     * @return false 
     */
    bool get_bool(const std::string &option)
    {
        return args_[option].asBool();
    }

    /**
     * @brief 返回一个整数参数
     * 
     * @param option 
     * @return int 
     */
    int get_int(const std::string &option)
    {
        return static_cast<int>(args_[option].asLong());
    }

    /**
     * @brief 返回字串参数
     * 
     * @param option 
     * @return const std::string& 
     */
    const std::string &get_string(const std::string &option)
    {
        return args_[option].asString();
    }

protected:
    std::map<std::string, docopt::value> args_;
};


} // end nos

#endif // __MAIN_OPTIONS_H__
