#pragma once
#include <mutex>
#include <condition_variable>
#include <thread>



#include "Thread.h"

class EventLoop;


// 不可复制
class EventLoopThread
{
public:
	EventLoopThread();
	~EventLoopThread();
	EventLoop* startLoop();

private:

	void threadFunc();
	EventLoop* loop_;
	bool exiting_;

	//muduo中对thread、mutex和condition_variable 都做了封装
	Thread thread_;
	std::mutex mutex_;
	std::condition_variable cond_;
};

