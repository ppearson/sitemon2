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

#include "profile_test_engine.h"

ProfileSegment::ProfileSegment(int concurrentRequests, int duration) : m_concurrentRequests(concurrentRequests), m_duration(duration)
{

}

ProfileTestEngine::ProfileTestEngine(bool smooth) : m_pRequest(NULL), m_pScript(NULL), m_debugging(false), m_pSaver(NULL)
{
	if (smooth)
	{
		m_testStyle = PROFILE_SMOOTH;
	}
	else
	{
		m_testStyle = PROFILE_STEP;
	}
}

ProfileTestEngine::ProfileTestEngine(int concurrentRequests, int duration, bool debugging) : m_pRequest(NULL), m_pScript(NULL), m_debugging(debugging),
									m_pSaver(NULL)
{
	addProfileSegment(concurrentRequests, duration);
	
	m_testStyle = PROFILE_STEP;
}

ProfileTestEngine::~ProfileTestEngine()
{
	
}

bool ProfileTestEngine::initialise(Script &script)
{
	m_pScript = &script;
	
	m_isScript = true;

	return true;
}

bool ProfileTestEngine::initialise(HTTPRequest &request, int sleep)
{
	m_pRequest = &request;
	request.setPauseTime(sleep);
	
	m_isScript = false;
	
	return true;
}

void ProfileTestEngine::addProfileSegment(int concurrentRequests, int duration)
{
	ProfileSegment newSegment(concurrentRequests, duration);
	
	m_aProfileSegments.push_back(newSegment);
}

// Timeframe counts exist as a count of the number of threads that
// should be running for every minute the test should run for
void ProfileTestEngine::setupTimeframeCounts()
{
	m_runLength = 0;
	m_maxNumberOfThreads = 0;
	
	std::vector<ProfileSegment>::iterator it = m_aProfileSegments.begin();
	for (; it != m_aProfileSegments.end(); ++it)
	{
		ProfileSegment &segment = *it;
		
		if (segment.m_duration == 0)
			continue;
		
		m_runLength += segment.m_duration;
		
		if (segment.m_concurrentRequests > m_maxNumberOfThreads)
			m_maxNumberOfThreads = segment.m_concurrentRequests;
		
		for (int i = 0; i < segment.m_duration; i++)
		{
			m_aTimeframeCounts.push_back(segment.m_concurrentRequests);			
		}		
	}
}

bool ProfileTestEngine::start()
{
	setupTimeframeCounts();
	
	if (m_runLength == 0 || m_maxNumberOfThreads == 0)
		return false;
	
	// copy script over, so load test results saver knows about each step
	if (m_pSaver && m_pScript)
	{
		m_pSaver->copyScript(*m_pScript);
	}
	
	if (m_debugging)
		printf("Starting load test with length: %i and max threads: %i\n", m_runLength, m_maxNumberOfThreads);
	
	if (!m_isScript) // if we weren't given a script, create one out of the single request
	{
		if (!m_pRequest) // shouldn't happen, but...
		{
			return false;
		}
		
		m_pScript = new Script(m_pRequest);

		if (m_pSaver)
		{
			m_pSaver->copyScript(*m_pScript);
		}
	}
	
	m_numberOfActiveThreads = 0;
	
	int minute = 0;
	
	m_aThreads.reserve(m_maxNumberOfThreads);
	
	while (true && minute < m_runLength)
	{
		int thisTargetCount = m_aTimeframeCounts[minute];
		
		if (m_debugging)
			printf("Active threads:\t%i\t\tTarget:\t%i\n", m_numberOfActiveThreads, thisTargetCount);
		
		int diff = thisTargetCount - m_numberOfActiveThreads;
		
		if (diff > 0) // need to create more threads
		{
			for (int i = m_numberOfActiveThreads; i < thisTargetCount; i++)
			{
				ProfileThreadData *pNewData = new ProfileThreadData(i, m_pScript, m_pSaver, false);
				
				if (pNewData)
				{
					ProfileLoadRequestThread *pNewThread = new ProfileLoadRequestThread(pNewData);
					
					if (pNewThread)
					{
						m_aThreads[i] = pNewThread;
						
						pNewThread->start();
						
						m_numberOfActiveThreads ++;
					}
				}				
			}			
		}
		else if (diff < 0) // need to kill off threads
		{
			int unneededTopIndex = m_numberOfActiveThreads;
			// stop the threads that aren't needed for the next minute timeframe from the lowest unneeded one
			for (int i = m_numberOfActiveThreads + diff; i < unneededTopIndex; i++)
			{
				ProfileLoadRequestThread *pThread = m_aThreads[i];
				
				if (pThread)
				{
					// make the thread shutdown after it's finished this Script run
					// this will also autoDestruct the thread so we don't need to delete it
					pThread->shutdownThreadAndKill();
					
					m_numberOfActiveThreads --; // maybe this should be done as a callback...					
				}				
			}			
		}
		
		minute ++;
		
		Thread::sleep(60);
	}
	
	// now we need to wait for remaining threads to end, and then kill them off
	// we'll do this as two separate loops, so that all threads will be told to stop
	// as soon as posible
	
	int i = 0;
	for (; i < m_maxNumberOfThreads; i++)
	{
		ProfileLoadRequestThread *pThisThread = m_aThreads[i];
		
		if (pThisThread)
		{
			pThisThread->tellThreadToStop();
		}
	}
	
	for (i = 0; i < m_maxNumberOfThreads; i++)
	{
		ProfileLoadRequestThread *pThisThread = m_aThreads[i];
		
		if (pThisThread)
		{
			pThisThread->waitForCompletion();
			
			delete pThisThread;
			pThisThread = NULL;
		}
	}

	if (!m_isScript && m_pScript)
	{
		delete m_pScript;
	}

	if (m_debugging)
		printf("All threads finished.\n");
	
	return true;	
}

#ifdef _MSC_VER

static void win32_locking_callback(int mode, int type, char *file, int line)
{
	if (mode & CRYPTO_LOCK)
	{
		WaitForSingleObject(lock_cs1[type], INFINITE);
	}
	else
	{
		ReleaseMutex(lock_cs1[type]);
	}
}

static void thread_setup()
{
	int i;
	
	lock_cs1 = (void**)OPENSSL_malloc(CRYPTO_num_locks() * sizeof(HANDLE));
	for (i = 0; i < CRYPTO_num_locks(); i++)
	{
		lock_cs1[i] = CreateMutex(NULL, FALSE, NULL);
	}
	
	CRYPTO_set_locking_callback((void (*)(int, int, const char *, int))win32_locking_callback);
}

static void thread_cleanup()
{
	int i;
	
	CRYPTO_set_locking_callback(NULL);
	for (i = 0; i < CRYPTO_num_locks(); i++)
		CloseHandle(lock_cs1[i]);
	
	OPENSSL_free(lock_cs1);
}

static unsigned long id_function(void)
{
	return ((unsigned long)GetCurrentThreadId());
}

#endif