#pragma once
#ifndef MUDUO_EVENT_LOOP
#define MUDUO_EVENT_LOOP


#include <pthread.h>
#include <vector>
#include <memory>
#include <sys/types.h>
#include <sys/syscall.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <signal.h>
#include <iostream>
#include <mutex>
#include "Timer.h"
#include "TimerQueue.h"



class Channel;
class Poller;

class EventLoop
{
public:
    typedef std::function<void()> TimerCallBack;
    typedef std::function<void()> Functor;

    EventLoop();

    EventLoop(const EventLoop&) = delete;

    ~EventLoop();

    void loop();
    void quit();

    Timestamp pollReturnTime() const { return pollReturnTime_; }

    void runInLoop(const Functor& cb);

    void queueInLoop(const Functor& cb);



    // Timers中有关回调函数的部分
    TimerId runAt(const Timestamp& time, const TimerCallBack& cb);

    TimerId runAfter(double delay, const TimerCallBack& cb);

    TimerId runEvery(double interval, const TimerCallBack& cb);

    void wakeup();

    void updateChannel(Channel* channel);

    void assertInLoopThread()
    {
        if (!isInLoopThread())
        {
            abortNotInLoopThread();
        }

    }

    bool isInLoopThread() const 
    { 
        std::cout << threadId_ << "   syscall :  " << static_cast<pid_t>(::syscall(SYS_gettid)) << std::endl;
        return (threadId_ == static_cast<pid_t>(::syscall(SYS_gettid))); 
    }



private:
    void abortNotInLoopThread();
    void handleRead();
    void doPendingFunctors();



    typedef std::vector<Channel*> ChannelList;


    bool looping_;
    bool quit_;
    bool callingPendingFunctors_;
    const pid_t threadId_;
    Timestamp pollReturnTime_;



    // boost库中的指针类型
    std::shared_ptr<Poller> poller_;
    std::shared_ptr<TimerQueue> timerQueue_;
    int wakeupFd_;
    std::shared_ptr<Channel> wakeupChannel_;
    ChannelList activateChannels_;
    mutable std::mutex mutex_;
    std::vector<Functor> pendingFunctors_;
};

#endif // !MUDUO_EVENT_LOOP