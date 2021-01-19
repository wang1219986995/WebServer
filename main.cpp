//#include <iostream>
//#include <pthread.h>
//#include <unistd.h>
//#include "EventLoop.h"
//#include "Channel.h"
//
//#include <sys/syscall.h>
//#include <sys/types.h>
//#include <sys/timerfd.h>
//#include <stdio.h>
//#include <string.h>
//
//__thread int i = 0;
//
//
//
//EventLoop* g_loop;
//
//void timeout()
//{
//    printf("Timeout!\n");
//    g_loop->quit();
//}
//
//
//
//
//
//int main()
//{
//    std::cout << "There is begin:  " << std::endl;
//    EventLoop loop;
//    g_loop = &loop;
//
//    int timerfd = ::timerfd_create(CLOCK_MONOTONIC, TFD_NONBLOCK | TFD_CLOEXEC);
//    Channel channel(&loop, timerfd);
//    channel.setReadCallback(timeout);
//    channel.enableReading();
//    struct itimerspec howlong;
//    bzero(&howlong, sizeof howlong);
//    howlong.it_value.tv_sec = 2;
//    loop.loop();
//
//    ::close(timerfd);
//
//    std::cout << "There is end!" << std::endl;
//}



#include <iostream>
#include <string>
#include <thread>
#include <mutex>
#include <condition_variable>

std::mutex m;
std::condition_variable cv;
std::string data;
bool ready = false;
bool processed = false;

void worker_thread()
{
    // Wait until main() sends data
    std::unique_lock<std::mutex> lk(m);
    cv.wait(lk, [] {return ready; });

    // after the wait, we own the lock.
    std::cout << "Worker thread is processing data\n";
    data += " after processing";

    // Send data back to main()
    processed = true;
    std::cout << "Worker thread signals data processing completed\n";

    // Manual unlocking is done before notifying, to avoid waking up
    // the waiting thread only to block again (see notify_one for details)
    lk.unlock();
    cv.notify_one();
}

int main()
{
    std::thread worker(worker_thread);

    data = "Example data";
    // send data to the worker thread
    {
        std::lock_guard<std::mutex> lk(m);
        ready = true;
        std::cout << "main() signals data ready for processing\n";
    }
    cv.notify_one();

    // wait for the worker
    {
        std::unique_lock<std::mutex> lk(m);
        cv.wait(lk, [] {return processed; });
    }
    std::cout << "Back in main(), data = " << data << '\n';

    worker.join();
}