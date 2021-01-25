#include "Thread.h"
#include <sys/syscall.h>
#include <unistd.h>


#include <assert.h>





struct ThreadData 
{
public:
	typedef Thread::ThreadFunc ThreadFunc;

	ThreadData(const ThreadFunc& func, const std::shared_ptr<pid_t>& tid)
		:func_(func),wkptr_(tid)
	{}

	ThreadFunc func_;
	std::weak_ptr<pid_t> wkptr_;


	void runInThread()
	{
		pid_t tid = static_cast<pid_t>(::syscall(SYS_gettid));
		std::shared_ptr<pid_t> tid_ptr = wkptr_.lock();
		if (tid_ptr)
		{
			*tid_ptr = tid;
			tid_ptr.reset();
			func_();
		}
	}
};


void* startThread(void* obj)
{
	ThreadData* data = static_cast<ThreadData*>(obj);
	data->runInThread();
	delete data;
	return NULL;
}





Thread::Thread(const ThreadFunc& func):
	started_(false), joined_(false),
	pthreadId_(0), tid_(new pid_t(0)),
	func_(func)
{


}

Thread::~Thread()
{
	if (started_ && !joined_)
	{
		pthread_detach(pthreadId_);
	}

}

void Thread::start()
{
	assert(!started_);
	started_ = true;
	ThreadData* data = new ThreadData(func_, tid_);
	if (pthread_create(&pthreadId_, NULL, &startThread, data))
	{
		started_ = false;
		delete data;
		abort();
	}

}

void Thread::join()
{
}
