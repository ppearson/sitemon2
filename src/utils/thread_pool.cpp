
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
		m_threadAvailable.reset();
		
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
	
//	m_threadAvailable.reset();
	m_threadAvailable.set();
}

ThreadPoolThread::ThreadPoolThread(Task *pTask, ThreadPool *pThreadPool, int threadID) : m_pTask(pTask),
									m_pThreadPool(pThreadPool), m_threadID(threadID)
{
	m_autoDestruct = true;
}

void ThreadPoolThread::run()
{
	// call ThreadPool subclass's doTask method
	if (m_pThreadPool)
		m_pThreadPool->doTask(m_pTask, m_threadID);
	
	// thread's task is finished, so clean up Task and free thread so it can be used for another task
	
	delete m_pTask;
	
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

void ThreadPool::startPoolAndWaitForCompletion()
{
	ThreadPoolThread *m_pThreads[MAX_THREADS];
	
	int threadID = -1;
	while (!m_aTasks.empty())
	{
		threadID = m_controller.getThread();
		
		if (threadID != -1) // might want to break out here
		{
			m_lock.lock();
			
			Task *pTask = m_aTasks.front();
			
			if (!pTask)
				continue;
			
			m_pThreads[threadID] = new ThreadPoolThread(pTask, this, threadID);
			
			if (m_pThreads[threadID])
			{
				m_pThreads[threadID]->start();
			}
			
			m_aTasks.pop_front();			
			
			m_lock.unlock();			
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
}