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



// ���ɸ��ƣ���Ҫ��֤��ʱ�¼���Ψһ��
class TimerEvent {
public:
	// @problem ��ʱ����Ҫ����������ǲ���Ӧ���� ��ʱ�� �����ã�
	// ���ݶ�����
	typedef std::function<void()> TimerCallback;
	TimerEvent(const TimerCallback& cb, uint64_t when, uint64_t interval)
		:callback_(cb), expiration_(when),
		interval_(interval), repeat_(interval > 0) {};

	void run() const { callback_(); }

	bool repeat() const { return repeat_; }

	// ���������ʲô��˼
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
	// ��ʾ����ʱ�䣬΢����
	uint64_t expiration_;




	// @problem Ϊʲô�� const �ģ� �����в���
	const TimerCallback callback_;
	const uint64_t interval_;
	const bool repeat_;


};


// ���� < ����������� std::set �����У���ʱ��������
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






// ʹ�� std::set ����������ʱ���¼� 
class TimerSet
{
public:
	typedef std::pair<uint64_t, TimerEvent* > TimerElement;
	typedef std::set<TimerElement> TimerEventSet;
	typedef std::function<void()> TimerCallback;



	// @problem ΪʲôҪ��EventLoop ��ʼ��
	TimerSet(EventLoop* loop);
	//~TimerSet();


	// @problem muduo�з���ֵΪ�� TimerId�� ������ʲô
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
	// muduo��ʹ�� std::pair<Timerstamp, Timer*>������ʱ��

	//  @problem ΪʲôҪ��pair�� ��Timer�������ı���expiration_ ������������ô��

	
	// @problem unique_ptr ����������ͨ�Ŀ����͸�ֵ
	// �ο���   https://www.cnblogs.com/DswCnblog/p/5628195.html

	//std::vector<TimerElement> getExpiredTimer(timeval now);

	// expiration Ϊ΢����
	timespec howMuchTimeFromNow(uint64_t expiration );

	pairElementList getExpiredTimer(uint64_t now);
	
	bool insert(TimerEvent* timer);
	
	TimerEventSet timers_;
	const int timerfd_;
	EventLoop* loop_;
	Channel timerfdChannel_;



};

