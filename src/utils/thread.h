/*
 Sitemon
 Copyright 2010 Peter Pearson.
 
 Licensed under the Apache License, Version 2.0 (the "License");
 You may not use this file except in compliance with the License.
 You may obtain a copy of the License at
 
 http://www.apache.org/licenses/LICENSE-2.0
 
 Unless required by applicable law or agreed to in writing, software
 distributed under the License is distributed on an "AS IS" BASIS,
 WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 See the License for the specific language governing permissions and
 limitations under the License.
 
 */

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
