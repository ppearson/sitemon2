#include <stdio.h>

#include "scheduler.h"
#include "scheduler_db_helpers.h"
#include "scheduler_test_thread.h"

#define TIME_MULTIPLIER 60 // needs to be 60 for minutes

Scheduler::Scheduler(SQLiteDB *pDB) : m_pMainDB(pDB)
{
	
}

Scheduler::~Scheduler()
{
	
}

void Scheduler::run()
{
	m_pSaver = new ScheduledResultSaver(m_pMainDB);
	
	if (m_pSaver)
	{
		m_pSaver->start();
	}
	
	createNeededSchedulerTables(m_pMainDB);
	
	buildScheduledItemsFromDB(m_pMainDB);	
	
	int count = 0;
	while (m_isRunning)
	{
		m_scheduledItemsLock.lock();
		
		count ++;
		
		time_t timeNow;
		time(&timeNow);

		std::vector<ScheduledItem>::iterator it = m_aScheduledItems.begin();
		for (; it != m_aScheduledItems.end(); ++it)
		{
			ScheduledItem &item = *it;
			
			if (item.isEnabled() && item.getNextTime() < timeNow)
			{
				printf("Firing off: %ld\t%s\n", item.getID(), item.getDescription().c_str());

				SchedulerTestThread *pNewTest = new SchedulerTestThread(m_pSaver, m_pMainDB, item.getTestType(), item.getTestID());

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
		updateScheduledSingleTests(m_pMainDB, m_aScheduledItems);
		m_scheduledItemsLock.unlock();

		sleep(1);
	}
	
	if (m_pSaver)
	{
		delete m_pSaver;
	}
	
	if (m_pMainDB)
		delete m_pMainDB;
}

void Scheduler::buildScheduledItemsFromDB(SQLiteDB *pDB)
{
	m_scheduledItemsLock.lock();

	m_aScheduledItems.clear();
	getScheduledSingleTestsFromDB(pDB, m_aScheduledItems);
	
	m_scheduledItemsLock.unlock();
}

ScheduledItem::ScheduledItem(unsigned long id, const std::string &description, unsigned long interval, unsigned long currentTime) : 
								m_id(id), m_description(description), m_interval(interval), m_enabled(true)
{
	m_nextTime = currentTime + (interval * TIME_MULTIPLIER);
}

void ScheduledItem::incrementNextTime()
{
	m_nextTime += (m_interval * TIME_MULTIPLIER);
}
