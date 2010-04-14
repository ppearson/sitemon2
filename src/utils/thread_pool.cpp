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

#include "thread_pool.h"

Task::Task()
{
	
}

void Task::setThreadPool(ThreadPool *pThreadPool)
{
	m_pThreadPool = pThreadPool;
}

ThreadController::ThreadController(int threads) : m_numberOfThreads(threads)
{
	// set the number of available threads to 1 (available) in the bitset - the remainder
	// are 0
	
	m_bsThreadsAvailable.reset();

	m_threadAvailable.reset();
	m_threadAvailable.set();
	
	for (int i = 0; i < m_numberOfThreads; i++)
	{
		m_bsThreadsAvailable.set(i, true);
	}
}

int ThreadController::getThread()
{
	int thread = -1;
	
	// first see if there's already a free one
	m_lock.lock();
	
	if (m_bsThreadsAvailable.any())
	{
		for (int i = 0; i < m_numberOfThreads; i++)
		{
			if (m_bsThreadsAvailable.test(i))
			{
				thread = i;
				m_bsThreadsAvailable.set(i, false);
				break;
			}
		}
		
		m_lock.unlock();
	}
	else
	{
		m_lock.unlock();
		
//		
		m_threadAvailable.wait();
				
		m_lock.lock();
		
		// find the thread that's available
		
		for (int i = 0; i < m_numberOfThreads; i++)
		{
			if (m_bsThreadsAvailable.test(i))
			{
				thread = i;
				m_bsThreadsAvailable.set(i, false);
				break;
			}
		}
		
		m_threadAvailable.reset();
		
		m_lock.unlock();
	}
	
	return thread;
}

void ThreadController::freeThread(int thread)
{
	if (thread < 0 || thread >= m_numberOfThreads)
		return;
	
	m_lock.lock();
	
	m_bsThreadsAvailable.set(thread, true);
	
	m_lock.unlock();
	
	m_threadAvailable.set();
}

ThreadPoolThread::ThreadPoolThread(Task *pTask, ThreadPool *pThreadPool, int threadID) : m_pTask(pTask),
									m_pThreadPool(pThreadPool), m_threadID(threadID)
{
	m_autoDestruct = true;
}

void ThreadPoolThread::run()
{
	// while we have a task
	while (m_pTask && m_pThreadPool)
	{
		// call ThreadPool subclass's doTask method
		m_pThreadPool->doTask(m_pTask, m_threadID);

		delete m_pTask;

		// to save constantly creating and destroying loads of threads, try and reuse them if there's work to do...
		m_pTask = m_pThreadPool->getNextTask();
	}

	// otherwise, free the thread		
	m_pThreadPool->freeThread(m_threadID);
}

ThreadPool::ThreadPool(int threads) : m_controller(threads), m_numberOfThreads(threads)
{
	
}

ThreadPool::~ThreadPool()
{
	
}

void ThreadPool::addTask(Task *pTask)
{
	m_lock.lock();
	
	pTask->setThreadPool(this);
	
	m_aTasks.push_back(pTask);
	
	m_lock.unlock();
}

Task *ThreadPool::getNextTask()
{
	Task *pTask = NULL;

	m_lock.lock();

	if (!m_aTasks.empty())
	{
		pTask = m_aTasks.front();
		m_aTasks.pop_front();
	}

	m_lock.unlock();

	return pTask;
}

void ThreadPool::startPoolAndWaitForCompletion()
{
	int threadID = -1;
	while (!m_aTasks.empty())
	{
		threadID = m_controller.getThread();
		
		if (threadID != -1)
		{
			m_lock.lock();

			if (!m_aTasks.empty())
			{
				Task *pTask = m_aTasks.front();
				
				if (!pTask)
					continue;
				
				m_pThreads[threadID] = new ThreadPoolThread(pTask, this, threadID);
				
				if (m_pThreads[threadID])
				{
					m_pThreads[threadID]->start();
				}
				
				m_aTasks.pop_front();
			}
			
			m_lock.unlock();			
		}
		else
		{
			// something weird happened - we shouldn't have got here, but we often do...
			
			Thread::sleep(1);
		}
	}
	
	// now need to make sure any active threads have finished before we go out of scope
	
	for (int i = 0; i < m_numberOfThreads; i++)
	{
		if (m_pThreads[i])
		{
			m_pThreads[i]->waitForCompletion();
		}
	}	
}

void ThreadPool::freeThread(int threadID)
{
	m_controller.freeThread(threadID);
	
	// clean up the thread handle so we don't bother waiting on it
	
	m_pThreads[threadID] = NULL;
}