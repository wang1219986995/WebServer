#pragma once
#include <functional>
#include <sys/time.h>
#include <stdint.h>



class EventLoop;
// 不可复制
class Channel
{
public:
	typedef std::function<void()> EventCallback;
	typedef std::function<void(uint64_t)> ReadEventCallback;

	Channel(EventLoop* loop, int fd);
	~Channel();

	void handleEvent(  uint64_t revceiveTime);

	//设置读、写、关、错误事件
	void setReadCallback(const ReadEventCallback& cb)
	{
		readCallback_ = cb;
	}
	void setWriteCallback(const EventCallback& cb)
	{
		writeCallback_ = cb;
	}
	void setErrorCallback(const EventCallback& cb)
	{
		errorCallback_ = cb;
	}
	void setCloseCallback(const EventCallback& cb)
	{
		closeCallback_ = cb;
	}

	void enableReading()
	{
		events_ |= kReadEvent;
		update();
	}

	void enableWriting()
	{
		events_ |= kWriteEvent; update();
	}
	
	void disableWriting()
	{
		events_ &= ~kWriteEvent; update();
	}

	bool isWriting() const { return events_ & kWriteEvent; }


	//void remove();

	bool isNoneEvent() const { return events_ == kNoneEvent; }

	void disableAll() { events_ = kNoneEvent; update(); }

	// 常规函数
	int index() { return index_; }
	void set_index(int idx) { index_ = idx; }
	int fd() const { return fd_; }
	int events() const { return events_; }
	void set_revents(int rev) { revents_ = rev; }
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

	bool eventHandling_;







	ReadEventCallback readCallback_;
	EventCallback writeCallback_;
	EventCallback errorCallback_;
	EventCallback closeCallback_;

};

