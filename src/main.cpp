#include "Channel.h"
#include "EventLoop.h"

#include <stdio.h>
#include <sys/timerfd.h>
#include <string>
#include <string.h>
#include <iostream>

//void timeout()
//{
//    printf("Timeout!\n");
//    g_loop->quit();
//}
//
//int main()
//{
//    // 创建一个loop的对象，loop的对象每个线程最多拥有一个loop对象，loop对象里面有一个死循环 loop.loop() 函数
//    // loop对象在初始化的时候，自己的数据成员有一个是Poller类的对象，在这个时候完成了初始化
//    EventLoop loop;
//    g_loop = &loop;
//
//    // 创建一个fd测试
//    int timerfd = ::timerfd_create(CLOCK_MONOTONIC, TFD_NONBLOCK | TFD_CLOEXEC);
//
//    // chanel 需要loop对象和fd
//    Channel channel(&loop, timerfd);
//    channel.setReadCallback(timeout);
//
//    // 这个里面调用了updateChannel函数
//    // 通过这个enablereading，将自己注册到poller对象里面
//    channel.enableReading();
//
//    // 设置fd的属性
//    struct itimerspec howlong;
//    bzero(&howlong, sizeof howlong);
//    howlong.it_value.tv_sec = 5;
//    ::timerfd_settime(timerfd, 0, &howlong, NULL);
//
//    // loop.loop
//    loop.loop();
//
//    ::close(timerfd);
//}


//
//int cnt = 0;
//EventLoop* g_loop;
//
//void printTid()
//{
//    printf("pid = %d, tid = %d\n", getpid(), syscall(SYS_gettid ) );
//    //printf("now %s\n", muduo::Timestamp::now().toString().c_str());
//}
//
//void print(const char* msg)
//{
//    std::cout << "cnt is : " << cnt << "  ";
//    printf("msg  %s\n", msg);
//    if (++cnt == 10)
//    {
//        g_loop->quit();
//    }
//}
//
//int main()
//{
//    printTid();
//    EventLoop loop;
//    g_loop = &loop;
//
//    print("main");
//    //loop.runAfter(1, std::bind(print, "once1"));
//    // loop.runAfter(1.5, boost::bind(print, "once1.5"));
//    // loop.runAfter(2.5, boost::bind(print, "once2.5"));
//    // loop.runAfter(3.5, boost::bind(print, "once3.5"));
//    loop.runEvery(1, std::bind(print, "every2"));
//    // loop.runEvery(3, boost::bind(print, "every3"));
//
//    loop.loop();
//    print("main loop exits");
//    sleep(1);
//}


#include <stdio.h>

//
//int g_flag = 0;
//
//void run4()
//{
//    printf("run4(): pid = %d, flag = %d\n", getpid(), g_flag);
//    g_loop->quit();
//}
//
//void run3()
//{
//    printf("run3(): pid = %d, flag = %d\n", getpid(), g_flag);
//    g_loop->runAfter(3, run4);
//    g_flag = 3;
//}
//
//void run2()
//{
//    printf("run2(): pid = %d, flag = %d\n", getpid(), g_flag);
//    g_loop->queueInLoop(run3);
//}
//
//void run1()
//{
//    g_flag = 1;
//    printf("run1(): pid = %d, flag = %d\n", getpid(), g_flag);
//    g_loop->runInLoop(run2); // 理解这个runInLoop的函数的功能
//    g_flag = 2;
//}
//
//int main()
//{
//    printf("main(): pid = %d, flag = %d\n", getpid(), g_flag);
//
//    EventLoop loop; // 注意哦，loop对象事被跨线程调用
//    g_loop = &loop;
//
//    loop.runAfter(2, run1);
//    loop.loop();
//    printf("main(): pid = %d, flag = %d\n", getpid(), g_flag);
//}

// test5.cc 只是一个单线程的程序，用来测试函数的正确性
// test6.cc 是一个多线程的程序
#include "EventLoopThread.h"

void runInThread()
{
    printf("runInThread(): pid = %d, tid = %d\n", getpid(), static_cast<pid_t>(::syscall(SYS_gettid)) );
}

int main()
{
    // 加入主线程的tid是660
    printf("main(): pid = %d, tid = %d\n", getpid(), static_cast<pid_t>(::syscall(SYS_gettid)));

    // 这EventLoopThread对象里面创建了一个子线程，子线程拥有一个loop对象，并返还给loop指针
    // 因为loop对象是在子线程里面创建的，加入子线程的id是661，那么loop对象里面的id是661
    EventLoopThread loopThread;
    EventLoop* loop = loopThread.startLoop();

    //在主线程里面调用，子线程创建的对象，那么这个任务不会立刻执行，会被插入到子线程的loop里面，被子线程执行
    loop->runInLoop(runInThread);
    sleep(1);

    //这个也是同样的道理
    loop->runAfter(2, runInThread);
    //sleep(3);
    //loop->quit();

    while (1)
    {

    }

    printf("exit main().\n");
}
