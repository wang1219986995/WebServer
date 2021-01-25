#pragma once
#include <set>
#include <stdint.h>
#include <sys/time.h>
#include <functional>
#include <vector>
#include <memory>

#include "Channel.h"
class EventLoop;


struct TimeStamp;



// 不可复制，需要保证定时事件的唯一性
class TimerEvent {
public:
	// @problem 定时器需要处理的任务是不是应该在 定时器 中设置？
	// 先暂定这样
	typedef std::function<void()> TimerCallback;
	TimerEvent(const TimerCallback& cb, uint64_t when, uint64_t interval)
		:callback_(cb), expiration_(when),
		interval_(interval), repeat_(interval > 0) {};

	void run() const { callback_(); }

	bool repeat() const { return repeat_; }

	// 这个参数是什么意思
	void restart(uint64_t now)
	{
		if (repeat_)
		{
			expiration_ = now + interval_;
		}
		else
		{

		}
	}

	uint64_t getExpiration() { return expiration_; }

private:
	// 表示到期时间，微秒数
	uint64_t expiration_;




	// @problem 为什么是 const 的， 不是行不行
	const TimerCallback callback_;
	const uint64_t interval_;
	const bool repeat_;


};


// 重载 < 运算符，用于 std::set 容器中，定时器的排序
inline bool operator<(TimerEvent& lhs, TimerEvent& rhs)
{
	return lhs.getExpiration() < rhs.getExpiration();
}

inline bool operator==(TimerEvent& lhs, TimerEvent& rhs)
{
	return lhs.getExpiration() == rhs.getExpiration();
}

inline bool operator>(TimerEvent& lhs, TimerEvent& rhs)
{
	return lhs.getExpiration() > rhs.getExpiration();
}






// 使用 std::set 容器来管理定时器事件 
class TimerSet
{
public:
	typedef std::pair<uint64_t, TimerEvent* > TimerElement;
	typedef std::set<TimerElement> TimerEventSet;
	typedef std::function<void()> TimerCallback;



	// @problem 为什么要用EventLoop 初始化
	TimerSet(EventLoop* loop);
	//~TimerSet();


	// @problem muduo中返回值为类 TimerId， 意义是什么
	//void addTimer(const TimerCallback& cb, timeval when, double interval);
	void addTimer(const TimerCallback& cb, uint64_t when, uint64_t interval);
	
	void addTimerInLoop(TimerEvent* timer);


private:

	typedef std::pair<uint64_t, TimerEvent*> pairElement;
	typedef std::vector<pairElement> pairElementList;


	uint64_t transformToUS(timeval& time);

	void handleRead();
	void reset(const pairElementList& expired, uint64_t now);
	void resetTimerfd(int timerfd, uint64_t expiration);
	void readTimerfd(int timerfd, uint64_t now);
	// muduo中使用 std::pair<Timerstamp, Timer*>来管理定时器

	//  @problem 为什么要用pair， 在Timer中声明的变量expiration_ 变量来排序不行么？

	
	// @problem unique_ptr 不允许拷贝普通的拷贝和赋值
	// 参考：   https://www.cnblogs.com/DswCnblog/p/5628195.html

	//std::vector<TimerElement> getExpiredTimer(timeval now);

	// expiration 为微秒数
	timespec howMuchTimeFromNow(uint64_t expiration );

	pairElementList getExpiredTimer(uint64_t now);
	
	bool insert(TimerEvent* timer);
	
	TimerEventSet timers_;
	const int timerfd_;
	EventLoop* loop_;
	Channel timerfdChannel_;



};

