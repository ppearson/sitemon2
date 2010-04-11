#ifndef MUTEX_H
#define MUTEX_H

#ifndef _MSC_VER
#include <pthread.h>
#else
#include <windows.h>
#endif

class Mutex
{
public:
	Mutex();
	~Mutex();

	void lock();
	void unlock();

protected:
#ifdef _MSC_VER
	HANDLE m_mutex;
#else
	pthread_mutex_t m_mutex;
#endif
	bool m_created;

};

class Guard
{
public:
	Guard()
	{
		m_mutex.lock();
	}
	
	~Guard()
	{
		m_mutex.unlock();
	}
	
protected:
	Mutex m_mutex;
};

#endif
