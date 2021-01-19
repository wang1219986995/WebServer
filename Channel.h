#pragma once
#ifndef MUDUO_CHNNEL
#define MUDUO_CHNNEL

#include <functional>

class EventLoop;

class Channel
{
public:
	// C++11中std::function的使用  https://blog.csdn.net/fengbingchun/article/details/52562918
	typedef std::function<void()> EventCallback;

	Channel(EventLoop* loop, int fd);
	Channel(const Channel&) = delete;


	void handleEvent();
	void setReadCallback(const EventCallback& cb)
	{	readCallback_ = cb; }
	void setWriteCallback(const EventCallback& cb)
	{	writeCallback_ = cb; }
	void setErrorCallback(const EventCallback& cb)
	{	errorCallback_ = cb; }

	int fd() const { return fd_; }
	int events() const { return events_; }
	void set_revents(int revt) { revents_ = revt; }
	bool isNoneEvent() const { return events_ == kNoneEvent; }

	void enableReading() { events_ |= kReadEvent; update(); }

	// Poller中使用的函数
	int index() { return index_; }
	void set_index(int idx) { index_ = idx; }

	EventLoop* ownerLoop() { return loop_; }

private:  
	void update();
	static const int kNoneEvent;
	static const int kReadEvent;
	static const int kWriteEvent;

	EventLoop* loop_;
	const int fd_;
	int events_;
	int revents_;
	int index_;

	EventCallback readCallback_;
	EventCallback writeCallback_;
	EventCallback errorCallback_;

};


#endif // !MUDUO_CHNNEL





