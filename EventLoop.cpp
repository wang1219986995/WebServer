#include <assert.h>
#include "EventLoop.h"
#include "Poller.h"
#include "Channel.h"
#include <iostream>
#include <pthread.h>
#include <unistd.h>
#include <sys/types.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/syscall.h>
#include <stdint.h>

__thread EventLoop* t_loopInThisThread = 0;
const int kPollTimeMs = 10000;


// pid,tid,真实pid  的使用
// 进程pid: getpid()                 
// 线程tid: pthread_self()     //进程内唯一，但是在不同进程则不唯一。
// 线程pid: syscall(SYS_gettid)     //系统内是唯一的
//EventLoop::EventLoop() :looping_(false), quit_(false), threadId_(syscall(SYS_gettid))
EventLoop::EventLoop():
    looping_(false), quit_(false), 
    threadId_( static_cast<pid_t>(::syscall(SYS_gettid)) ), 
    poller_(new Poller(this))
{
    std::cout << "Another EnentLoop " << t_loopInThisThread <<
        "   exists in this thread  " << threadId_ << std::endl;
    if (t_loopInThisThread)
    {
        std::cout << "Another EnentLoop " << t_loopInThisThread <<
            "exists in this thread" << threadId_ << std::endl;
    }
    else
    {
        t_loopInThisThread = this;
    }

}

EventLoop::~EventLoop()
{
    assert(!looping_);
    t_loopInThisThread = NULL;
}

void EventLoop::loop()
{
    assert(!looping_);
    assertInLoopThread();
    looping_ = true;
    quit_ = false;

    while (!quit_)
    {
        activateChannels_.clear();
        poller_->poll(kPollTimeMs, &activateChannels_);
        
        for (ChannelList::iterator it = activateChannels_.begin(); it != activateChannels_.end(); ++it)
        {
            (*it)->handleEvent();
        }
        doPendingFunctors();
    }

    std::cout << "EventLoop " << this << "  stop looping";
    looping_ = false;
}



void EventLoop::quit()
{
    quit_ = true;
    if (!isInLoopThread())
    {
        wakeup();
    }
}

void EventLoop::runInLoop(const Functor& cb)
{
    if (isInLoopThread())
    {
        printf("[debug] runInLoop() : isInLoopThread \n");
        cb();
    }
    else
    {
        printf("[debug] runInLoop() : not isInLoopThread \n");
        queueInLoop(cb);
        
    }
}

void EventLoop::queueInLoop(const Functor& cb)
{
    std::lock_guard<std::mutex> lg(mutex_);
    pendingFunctors_.push_back(cb);
    if (!isInLoopThread() || callingPendingFunctors_)
    {
        wakeup();
    }
}

TimerId EventLoop::runAt(const Timestamp& time, const TimerCallBack& cb)
{


    return timerQueue_->addTimer(cb, time, 0.0);
}

TimerId EventLoop::runAfter(double delay, const TimerCallBack& cb)
{

    Timestamp time(addTime(Timestamp::now(), delay));
    return runAt(time, cb);
}

TimerId EventLoop::runEvery(double interval, const TimerCallBack& cb)
{
    Timestamp time(addTime(Timestamp::now(), interval));
    return timerQueue_->addTimer(cb, time, interval);
}

void EventLoop::wakeup()
{
    uint64_t one = 1;
    ssize_t n = ::write(wakeupFd_, &one, sizeof one);
    if (n != sizeof one)
    {
        std::cout << "EventLoop::wakeup() writes " << n <<
            " bytes instead of 8";
    }
}

void EventLoop::updateChannel(Channel* channel)
{
     assert(channel->ownerLoop() == this);
     assertInLoopThread();
     poller_ -> updateChannel(channel);
}

void EventLoop::abortNotInLoopThread()
{
    std::cout << "EventLoop::abortNotInLoopThread - EventLoop " << this
        << " was created in threadId_ = " << threadId_
        << ", current thread id = " << syscall(SYS_gettid) << std::endl;
}

void EventLoop::handleRead()
{
    uint64_t one = 1;
    ssize_t n = ::write(wakeupFd_, &one, sizeof one);
    if (n != sizeof one)
    {
        std::cout << "EventLoop::handleRead() reads " << n <<
            " bytes instead of 8";
    }
}

void EventLoop::doPendingFunctors()
{
    std::vector<Functor> functors;
    callingPendingFunctors_ = true;
    
    std::lock_guard<std::mutex> lg(mutex_);
    functors.swap(pendingFunctors_);

    for (size_t i = 0; i < functors.size(); ++i)
    {
        functors[i]();
    }
    callingPendingFunctors_ = false;
}


