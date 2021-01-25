#include "EventLoopThread.h"
#include "EventLoop.h"

#include <assert.h>
#include <iostream>


EventLoopThread::EventLoopThread():
	loop_(NULL),exiting_(false),
	thread_(std::bind(&EventLoopThread::threadFunc,this)),
	mutex_(),cond_()
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
	assert(!thread_.started());
	thread_.start();

	{
		std::unique_lock<std::mutex> uk(mutex_);
		cond_.wait(uk, [this]() { return loop_ != NULL; });
	}
	return loop_;
}

void EventLoopThread::threadFunc()
{
	EventLoop loop;
	
	{
		std::lock_guard<std::mutex> lg(mutex_);
		loop_ = &loop;
		cond_.notify_all();
	}
	loop.loop();

	std::lock_guard<std::mutex> lg(mutex_);
	loop_ = NULL;
}
