#pragma once
#include <functional>

#include "Timestamp.h"

class Timer
{
public:
	typedef std::function<void()> TimerCallBack;
	Timer(const TimerCallBack& cb, Timestamp when, double interval)
		:callback_(cb),
		expiration_(when),
		interval_(interval),
		repeat_(interval > 0)
	{}

	void run() const { callback_(); }

	Timestamp expiration() const { return expiration_; }

	bool repeat() const { return repeat_; }

	void restart(Timestamp now);

private:
	const TimerCallBack callback_;
	Timestamp expiration_;
	const double interval_;
	const bool repeat_;

};




// ¿É¸´ÖÆµÄ
class TimerId {
public:
	explicit TimerId(Timer* timer) :value_(timer) {}

private:
	Timer* value_;
};







