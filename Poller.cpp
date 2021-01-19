#include "Poller.h"
#include "Channel.h"
Poller::Poller(EventLoop* loop) :ownerLoop_(loop)
{
}

Poller::~Poller()
{
}

Timestamp Poller::poll(int timeoutMs, ChannelList* activateChannels)
{
	int numEvents = ::poll(&*pollfds_.begin(), pollfds_.size(), timeoutMs);

	// Timestamp 是获取系统时间的类
	Timestamp now(Timestamp::now());
	std::cout << pollfds_.size() << std::endl;
	if (numEvents > 0) {
		std::cout << numEvents << "events happended ";
		fillActivateChannels(numEvents, activateChannels);
	}
	else if (numEvents == 0) {
		std::cout << "nothing hapended." << std::endl;
	}

	return now;
}

void Poller::updateChannel(Channel* channel)
{
	// 这个函数没定义啊
	assertInLoopThread();
	if (channel->index() < 0) {
		// Channel初始化时index_置为-1，此时是一个新对象
		assert(channels_.find(channel->fd()) == channels_.end());
		struct pollfd pfd;


		pfd.fd = channel->fd();
		// 为什么是  size（） - 1  ？？？ 
		pfd.events = static_cast<short>(channel->events());
		pfd.revents = 0;

		pollfds_.push_back(pfd);
		int idx = static_cast<int>(pollfds_.size()) - 1;
		channel->set_index(idx);
		channels_[pfd.fd] = channel;

	}
	else {
		// 对已有Channel进行更新
		assert(channels_.find(channel->fd()) != channels_.end());
		assert(channels_[channel->fd()] == channel);
		int idx = channel->index();
		assert(0 <= idx && idx < static_cast<int>(pollfds_.size()));
		struct pollfd& pfd = pollfds_[idx];
		assert(pfd.fd == channel->fd() || pfd.fd == -1);
		pfd.events = static_cast<short>(channel->events());
		pfd.revents = 0;
		if (channel->isNoneEvent()) {
			pfd.fd = -1;
		}

	}
}

void Poller::assertInLoopThread()
{
	std::cout << "There is in assertInLoopThread and not do anything!" << std::endl;
	ownerLoop_->assertInLoopThread();
}

void Poller::fillActivateChannels(int numEvents, ChannelList* activateChannels) const
{
	for (PollFdList::const_iterator pfd = pollfds_.begin(); pfd != pollfds_.end() && numEvents > 0; ++pfd) 
	{
		if (pfd->revents > 0)
		{
			// 找出所有活动事件，毕竟将其填入 activateChannels中，活动时间的总数为numEvents
			--numEvents;
			ChannelMap::const_iterator ch = channels_.find(pfd->fd);
			assert(ch != channels_.end());
			Channel* channel = ch->second;
			assert(channel->fd() == pfd->fd);
			channel->set_revents(pfd->revents);
			activateChannels->push_back(channel);
		}
	}
}



