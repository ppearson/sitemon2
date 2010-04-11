#ifndef SCHEDULER_TEST_THREAD_H
#define SCHEDULER_TEST_THREAD_H

#include "../utils/sqlite_db.h"
#include "../utils/thread.h"
#include "http_engine.h"
#include "scheduler.h"

#include "scheduled_results_saver.h"

class SchedulerTestThread : public Thread
{
public:
	SchedulerTestThread(ScheduledResultSaver *pSaver, SQLiteDB *pDB, TestType testType, unsigned long testID);
	virtual ~SchedulerTestThread() { }
	
	virtual void run();

	void runTest();
	
protected:
	SQLiteDB	*m_pMainDB;
	ScheduledResultSaver	*m_pSaver;

	TestType	m_testType;
	unsigned long	m_testID;

};

#endif
