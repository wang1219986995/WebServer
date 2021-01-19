#include "EventLoopThread.h"
#include "EventLoop.h"
#include <assert.h>



EventLoopThread::EventLoopThread(const ThreadInitCallback& cb, const std::string& name):
loop_(nullptr), 
exiting_(false),
thread_(std::bind(&EventLoopThread::threadFunc,this),name),
mutex_(),
condition_(),
callback_(cb)
{
	
}




EventLoopThread::~EventLoopThread()
{
	exiting_ = true;
	loop_->quit();
	thread_.join();
}

EventLoop* EventLoopThread::startLoop()
{
	// 这个函数没看懂
	assert(!thread_.started());
	thread_.start();

	EventLoop* loop = nullptr;
	std::unique_lock<std::mutex> ul(mutex_);
	while (loop_ == NULL)
	{
		condition_.wait(ul, [this]() { return loop_ != nullptr; });
		loop = loop_;
	}

	return loop;
}

void EventLoopThread::threadFunc()
{
	EventLoop loop;
	if (callback_)
	{
		callback_(&loop);
	}

	{
		std::lock_guard<std::mutex> lg(mutex_);
		loop_ = &loop;
		condition_.notify_all();
	}
	loop.loop();

	std::lock_guard<std::mutex> lg(mutex_);
	loop_ = nullptr;
}
