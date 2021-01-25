#pragma once
#include <functional>
#include <sys/time.h>




class EventLoop;
// ���ɸ���
class Channel
{
public:
	typedef std::function<void()> EventCallback;

	Channel(EventLoop* loop, int fd);
	~Channel();

	void handleEvent();

	//���ö���д���ء������¼�
	void setReadCallback(const EventCallback& cb)
	{
		readCallback_ = cb;
	}

	void enableReading()
	{
		events_ |= kReadEvent;
		update();
	}


	//void remove();

	bool isNoneEvent() const { return events_ == kNoneEvent; }



	// ���溯��
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
	EventCallback readCallback_;


};

