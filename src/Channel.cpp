#include "Channel.h"
#include "EventLoop.h"


#include <iostream>
#include <poll.h>

const int Channel::kNoneEvent = 0;
const int Channel::kReadEvent = POLLIN | POLLPRI;
const int Channel::kWriteEvent = POLLOUT;


Channel::Channel(EventLoop* loop, int fd):loop_(loop),fd_(fd),
events_(0),revents_(0),index_(-1)
{
}

Channel::~Channel()
{
}

void Channel::handleEvent()
{

	// 需要完善，没有完全写完
	if (revents_ & POLLNVAL)
	{
		std::cout << "Channel::handleEvent() POLLNVAL" << std::endl;

	}
	
	if (revents_ & (POLLERR | POLLNVAL))
	{
		//
	}

	if (revents_ & (POLLIN | POLLPRI | POLLRDHUP))
	{
		if (readCallback_)  readCallback_();
	}
	
	if (revents_ & POLLOUT)
	{

	}
		
}


void Channel::update()
{
	loop_->updateChannel(this);
}
