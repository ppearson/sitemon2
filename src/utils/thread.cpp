/*
 *  Thread.cpp
 *  lrip
 *
 *  Created by Peter Pearson on 21/10/2009.
 *  Copyright 2009 __MyCompanyName__. All rights reserved.
 *
 */

#include "thread.h"

Thread::Thread() : m_thread(NULL), m_isRunning(false), m_autoDestruct(false)
{

}

Thread::~Thread()
{
	stop();
}

void Thread::sleep(int seconds)
{
#ifndef _MSC_VER
	::sleep(seconds);
#else
	::Sleep(seconds * 1000);
#endif
}

#ifdef _MSC_VER
unsigned long __stdcall Thread::threadProc(void *ptr)
#else
void *threadProc(void *ptr)
#endif
{
	((Thread*)ptr)->setRunning(true);
	
	((Thread*)ptr)->run();
	
	((Thread*)ptr)->setRunning(false);

	if (((Thread*)ptr)->shouldAutodestruct())
	{
		delete (Thread*)ptr;
	}
	
	return 0;
}

// TODO: if this is called twice in a row, the handle to the first thread will be lost, but
// the first thread will still run to completion - maybe return a pointer to thread??
bool Thread::start()
{
	m_threadObject = this;

#ifdef _MSC_VER
	unsigned long threadID = 0;
	
	if (m_thread)
		CloseHandle(m_thread);

	m_thread = CreateThread(0, 0, threadProc, m_threadObject, 0, &threadID);
#else
//	pthread_attr_t attr;
//	pthread_attr_init(&attr);
//	pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);

	int ret = pthread_create(&m_thread, NULL, threadProc, (void *)m_threadObject);
	
	if (ret)
	{
		return false;
	}
#endif
	
	setRunning(true);
	
	return true;
}

// TODO: maybe add support for killing the thread while it's running?
void Thread::stop(bool kill)
{
	if (m_thread)
	{
#ifdef _MSC_VER
		CloseHandle(m_thread);
		
		if (kill)
		{
			unsigned long exitCode = 0;
			GetExitCodeThread(m_thread, &exitCode);
			
			if (exitCode == STILL_ACTIVE)
			{
				TerminateThread(m_thread, -1);
			}			
		}
#else

#endif
	}
	
	m_thread = NULL;

	setRunning(false);
}

void Thread::waitForCompletion()
{
	if (!m_isRunning)
		return;

#ifdef _MSC_VER
	WaitForSingleObject(m_thread, INFINITE);
#else
	void *pointer = NULL;
	pthread_join(m_thread, &pointer);
#endif
}

void Thread::setRunning(bool running)
{
	m_mutex.lock();
	m_isRunning = running;
	m_mutex.unlock();
}
