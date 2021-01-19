#pragma once
#include <set>
#include <vector>
#include <functional>

#include "Timestamp.h"
#include "Channel.h"

class EventLoop;
class TimerId;
class Timer;

// ²»¿É¸´ÖÆ
class TimerQueue
{
public:
	TimerQueue(EventLoop* loop);
	TimerQueue(const TimerQueue&) = delete;
	~TimerQueue();

	typedef std::function<void()> TimerCallBack;
	TimerId addTimer(const TimerCallBack& cb, Timestamp when, double interval);


private:
	typedef std::pair<Timestamp, Timer*> Entry;
	typedef std::set<Entry> TimerList;

	void addTimerInLoop(Timer* timer);
	void handleRead();
	std::vector<Entry> getExpired(Timestamp now);
	void reset(const std::vector<Entry>& expired, Timestamp now);
	bool insert(Timer* time);
	EventLoop* loop_;
	const int timerfd_;
	Channel timerfdChannel_;
	TimerList timers_;


};

