#pragma once
#include <mutex>
#include <condition_variable>


#include "base/Thread.h"

class EventLoop;


// ���ɸ���
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

	// muduo������һ��Thread�࣬�ȷ���������棬�����ٸ�
	ideal::Thread thread_;
	ThreadInitCallback callback_;




};

