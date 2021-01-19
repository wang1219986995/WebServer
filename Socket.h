#pragma once

class InetAddress;


// ²»¿É¸´ÖÆ
class Socket
{
public:
	explicit Socket(int sockfd) :sockfd_(sockfd) {};
	~Socket();

	int fd() const { return sockfd_; }

	void bindAddress(const InetAddress& localadrr);

	void listen();

	int accept(InetAddress* peeraddr);

	void setReusAddr(bool on);

private:
	const int sockfd_;
};

