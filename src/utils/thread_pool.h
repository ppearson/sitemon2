#ifndef THREAD_POOL_H
#define THREAD_POOL_H

#include <deque>
#include <bitset>

#include "thread.h"
#include "mutex.h"
#include "event.h"

#define MAX_THREADS 16

class ThreadPool;

class Task
{
public:
	Task();
	virtual ~Task() { }
	
	void setThreadPool(ThreadPool *pThreadPool);
				 
protected:
	ThreadPool *	m_pThreadPool;	
};

// assumes events accurately portray whether there are threads available or not
class ThreadController
{
public:
	ThreadController(int threads = 2);
	~ThreadController() { }
	
	int getThread();
	void freeThread(int thread);
	
protected:
	Mutex						m_lock;
	int							m_numberOfThreads;
	std::bitset<MAX_THREADS>	m_bsThreadsAvailable;
	Event						m_threadAvailable;
};

class ThreadPoolThread : public Thread
{
public:
	ThreadPoolThread(Task *pTask, ThreadPool *pThreadPool, int threadID);
	~ThreadPoolThread() { }
	
	virtual void run();
	
protected:
	Task *			m_pTask;
	ThreadPool *	m_pThreadPool;
	int				m_threadID;
};

class ThreadPool
{
public:
	ThreadPool(int threads = 2);
	virtual ~ThreadPool();
	
	void addTask(Task *pTask);
	void startPoolAndWaitForCompletion();
	void processTask(Task *pTask, int threadID);
	
	void freeThread(int threadID);
	
	virtual void doTask(Task *pTask, int threadID) = 0;
	
protected:
	int					m_numberOfThreads;
	ThreadController	m_controller;
	std::deque<Task*>	m_aTasks;
	Mutex				m_lock;	
};

#endif
