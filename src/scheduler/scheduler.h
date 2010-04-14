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

#ifndef SCHEDULER_H
#define SCHEDULER_H

#include <string>
#include <vector>

#include <ctime>

#include "../utils/thread.h"

#include "../utils/sqlite_db.h"

#include "scheduled_results_saver.h"

class ScheduledItem
{
public:
	ScheduledItem() { }
	ScheduledItem(unsigned long id, const std::string &description, unsigned long interval, unsigned long currentTime);
	~ScheduledItem() { }
	
	void setEnabled(bool enabled) { m_enabled = enabled; }
	void setTestType(TestType type) { m_testType = type; }
	void setTestID(unsigned long testID) { m_testID = testID; }
	void setInterval(unsigned long interval) { m_interval = interval; }
	void setDescription(std::string &description) { m_description = description; }
	
	void incrementNextTime();
	
	unsigned long	getID() const { return m_id; }
	bool			isEnabled() const { return m_enabled; }
	unsigned long	getInterval() const { return m_interval; }
	std::string		getDescription() const { return m_description; }
	unsigned long	getNextTime() const { return m_nextTime; }
	TestType		getTestType() const { return m_testType; }
	unsigned long	getTestID() const { return m_testID; }
	
protected:
	unsigned long	m_id;
	bool			m_enabled;
	unsigned long	m_interval;
	std::string		m_description;
	
	unsigned long	m_nextTime;
	
	TestType		m_testType;
	
	unsigned long	m_testID;	
};

class Scheduler : public Thread
{
public:
	Scheduler(SQLiteDB *pDB);
	virtual ~Scheduler();
	
	virtual void run();
	
	void buildScheduledItemsFromDB(SQLiteDB *pDB);

protected:
	SQLiteDB	*m_pMainDB;
	Mutex		m_scheduledItemsLock;
	std::vector<ScheduledItem> m_aScheduledItems;
	
	ScheduledResultSaver	*	m_pSaver;
};

#endif
