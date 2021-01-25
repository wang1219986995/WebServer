#include "Timer.h"
#include <assert.h>
#include <iterator>
#include "EventLoop.h"
#include <string.h>
#include <sys/timerfd.h>
#include <iostream>
#include <functional>


const int kStoUS = 1000000;
const int kStoNS = 1000000000;
const int kUStoNS = 1000;



int createTimerfd()
{
	int timerfd = ::timerfd_create(CLOCK_MONOTONIC,
		TFD_NONBLOCK | TFD_CLOEXEC);
	if (timerfd < 0)
	{
		std::cout << "Failed in timerfd_creater." << std::endl;
	}
	return timerfd;
}









TimerSet::TimerSet(EventLoop* loop):
	loop_(loop),timerfd_(createTimerfd() ),timerfdChannel_(Channel(loop,timerfd_)),timers_()
{
	//这里还没写完
	timerfdChannel_.setReadCallback(std::bind(&TimerSet::handleRead,this));
	timerfdChannel_.enableReading();
}

void TimerSet::addTimer(const TimerCallback& cb, uint64_t when, uint64_t interval)
{
	TimerEvent* timer = new TimerEvent(cb, when, interval);;
	
	loop_->runInLoop(std::bind(&TimerSet::addTimerInLoop, this, timer));
	
}

void TimerSet::addTimerInLoop(TimerEvent* timer)
{
	loop_->assertInLoopThread();


	// 判断是不是最早发生的
	bool earliestChanged = insert(timer);
	if (earliestChanged)
	{
		// 设置fd 参数在多久之后可读， muduo中封装了一个resetTimerfd 函数
		resetTimerfd(timerfd_, timer->getExpiration());
	}
}


void TimerSet::handleRead()
{
	loop_->assertInLoopThread();
	timeval value;
	gettimeofday(&value, NULL);

	// 这里先暂定这样，标志一下
	uint64_t now = static_cast<uint64_t>(value.tv_sec * kStoUS ) + static_cast<uint64_t>(value.tv_usec);

	readTimerfd(timerfd_, now);


	pairElementList expired = getExpiredTimer(now);

	for (pairElementList::iterator it = expired.begin();
		it != expired.end(); ++it)
	{
		it->second->run();
	}

	//reset函数
	reset(expired, now);

}



timespec TimerSet::howMuchTimeFromNow(uint64_t expiration)
{
	timeval now;
	gettimeofday(&now, NULL);
	int64_t microseconds = expiration - (static_cast<uint64_t>(now.tv_sec) * kStoUS + now.tv_usec);
	if (microseconds < 100)
	{
		microseconds = 100;
	}

	struct timespec ts;

	ts.tv_sec = static_cast<time_t>(microseconds / kStoUS);
	ts.tv_nsec = static_cast<long>((microseconds * kUStoNS) % kStoNS);
	return ts;
}

typename TimerSet::pairElementList TimerSet::getExpiredTimer(uint64_t now)
{
	std::vector<pairElement> expired;
	TimerElement until(now, reinterpret_cast<TimerEvent*>(UINTPTR_MAX)  );
	TimerEventSet::iterator end = timers_.lower_bound(until);
	assert(end == timers_.end() || now < end->first );
	std::copy(timers_.begin(), end, std::back_inserter(expired) );
	timers_.erase(timers_.begin(), end);
	return expired;

}

bool TimerSet::insert(TimerEvent* timer)
{
	bool earliestChanged = false;
	uint64_t when = timer->getExpiration();
	TimerEventSet::iterator it = timers_.begin();
	if (it == timers_.end() || when < it->first)
	{
		earliestChanged = true;
	}
	timers_.insert(std::make_pair(when, timer));
	return earliestChanged;
}



void TimerSet::reset(const pairElementList& expired, uint64_t now)
{
	uint64_t nextExpire;
	for (pairElementList::const_iterator it = expired.begin();
		it != expired.end(); ++it)
	{
		if (it->second->repeat())
		{
			it->second->restart(now);
			insert(it->second);
		}
		else
		{
			delete it->second;
		}

		if (!timers_.empty())
		{
			nextExpire = timers_.begin()->second->getExpiration();
		}
		// 这里还没有写完
		if (nextExpire > 0)
		{
			resetTimerfd(timerfd_, nextExpire);
		}

	}

}

void TimerSet::resetTimerfd(int timerfd, uint64_t expiration)
{
	struct itimerspec newValue, oldValue;
	bzero(&newValue, sizeof newValue);
	bzero(&oldValue, sizeof newValue);

	newValue.it_value = howMuchTimeFromNow(expiration);
	int ret = timerfd_settime(timerfd_, 0, &newValue, &oldValue);
	if (ret)
	{
		std::cout << "timerfd_settime()  successfull ." << std::endl;
	}
}

void TimerSet::readTimerfd(int timerfd, uint64_t now)
{
	uint64_t howmany;
	ssize_t  n = ::read(timerfd, &howmany, sizeof howmany);
	std::cout << howmany << " at readTimerfd()" << std::endl;
	if (n != sizeof howmany)
	{
		std::cout << "n != sizeof howmany " << std::endl;
	}
}



uint64_t TimerSet::transformToUS(timeval& time)
{
	uint64_t s = time.tv_sec;
	return uint64_t(s * kStoUS + time.tv_usec);
}