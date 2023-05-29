
/*
```
可以将一个 `std::promise` 对象传递给多个子线程，但需要注意的是，`std::promise` 对象只能被设置一次值。

如果需要让多个子线程同时等待一个值的设置，可以使用 `std::shared_future` 类型来实现。具体来说，可以将一个 `std::future` 对象转换为 `std::shared_future` 对象，并将其传递给多个子线程。这样，多个子线程可以同时等待这个值的设置，而且这个值只需要被设置一次。

具体实现如下：

```
*/
#include <iostream>
#include <thread>
#include <future>
#include <ctime>
#include <cstdlib>


/*
request 是某个类的方法？ 如TCP服务的方法
*/
void request(std::string & msg, std::promise<int>& my_promise)
{
    // send msg
    

}


void read_data()
{
    std::promise<int> my_promise;
    std::future<int> my_future = my_promise.get_future();

    request("request msg", std::ref(my_promise));

    ret = my_future.get();
}



void my_function(std::promise<int>& my_promise) {
    // do some work

    std::srand(std::time(nullptr));

    while (true)
    {

        int ms = std::rand() % 1000 + 1;
        std::cout << "thread0 sleep: " << ms << std::endl;

        std::this_thread::sleep_for(std::chrono::milliseconds(ms));
        
        //if (std::future_status::ready())
        my_promise.set_value(ms);

    }

}


void my_function1(std::promise<int>& my_promise) {
    
    std::srand(std::time(nullptr) + 1);

    while (true)
    {
        int result = 43;
        my_promise.set_value(result);

        int ms = std::rand() % 1000 + 1;

        std::cout << "thread1 sleep: " << ms << std::endl;

        std::this_thread::sleep_for(std::chrono::milliseconds(ms));
    }
}

int main() {
    std::promise<int> my_promise;
    std::future<int> my_future = my_promise.get_future();

    std::thread my_thread1(my_function, std::ref(my_promise));
    //std::thread my_thread2(my_function1, std::ref(my_promise));


    while (true)
    {
        int result1 = my_future.get();
        //int result2 = my_future.get();

        std::cout << "result1 = " << result1 << std::endl;
        //std::cout << "result2 = " << result2 << std::endl;
    }

    my_thread1.join();
    //my_thread2.join();



    return 0;
}

/*
```

在上述代码中，`std::shared_future` 对象 `my_future` 被传递给了两个子线程，它们都可以通过 `get()` 方法等待 `my_promise` 对象设置值。在主线程中，通过两次调用 `get()` 方法，分别获取了两个子线程设置的值，并输出到控制台上。

*/