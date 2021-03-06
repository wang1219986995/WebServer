#include "Poller.h"
#include "Channel.h"

#include <iostream>
#include <assert.h>


Poller::Poller(EventLoop* loop):
	ownerLoop_(loop)
{

}

Poller::~Poller()
{
}

timeval Poller::poll(int timeoutMs, ChannelList *activateChannels)
{	
	int numEvents = ::poll(&*pollfds_.begin(), pollfds_.size(), timeoutMs);

	timeval now;
	gettimeofday(&now, NULL);
	std::cout << pollfds_.size() << std::endl;
	if (numEvents > 0) {
		std::cout << numEvents << "events happended ";
		fillActivateChannels(numEvents, activateChannels);
	}
	else if (numEvents == 0) {
		std::cout << "nothing hapended." << std::endl;
	}

	return now;
	return timeval();
}

void Poller::updateChannel(Channel* channel)
{
	assertInLoopThread();

	if (channel->index() < 0) {
		// 这是个新channel,添加
		assert( channelmaps_.find(channel->fd() ) == channelmaps_.end()  );
		struct pollfd pfd;
		pfd.fd = channel->fd();
		pfd.events = static_cast<short>(channel->events());
		pfd.revents = 0;

		pollfds_.push_back(pfd);
		int idx = static_cast<int>(pollfds_.size()) - 1;
		channel->set_index(idx);
		channelmaps_[pfd.fd] = channel;
	}
	else
	{
		// 这是个已存在channel，更新它
		assert(channelmaps_.find(channel->fd()) == channelmaps_.end());
		assert(channelmaps_[channel->fd()] == channel);
		int idx = channel->index();
		assert(0 <= idx && idx < static_cast<int>(pollfds_.size()));

		struct pollfd& pfd = pollfds_[idx];
		assert(pfd.fd == channel->fd() || pfd.fd == -channel->fd() -1);
		pfd.events = static_cast<short>(channel->events());
		pfd.revents = 0;
		if (channel->isNoneEvent())
		{
			pfd.fd = -1;
		}

	}
}

void Poller::removeChannel(Channel* channel)
{
	assertInLoopThread();
	assert(channelmaps_.find(channel->fd()) != channelmaps_.end());
	assert(channelmaps_[channel->fd()] == channel);
	assert(channel->isNoneEvent());

	int idx = channel->index();
	assert(0 <= idx && idx < static_cast<int>(pollfds_.size()));
	const struct pollfd& pfd = pollfds_[idx]; (void)pfd;

	// 为什么有负号呢
	assert(pfd.fd == -channel->fd() - 1 && pfd.events == channel->events());


	size_t n = channelmaps_.erase(channel->fd());
	assert(n == 1); void(n);
	if (static_cast<size_t>(idx) == pollfds_.size() - 1)
	{
		pollfds_.pop_back();
	}
	else
	{
		int channelAtEnd = pollfds_.back().fd;
		std::iter_swap(pollfds_.begin() + idx, pollfds_.end() - 1);
		if (channelAtEnd < 0)
		{
			channelAtEnd = -channelAtEnd - 1;
		}
		channelmaps_[channelAtEnd]->set_index(idx);
		pollfds_.pop_back();
	}
}



void Poller::fillActivateChannels(int numEvents, ChannelList *activateChannels) const
{
	for (PollFdList::const_iterator pfd = pollfds_.begin();
		pfd != pollfds_.end(); ++pfd)
	{
		if (pfd->revents > 0)
		{
			--numEvents;
			ChannelMap::const_iterator ch = channelmaps_.find(pfd->fd);
			assert(ch != channelmaps_.end());
			Channel* channel = ch->second;
			assert(channel->fd() == pfd->fd);
			channel->set_revents(pfd->revents);
			activateChannels->push_back(channel);

		}
	}
}
