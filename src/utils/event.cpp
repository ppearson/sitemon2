
#include "event.h"

Event::Event()
{
#ifdef _MSC_VER
	m_event = CreateEvent(NULL, FALSE, FALSE, NULL);
#else
	pthread_mutexattr_t mutexAttr;
	pthread_mutexattr_init(&mutexAttr);
	pthread_mutex_init(&m_lock, &mutexAttr);
//	pthread_cond_init(&m_ready, 0);
#endif
}

Event::~Event()
{
#ifdef _MSC_VER
	CloseHandle(m_event);
#else
	pthread_cond_destroy(&m_ready);
	pthread_mutex_destroy(&m_lock);
#endif
}

void Event::set()
{
#ifdef _MSC_VER
	SetEvent(m_event);
#else
	pthread_cond_signal(&m_ready);
#endif
}

void Event::wait()
{
#ifdef _MSC_VER
	WaitForSingleObject(m_event, INFINITE);
#else
	pthread_mutex_lock(&m_lock);
	pthread_cond_wait(&m_ready, &m_lock);
#endif
}

void Event::reset()
{
#ifndef _MSC_VER
	pthread_mutex_unlock(&m_lock);
#endif
}
