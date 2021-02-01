#pragma once
#include <functional>
#include <pthread.h>
#include <sys/syscall.h>
#include <sys/types.h>
#include <memory>
#include <unistd.h>
#include <vector>
#include <mutex>

class Channel;
class Poller;
class TimerSet;
typedef std::vector<Channel*> ChannelList;
// 不可复制和拷贝
class EventLoop
{
public:
	typedef std::function<void()> TimerCallback;
	typedef std::function<void()> Functor;
	EventLoop();
	~EventLoop();

	void loop();
	void quit() 
	{ 
		quit_ = true;
		if (!isInLoopThread())
		{
			wakeup();
		}
	}


	void updateChannel(Channel* channel);

	void assertInLoopThread()
	{
		if (!isInLoopThread())
		{
			abortnotInLoopThread();
		}
	}

	bool isInLoopThread() const { return threadId_ == syscall(SYS_gettid); }



	void runInLoop(const Functor& cb);

	void queueInLoop(const Functor& cb);
	
	void wakeup();


	// 表示定时器设定时间是的单位，秒、毫秒、微秒
	enum TimeInputState { kS=1, kMS, kUS };




	static const int kStoUS = 1000 * 1000;
	static const int kMStoUS = 1000;

	void runAt(uint64_t time, const TimerCallback& cb, int flag = kS);

	void runAfter(uint64_t time, const TimerCallback& cb, int flag = kS);

	void runEvery(uint64_t time, const TimerCallback& cb, int flag = kS);



private:
	void abortnotInLoopThread();
	void handleRead();
	void doPendingFunctors();


	bool looping_;
	bool quit_;
	bool callingPendingFunctors_;

	const pid_t threadId_;
	std::shared_ptr<Poller> poller_;
	std::shared_ptr<TimerSet> timerset_;
	int wakeupFd_;
	std::shared_ptr<Channel> wakeupChannel_;
	std::vector<Functor> pendingFunctors_;
	std::mutex mutex_;

	uint64_t pollReturnTime_;
	

	ChannelList activateChannels_;
	Channel* currentActivateChannel_;





};

