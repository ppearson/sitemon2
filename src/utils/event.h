#ifndef EVENT_H
#define EVENT_H

#ifndef _MSC_VER
#include <pthread.h>
#else
#include <windows.h>
#endif

class Event
{
public:
	Event();
	~Event();
	
	void set();
	void wait();
	void reset();
	
protected:
#ifdef _MSC_VER
	HANDLE m_event;	
#else
	pthread_cond_t m_ready;
	pthread_mutex_t m_lock;	
#endif	
	
};

#endif
