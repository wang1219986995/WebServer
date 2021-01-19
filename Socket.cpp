#include "Socket.h"
#include <sys/socket.h>
#include <unistd.h>

Socket::~Socket()
{
	close(sockfd_);
}

void Socket::bindAddress(const InetAddress& localadrr)
{
	bind(sockfd_,add.get)
}

void Socket::listen()
{
}

int Socket::accept(InetAddress* peeraddr)
{
	return 0;
}

void Socket::setReusAddr(bool on)
{
}
