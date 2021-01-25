#pragma once
#include <mutex>
#include <condition_variable>
#include <thread>



#include "Thread.h"

class EventLoop;


// ���ɸ���
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

	//muduo�ж�thread��mutex��condition_variable �����˷�װ
	Thread thread_;
	std::mutex mutex_;
	std::condition_variable cond_;
};

