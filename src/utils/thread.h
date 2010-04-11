#ifndef THREAD_H
#define THREAD_H

#ifndef _MSC_VER
#include <pthread.h>
#include <unistd.h>
#else
#include <windows.h>
#endif

#include "Mutex.h"

class Thread
{
public:
	Thread();
	
	virtual ~Thread();

	static void sleep(int seconds);

	bool start();
	void stop(bool kill = false);
	void waitForCompletion();
	void setRunning(bool running);

	void setAutodestruct(bool autoDestruct) { m_autoDestruct = autoDestruct; }
	bool shouldAutodestruct() { return m_autoDestruct; }

	virtual void run() = 0;

protected:
#ifdef _MSC_VER
	static unsigned long __stdcall threadProc(void *ptr);
#else
	friend void *threadProc(void *ptr);
#endif

#ifdef _MSC_VER
	HANDLE m_thread;
#else
	pthread_t m_thread;
#endif

	Thread *m_threadObject;
	bool m_isRunning;
	bool m_autoDestruct;
	Mutex m_mutex;
};

#endif
