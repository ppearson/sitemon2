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

#include "hit_test_engine.h"

#include "../http_engine.h"
#include "hit_load_request_thread.h"
#include "../html_parser.h"

HitTestEngine::HitTestEngine() : m_pSaver(NULL)
{
	
}

bool HitTestEngine::initialise(Script &script, int numberOfThreads, int repeats)
{
	m_pScript = &script;
	
	m_hitTestType = HIT_SCRIPT;
	m_numberOfThreads = numberOfThreads;
	m_repeats = repeats;
	
	return true;
}

bool HitTestEngine::initialise(const HTTPRequest &request, int numberOfThreads, int repeats)
{
	m_pRequest = &request;
	
	m_hitTestType = HIT_REQUEST;
	m_numberOfThreads = numberOfThreads;
	m_repeats = repeats;
	
	return true;
}

bool HitTestEngine::start()
{
	// don't know if we even need this any more
#ifdef _MSC_VER
	thread_setup();
#endif
	
	// if it's a single request type, we need to create a temp Script object
	// to pass to the threads, then delete it later on
	
	if (m_hitTestType == HIT_REQUEST)
	{
		if (!m_pRequest) // shouldn't happen, but...
		{
			return false;
		}
		
		m_pScript = new Script(m_pRequest);
	}
	
	// copy script over, so load test results saver knows about each step
	if (m_pSaver && m_pScript)
	{
		m_pSaver->copyScript(*m_pScript);
	}
	
	std::vector<HitLoadRequestThread *> aThreads;
	
	int threadCount = 0;
	
	for (int i = 0; i < m_numberOfThreads; i++)
	{
		RequestThreadData *data = new RequestThreadData(i + 1, m_pScript, m_pSaver, m_repeats);
	
		HitLoadRequestThread *newThread = new HitLoadRequestThread(data);
		
		if (newThread)
		{
			threadCount++;
			
			aThreads.push_back(newThread);
		}
	}
	
	printf("Created %i threads...\n", threadCount);
	
	std::vector<HitLoadRequestThread *>::iterator it = aThreads.begin();
	for (; it != aThreads.end(); ++it)
	{
		(*it)->start();
	}
	
	for (it = aThreads.begin(); it != aThreads.end(); ++it)
	{
		(*it)->waitForCompletion();
		delete *it;
	}
	
	if (m_hitTestType == HIT_REQUEST && m_pScript)
	{
		delete m_pScript;
	}
	
	printf("All threads finished.\n");
	
#ifdef _MSC_VER
	thread_cleanup();
#endif
	
	return true;
}

#ifdef _MSC_VER

void win32_locking_callback(int mode, int type, char *file, int line)
{
	if (mode & CRYPTO_LOCK)
	{
		WaitForSingleObject(lock_cs[type], INFINITE);
	}
	else
	{
		ReleaseMutex(lock_cs[type]);
	}
}

void thread_setup()
{
	int i;
	
	lock_cs = (void**)OPENSSL_malloc(CRYPTO_num_locks() * sizeof(HANDLE));
	for (i = 0; i < CRYPTO_num_locks(); i++)
	{
		lock_cs[i] = CreateMutex(NULL, FALSE, NULL);
	}
	
	CRYPTO_set_locking_callback((void (*)(int, int, const char *, int))win32_locking_callback);
}

void thread_cleanup()
{
	int i;
	
	CRYPTO_set_locking_callback(NULL);
	for (i = 0; i < CRYPTO_num_locks(); i++)
		CloseHandle(lock_cs[i]);
	
	OPENSSL_free(lock_cs);
}

static unsigned long id_function(void)
{
	return ((unsigned long)GetCurrentThreadId());
}

#endif
