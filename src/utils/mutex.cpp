/*
 *  Mutex.cpp
 *  lrip
 *
 *  Created by Peter Pearson on 21/10/2009.
 *  Copyright 2009 __MyCompanyName__. All rights reserved.
 *
 */

#include "mutex.h"

Mutex::Mutex() : m_created(true)
{
#ifdef _MSC_VER
	m_mutex = CreateMutex(NULL, FALSE, NULL);
	
	if (!m_mutex)
		m_created = false;
#else
	pthread_mutexattr_t attr;
	pthread_mutexattr_init(&attr);
	pthread_mutex_init(&m_mutex, &attr);
#endif
}

Mutex::~Mutex()
{
	if (m_created)
	{
#ifdef _MSC_VER
		WaitForSingleObject(m_mutex, INFINITE);
		CloseHandle(m_mutex);
#else
		pthread_mutex_lock(&m_mutex);
		pthread_mutex_unlock(&m_mutex);
		pthread_mutex_destroy(&m_mutex);
#endif
	}
}

void Mutex::lock()
{
#ifdef _MSC_VER
	WaitForSingleObject(m_mutex, INFINITE);
#else
	pthread_mutex_lock(&m_mutex);
#endif
}

void Mutex::unlock()
{
#ifdef _MSC_VER
	ReleaseMutex(m_mutex);
#else
	pthread_mutex_unlock(&m_mutex);
#endif
}