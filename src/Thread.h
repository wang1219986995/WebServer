#pragma once
#include <functional>
#include <memory>
#include <pthread.h>
class Thread
{
public:
	typedef std::function<void()> ThreadFunc;

	explicit Thread(const ThreadFunc& func);
	~Thread();

	void start();
	void join();

	bool started() { return started_; }

	pid_t tid() const { return *tid_; }
	




private:
	bool started_;
	bool joined_;
	pthread_t pthreadId_;
	std::shared_ptr<pid_t> tid_;
	ThreadFunc func_;

};

