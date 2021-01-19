#pragma once
#include <mutex>
#include <condition_variable>


#include "base/Thread.h"

class EventLoop;


// 不可复制
class EventLoopThread
{
public:
	typedef std::function<void(EventLoop*)> ThreadInitCallback;
	EventLoopThread(const ThreadInitCallback& cb = ThreadInitCallback(),
		const std::string& name = std::string() );

	EventLoopThread(const EventLoopThread*) = delete;


	~EventLoopThread();
	EventLoop* startLoop();

private:
	void threadFunc();
	
	EventLoop* loop_;
	bool exiting_;
	std::mutex mutex_;
	std::condition_variable condition_;

	// muduo中用了一个Thread类，先放在这里代替，后面再改
	ideal::Thread thread_;
	ThreadInitCallback callback_;




};

