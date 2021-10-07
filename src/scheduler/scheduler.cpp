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

#include <stdio.h>

#include "scheduler.h"
#include "scheduler_db_helpers.h"
#include "scheduler_test_thread.h"

#define TIME_MULTIPLIER 60 // needs to be 60 for minutes

Scheduler::Scheduler(SQLiteDB *pDB) : m_pMainDB(pDB)
{
	m_pSaver = new ScheduledResultsSaver(m_pMainDB);
}

Scheduler::~Scheduler()
{
	if (m_pSaver)
	{
		delete m_pSaver;
	}
}

void Scheduler::run()
{
	if (m_pSaver)
	{
		m_pSaver->start();
	}
	
	buildScheduledItemsFromDB(m_pMainDB);	
	
	while (m_isRunning)
	{
		// strictly speaking, these mutexes aren't needed, as this function is the only thing which controls
		// the vectors of ScheduledItems. However, they guarentee that the processor doesn't start doing some
		// out-of-order execution (however unlikely) which could mess things up... 2021: Really?! :)
		m_scheduledItemsLock.lock();
		
		Time timeNow;
		timeNow.now();

		std::vector<ScheduledItem>::iterator it = m_aScheduledSingleItems.begin();
		for (; it != m_aScheduledSingleItems.end(); ++it)
		{
			ScheduledItem &item = *it;
			
			if (item.isEnabled() && item.getNextTime() < timeNow)
			{
//				printf("Firing off single test: %ld\t%s\n", item.getID(), item.getDescription().c_str());

				SchedulerTestThread *pNewTest = new SchedulerTestThread(m_pSaver, item);
				if (pNewTest)
				{
					pNewTest->start();
				}
				
				item.incrementNextTime();	
			}
		}
		
		////
		
		it = m_aScheduledScriptItems.begin();
		for (; it != m_aScheduledScriptItems.end(); ++it)
		{
			ScheduledItem &item = *it;
			
			if (item.isEnabled() && item.getNextTime() < timeNow)
			{
//				printf("Firing off script test: %ld\t%s\n", item.getID(), item.getDescription().c_str());
				
				SchedulerTestThread *pNewTest = new SchedulerTestThread(m_pSaver, item);
				if (pNewTest)
				{
					pNewTest->start();
				}
				
				item.incrementNextTime();				
			}
		}
		
		m_scheduledItemsLock.unlock();
		
		sleep(19);
		
		m_scheduledItemsLock.lock();
		updateScheduledSingleTests(m_pMainDB, m_aScheduledSingleItems);
		updateScheduledScriptTests(m_pMainDB, m_aScheduledScriptItems);
		m_scheduledItemsLock.unlock();

		sleep(1);
	}
}

void Scheduler::buildScheduledItemsFromDB(SQLiteDB *pDB)
{
	m_scheduledItemsLock.lock();

	m_aScheduledSingleItems.clear();
	getScheduledSingleTestsFromDB(pDB, m_aScheduledSingleItems);
	
	m_aScheduledScriptItems.clear();
	getScheduledScriptTestsFromDB(pDB, m_aScheduledScriptItems);
	
	m_scheduledItemsLock.unlock();
}

ScheduledItem::ScheduledItem(bool single, unsigned long id, const std::string &description, unsigned long interval, Time & currentTime) : 
								m_single(single), m_id(id), m_description(description), m_interval(interval), m_enabled(true)
{
	m_nextTime = currentTime;
	m_nextTime.incrementMinutes(interval);
}

void ScheduledItem::incrementNextTime()
{
	m_nextTime.incrementMinutes(m_interval);
}

