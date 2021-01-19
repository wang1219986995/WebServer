#pragma once
#include <functional>
#include "Channel.h"





class EventLoop;
class InetAddress;


// ²»¿É¸´ÖÆ
class Acceptor
{
public:
	typedef std::function<void(int sockfd, const InetAddress&)> NewConnectionCallback;

	Acceptor(EventLoop* loop, const InetAddress& listenAddr);
	Acceptor(const Acceptor&) = delete;

	void setNewConnectionCallback(const New)




private:
	void handleRead();

	EventLoop* loop_;
	Socket acceptSocket_;
	Channel acceptChannel_;
	NewConnectionCallback newConnectionCallback_;
	bool listening_;
};

