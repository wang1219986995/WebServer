#pragma once
#include <vector>
#include <sys/time.h>
#include <poll.h>
#include <map>


#include "EventLoop.h"



class Channel;

class Poller
{
public:
	typedef std::vector<struct pollfd> PollFdList;
	typedef std::map<int, Channel*> ChannelMap;

	Poller(EventLoop* loop);

	//纯虚函数的析构函数作用是什么？
	//virtual ~Poller();
	~Poller();


	timeval poll(int timeoutMs, ChannelList *activateChannels);

	void updateChannel(Channel* channel);

	void removeChannel(Channel* channel);


	void assertInLoopThread() const
	{
		ownerLoop_->assertInLoopThread();
	}



private:
	void fillActivateChannels(int numEvents, ChannelList* activateChannels) const;


	EventLoop* ownerLoop_;
	PollFdList pollfds_;

	// 这个map是干啥用的
	ChannelMap channelmaps_;




};




