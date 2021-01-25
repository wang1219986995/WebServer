#include "EventLoop.h"
#include "Poller.h"
#include "Channel.h"
#include "Timer.h"

#include <assert.h>
#include <iostream>




// 这个变量是什么意思
const int kPollTimerMs = 1000;


EventLoop::EventLoop():looping_(false),quit_(false),
threadId_( static_cast<pid_t>(syscall(SYS_gettid)) ),
poller_(new Poller(this)), timerset_(new TimerSet(this))
{

	std::cout << " EventLoop has created. " << std::endl;

}

EventLoop::~EventLoop()
{
}



void EventLoop::loop()
{
	assert(!looping_);
	assertInLoopThread();
	looping_ = true;
	quit_ = false;
	std::cout << " EventLoop " << threadId_ << " start looping." << std::endl;
	while (!quit_) 
	{
		activateChannels_.clear();
		poller_->poll(kPollTimerMs, &activateChannels_);
		for (ChannelList::iterator it = activateChannels_.begin(); it != activateChannels_.end(); ++it)
		{
			(*it)->handleEvent();
		}

		// 加在这里的作用是什么， 8.3节
		doPendingFunctors();
	}
	std::cout << " EventLoop " << threadId_ << " has stop looping." << std::endl;
}

void EventLoop::updateChannel(Channel* channel)
{
	assert(channel->ownerLoop() == this);
	assertInLoopThread();
	poller_->updateChannel(channel);

}



void EventLoop::runInLoop(const Functor& cb)
{
	if (isInLoopThread())
	{
		std::cout << "runInLoop() : isInLoopThread() = true." << std::endl;
		cb();
	}
	else
	{
		std::cout << "runInLoop() : isInLoopThread() = false." << std::endl;
		queueInLoop(cb);
	}
}

void EventLoop::queueInLoop(const Functor& cb)
{
	{
		std::lock_guard<std::mutex> lg(mutex_);
		pendingFunctors_.push_back(cb);
	}

	if (!isInLoopThread() || callingPendingFunctors_)
	{
		wakeup();
	}
	
}

void EventLoop::wakeup()
{
	uint64_t one = 1;
	ssize_t n = ::write(wakeupFd_, &one, sizeof one);
	if (n != sizeof one)
	{
		std::cout << "EventLoop::wakeup() writes" << n << " bytes instead of 8." << std::endl;
	}
}

void EventLoop::runAt(uint64_t time, const TimerCallback& cb, int flag )
{
	if (flag == kS)
	{
		timerset_->addTimer(cb, time * kStoUS,0);
	}
	else if (flag == kMS)
	{
		timerset_->addTimer(cb, time * EventLoop::kMStoUS, 0);
	}
	else if (flag == kUS)
	{
		timerset_->addTimer(cb, time, 0);
	}
	else
	{
		std::cout << "Time fromat not match." << std::endl;
	}
	
}

void EventLoop::runAfter(uint64_t time, const TimerCallback& cb, int flag )
{
	runAt(time, cb, flag);
}

void EventLoop::runEvery(uint64_t time, const TimerCallback& cb, int flag)
{
	if (flag == kS)
	{
		timerset_->addTimer(cb, time * kStoUS, time * kStoUS);
	}
	else if (flag == kMS)
	{
		timerset_->addTimer(cb, time * kMStoUS, time * kMStoUS);
	}
	else if (flag == kUS)
	{
		timerset_->addTimer(cb, time, time);
	}
	else
	{
		std::cout << "Time fromat not match." << std::endl;
	}

}



void EventLoop::abortnotInLoopThread()
{
}

void EventLoop::handleRead()
{
	uint64_t one = 1;
	ssize_t n = ::read(wakeupFd_, &one, sizeof one);
	if (n != sizeof one)
	{
		std::cout << "EventLoop::handleRead() reads " << n << " bytes instead of 8" << std::endl;
	}
}

void EventLoop::doPendingFunctors()
{
	std::vector<Functor> functors;
	callingPendingFunctors_ = true;
	{
		std::lock_guard<std::mutex> lg(mutex_);
		functors.swap(pendingFunctors_);
	}

	for (size_t i = 0; i < functors.size(); ++i)
	{
		functors[i]();
	}
	callingPendingFunctors_ = false;
}
