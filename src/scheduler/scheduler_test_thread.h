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

#ifndef SCHEDULER_TEST_THREAD_H
#define SCHEDULER_TEST_THREAD_H

#include "../utils/sqlite_db.h"
#include "../utils/thread.h"
#include "../http_engine.h"
#include "scheduler.h"

#include "scheduled_results_saver.h"

class SchedulerTestThread : public Thread
{
public:
	SchedulerTestThread(ScheduledResultsSaver *pSaver, ScheduledItem &item);
	virtual ~SchedulerTestThread() { }
	
	virtual void run();
	
protected:
	SQLiteDB	*m_pMainDB;
	ScheduledResultsSaver	*m_pSaver;

	unsigned long m_testID;

	bool		m_single;

	HTTPRequest	m_request;
	Script		m_script;
};

#endif
