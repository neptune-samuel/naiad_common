#ifndef __CXXARGS_H__
#define __CXXARGS_H__

/**
 * @file cxxargs.h
 * @author Liu Chuansen (179712066@qq.com)
 * @brief 一个简单的主程序参数解析类实现
 * @version 0.1
 * @date 2023-06-10
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#include <iostream>
#include <iomanip>
#include <string>
#include <map>
#include <set>
#include <vector>
#include <functional>


namespace cxxargs
{


namespace helper
{
    /// @brief 去除前面的空字符
    /// @param str 
    /// @return 
    std::string trim(const std::string& str) 
    {
        // 查找第一个不是空格、TAB、换行符的位置
        size_t start = str.find_first_not_of(" \t\n\r");
        // 如果字符串中没有非空白字符，则返回空字符串
        if (start == std::string::npos) {
            return "";
        }
        // 查找最后一个不是空格、TAB、换行符的位置
        size_t end = str.find_last_not_of(" \t\n\r");
        // 返回去掉前后空格、TAB、换行符的子串
        return str.substr(start, end - start + 1);
    }

    
    /// @brief 字串分割
    /// @param str 
    /// @param delimiter 
    /// @return 
    std::vector<std::string> split(const std::string& str, char delimiter) 
    {
        std::vector<std::string> tokens;
        size_t start = 0, end = 0;
        while ((end = str.find(delimiter, start)) != std::string::npos) {
            tokens.push_back(str.substr(start, end - start));
            start = end + 1;
        }
        tokens.push_back(str.substr(start));
        return tokens;
    }

    /// @brief 是否以指定字串结尾
    /// @param input 
    /// @param mark 
    /// @return 
    bool endswith(std::string const & input, std::string const &mark) 
    {
        if (input.length() < mark.length()) {
            return false;
        }
        return input.substr(input.length() - mark.length()) == mark;
    }
}


// 定义一个参数检查函数
typedef std::function<bool(const std::string &, std::string &msg)> OptionCheck;


class Option
{
public:
    enum class Mode : int 
    {
        Invalid, // 无效的
        None,    // no args
        Single,  // one args
        Multi    // multi args
    };

    //Option() { }

    Option(std::string const & mark, std::string const &desc, std::string const & def_value, OptionCheck check)
    {
        // no arg: -h,--help 
        // one args: -l,--log-level level
        // multi args: -f,--files file...
        origin = helper::trim(mark);
        tip = helper::trim(desc);
        default_value = def_value;
        check_function = check;

        if (default_value.size()){
            tip += " (default:" + default_value + ")";
        }

        auto check_key = [](const std::string & key) -> bool {
            return ((key.size() > 0) && (key[0] == '-'));
        };

        mode = Mode::Invalid;
        
        size_t pos = origin.find(' ');
        if (pos != origin.npos){
            // 有参数版本
            std::string s1 = origin.substr(0, pos);
            std::string s2 = origin.substr(pos + 1);

            if (check_key(s1)){
                // 将s1分解为keys,
                keys = helper::split(s1, ',');

                std::string args = helper::trim(s2);
                if (args.size() > 0){
                    mode = Mode::Single;
                    // 如果以...结尾，表示多个参数模式
                    if (helper::endswith(args, "...")){
                        mode = Mode::Multi;
                    }
                }
            }
        } else if (check_key(origin)){
            mode = Mode::None;
            keys = helper::split(origin, ',');
        }              

        if (mode == Mode::Invalid){
            std::cout << "***Invalid option:" << origin << std::endl;
        }
    }

    bool valid() const 
    {
        return !values.empty();
    }

    bool as_bool() const 
    {
        // 只要有值，就返回true
        return !values.empty();
    }

    double as_number() const 
    {
        // 将第一个值转换为浮点数
        // 如果出错，可能会抛出异常
        if (!values.empty()) {
            return std::stod(values[0]);
        }
        else if (!default_value.empty())
        {
            return std::stod(default_value);
        }
        return 0.0f;
    }

    std::string as_string() const 
    {
        if (!values.empty()) {
            return values[0];
        }
        return default_value;
    }

    std::vector<std::string> const & as_string_list() const
    {
        return values;
    }

private:
    Mode mode;
    std::string origin;
    std::string tip;
    std::vector<std::string> keys;
    std::vector<std::string> values; 
    std::string default_value;// 默认值  
    OptionCheck check_function = nullptr; 

    void dump() const 
    {
        // [-k,--key](%s)     :{'','','',''} 
        std::cout << "[";
        for (std::size_t i = 0; i < keys.size(); i ++){
            if (i > 0) std::cout << ",";
            std::cout << keys[i];
        }
        std::cout << "](" << default_value << "): {";
        for (std::size_t i = 0; i < values.size(); i ++){
            if (i > 0) std::cout << ",";
            std::cout << "'" << values[i] << "'";
        }
        std::cout << "}" << std::endl;
    }


    /**
     * @brief 是否匹配这个KEY
     * 
     * @param key 
     * @return bool  
     */
    bool is_matched(std::string const &key) const 
    {
        for (auto &k : keys){
            if (k == key){
                return true;
            }
        }
        return false;
    }

    // 校验参数
    bool check(const std::string &arg, std::string & msg)
    {
        if (check_function){
            return check_function(arg, msg);
        }
        // 默认为真
        return true;
    }

    // 让Parser直接可以访问它的所有成员
    template <typename T> friend class Parser;
};



template <typename OptionIdType>
class Parser
{
public:

    /// @brief 参数解析器
    /// @param app_name 指定程序名称
    explicit Parser(const char *app_name) : app_name_(app_name)
    {

    }    


    /// @brief 添加一个选项
    /// @param opt_id 选项ID
    /// @param mark 选项标记，如-h
    /// @param desc 选项描述
    /// @param default_value 默认值 
    /// @param check_function 选项值检查函数
    /// @return 
    Parser & option(OptionIdType opt_id, 
        std::string const & mark, std::string const &desc, 
        std::string const & default_value = "", OptionCheck check_function = nullptr)
    {
        Option opt(mark, desc, default_value, check_function);
        if (opt.mode == Option::Mode::Invalid)
        {
            exit(1);
        }

        // 插入到数据中
        options_.insert(std::make_pair(opt_id, opt));
        return *this;
    }

    /// @brief 不想指定默认值，但要检查函数
    /// @param opt_id 选项ID
    /// @param mark 选项标签
    /// @param desc 选项描述信息
    /// @param check_function 检查函数
    /// @return 
    Parser & option(OptionIdType opt_id, 
        std::string const & mark, std::string const &desc, 
        OptionCheck check_function)
    {
        return option(opt_id, mark, desc, "", check_function);
    }

    /// @brief 设置某个选项ID为帮助，它将在检测到这个选项时，打印帮助后退出
    /// @param opt_id 
    /// @return 
    Parser & set_help(OptionIdType opt_id)
    {
        help_id_ = opt_id;
        has_help_id_ = true;
        return *this;
    }

    /// @brief 解析主函数的参数
    /// @param argc 
    /// @param argv 
    void parse(int argc, const char *argv[])
    {
        // 从第2个参数开始
        // 如果参数不在存在 -- 程序退出，显示不支持的选项
        // 参数存在 
        //   无值参数  true -> values, 如果无值参数后面有值 -> unmatched_args
        //   单值参数  如果无值 -> exit, 有单值  -> values, 有多值 -> unmatched_args 
        //   多值参数  如果无值 -> exit, 有值 -> values 
        //   
        // 其他约束：
        // 无值参数  允许出现多次，可以使用count()返回出现的次数
        // 单值参数 允许出现多次. 以最后一次为准
        // 多值参数，允许多次出现，返回列表
        Option * opt_ptr = nullptr;
        // 在opt_ptr 设值时清0，每次添加一个参数时，它加1
        std::size_t opt_args_num = 0;

        for (int i = 1; i < argc; i ++){
            std::string arg = argv[i];
            if (arg[0] == '-'){
                // 保存值
                // 出现参数时：
                // 1.首先要判断上一个参数是否已正常结束：
                //    条件是，单值参数或多值参数，如果未设定过参数，提示错误并退出
                if (opt_ptr){
                    if (opt_args_num == 0){
                        std::cout << "***Option('" << argv[i - 1] << "'): No argument" << std::endl;
                        std::exit(1);
                    }
                }

                // 2.查找是否已定义的参数，如果没有，退出 
                //     找到 设定opt_ptr
                //       如果是无值参数，入值true， 并清空
                //       如果是单值或多值参数，不做什么事情 
                opt_ptr = nullptr;
                for (auto & kv : options_){
                    auto & opt = kv.second;
                    if (opt.is_matched(arg)){
                        opt_ptr = &opt;
                        opt_args_num = 0;
                    }
                }    
                // 如果没有找到程序退出 
                if (opt_ptr == nullptr){
                    std::cout << "***Unknown option '" << arg << "'" << std::endl;
                    std::exit(1);
                } 

                // 如果是无值参数，入一个值true, 然后清空
                if (opt_ptr) {
                    if (opt_ptr->mode == Option::Mode::None){
                        opt_ptr->values.push_back("1");// 方便as_integer
                        opt_ptr = nullptr;
                        opt_args_num = 0;
                    }else {
                        // 如果是有值参数，但是最后一参数了，提示错误
                        if (i == argc - 1){
                            std::cout << "***Option('" << arg << "'): No argument" << std::endl;
                            std::exit(1);
                        }
                    }
                }

            } else  {
                // 这里有两种情况
                // 不在key-range 时，保存到unmatched_args
                // 在key-range时： 
                //                 如果是单值参数，保存值，重置指针
                //                 如果是多值参数，保存值
                // 在key-range的条件：
                //  单值参数 或多值参数
                if (opt_ptr){

                    // 有值参数，使用用户定义的方法检查是否合规
                    std::string msg;
                    if (!opt_ptr->check(arg, msg)){
                        // 检查失败
                        if (msg.empty())
                        {
                            msg = "Invalid argument(" + arg + ")";
                        }
                        std::cout << "***Option('" << opt_ptr->keys[0] << "'): " << msg << std::endl;
                        std::exit(1); 
                    }

                    if (opt_ptr->mode == Option::Mode::Single){
                        // 单值参数，每次将值放到前面
                        opt_ptr->values.insert(opt_ptr->values.begin(), arg);
                        // 单值参数仅接收一个值
                        opt_ptr = nullptr;
                        opt_args_num = 0;
                    } else if (opt_ptr->mode == Option::Mode::Multi){
                        opt_ptr->values.push_back(arg);
                        opt_args_num ++;
                    }
                } else {
                    unmatched_args_.push_back(arg);
                }
            }
        }

        // 是否需要打印帮助
        if (has_help_id_ && count(help_id_) > 0){
            print_usage();
            std::exit(0);
        } 
    }

    /// @brief 返回一个选项
    /// @param opt_id 
    /// @return Option
    Option const & get(OptionIdType opt_id) const 
    {
        //return options_[opt_id];
        // 这个才会抛出异常
        return options_.at(opt_id);
    }

    /// @brief 返回指定参数的值出现的数量
    /// @param opt_id 
    /// @return 
    std::size_t count(OptionIdType opt_id) const
    {
        for (auto & kv : options_){
            if (opt_id == kv.first){
                return kv.second.values.size();
            }
        }

        return 0;
    }

    /// @brief 获取未匹配参数
    /// @return 
    std::vector<std::string> const & get_unmatched() const
    {
        return unmatched_args_;
    }

    /// @brief 打印帮助
    void print_usage() const 
    {
        /*
        Usage:
        app_name [options] 
        Options:
        -v,--key        tips  
        */
        size_t mark_size = 0;
        for (auto & kv : options_){
            auto opt = kv.second;
            if (mark_size < opt.origin.size()){
                mark_size = opt.origin.size();
            }
        }
        // 如果mark比较短，就使用8个空格，如果比较长就使用4个
        mark_size += ((mark_size < 16) ? 6 : 4);
        std::cout << "Usage:" << std::endl;
        std::cout << "  " << app_name_ << " [Options]" << std::endl;
        std::cout << "Options:" << std::endl;

        for (auto & kv : options_){
            auto opt = kv.second;
            std::cout << "  " << opt.origin << std::setfill(' ') << std::setw(mark_size - opt.origin.size()) << "" << opt.tip << std::endl; 
        }        
    }

    /// @brief 显示解析到的信息，调试使用
    void dump() const 
    {
        std::cout << " ---" << std::endl;
        for (auto &kv : options_){
            kv.second.dump();
        }
        std::cout << " ---" << std::endl;
        std::cout << "unmatched args:";
        for (auto &arg : unmatched_args_){
            std::cout << arg << " ";
        }
        std::cout << std::endl;
    }

private:
    std::string app_name_;
    std::map<OptionIdType, Option> options_;
    std::vector<std::string> unmatched_args_;
    OptionIdType help_id_;
    bool has_help_id_ = false;

    // 打印选项
};


} // end cxxargs

#endif // __CXXARGS_H__
