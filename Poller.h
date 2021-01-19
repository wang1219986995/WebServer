#pragma once
#ifndef MUDUO_POLLER
#define MUDUO_POLLER

#include <poll.h>
#include <map>
#include <iostream>
#include <assert.h>
#include "Timestamp.h"
#include <vector>

#include "EventLoop.h"
struct pollfd;

class Channel;



class Poller
{
public:
	typedef std::vector<Channel*> ChannelList;

	Poller(EventLoop* loop);
	Poller(const Poller&) = delete;
	~Poller();

	Timestamp poll(int timeoutMs, ChannelList* activateChannels);
	void updateChannel(Channel* channel);
	void assertInLoopThread();

private:
	void fillActivateChannels(int numEvents, ChannelList* activateChannels) const;
	typedef std::vector<struct pollfd> PollFdList;
	typedef std::map<int, Channel*> ChannelMap;

	EventLoop *ownerLoop_;
	PollFdList pollfds_;
	ChannelMap channels_;

};


#endif // !MUDUO_POLLER




