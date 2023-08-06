
#ifndef __CORE_DUMP_H__
#define __CORE_DUMP_H__

#ifdef __cplusplus
extern "C"
{
#endif

#include <execinfo.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

// SIGABRT信号处理函数
static inline void core_dump(int signum) 
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

#ifdef __cplusplus
}
#endif

#endif // __CORE_DUMP_H__
