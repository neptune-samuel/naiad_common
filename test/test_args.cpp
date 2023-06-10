


#include <common/cxxargs.h>


bool check_baudrate(std::string const & arg, std::string & msg)
{
    if (arg != "9600" && arg != "115200")
    {
        msg = "Unsupported paramter(" + arg + ")";
        return false;
    }
    return true;
}



int main(int argc, const char *argv[])
{
    enum class ArgId: int {Help, Test, LogLevel, Serial, Baudrate, Files, TestNone = 1000};

    auto args = 
    cxxargs::Parser<ArgId>("test_args")
        .option(ArgId::Help,        "-h,--help",           "Print this message")
        .option(ArgId::Test,        "-t,--test",           "Enable test")
        .option(ArgId::LogLevel,    "-l,--log-level <level>",    "Set log level, available: info,debug,warning,error", "info")
        .option(ArgId::Serial,      "-s <device>",         "Set serial device", "/dev/ttyUSB0")
        .option(ArgId::Baudrate,    "-r,--rate <rate> ",   "Set serial baudrate", check_baudrate)
        .option(ArgId::Files,       "--files <file>...",   "Set files")
        .set_help(ArgId::Help);

    args.parse(argc, argv);

    args.dump();

    std::cout << "count(-l)" << args.count(ArgId::LogLevel) << std::endl;
    std::cout << "test   " << args.get(ArgId::Test).as_bool() << std::endl;
    std::cout << "log    " << args.get(ArgId::LogLevel).as_string() << std::endl;
    std::cout << "serial " << args.get(ArgId::Serial).as_string() << std::endl;
    std::cout << "rate   " << args.get(ArgId::Baudrate).as_number() << std::endl;
    // 访问一个不存在的选项，它会抛出异常
    //std::cout << "none  " << args.get(ArgId::TestNone).as_string() << std::endl;

    return 0;
}

