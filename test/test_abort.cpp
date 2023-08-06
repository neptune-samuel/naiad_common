
#include <execinfo.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

// SIGABRT信号处理函数
void core_dump(int signum) 
{
    void* array[16];
    size_t size;

    fprintf(stderr, "====>>> catch signal %d <<<====\r\n", signum);
    // 获取backtrace
    size = backtrace(array, 16);
    // 打印backtrace
    fprintf(stderr, "backtrace(%lu):\r\n", size);
    backtrace_symbols_fd(array, size, STDERR_FILENO);
    // 退出程序
    exit(1);
}

void test()
{
    int a = 10;
    int b = 0;
    int c = a ;
    
    printf("aaaa\r\n");
    printf("bbbb\r\n");

    abort();

    printf("c = %d\r\n", c);
}


int main() {
    // 注册SIGABRT信号处理函数
    signal(SIGABRT, core_dump);

    // 人为制造一个SIGABRT信号
    //abort();
    test();
    
    return 0;
}

