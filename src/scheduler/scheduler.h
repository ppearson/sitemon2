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

#include "../http_request.h"
#include "../script.h"
#include "../utils/time.h"
#include "../utils/thread.h"

#include "../utils/sqlite_db.h"

#include "scheduled_results_saver.h"

class ScheduledItem
{
public:
	ScheduledItem() { }
	ScheduledItem(bool single, unsigned long id, const std::string &description, unsigned long interval, Time & currentTime);
	~ScheduledItem() { }
	
	void setEnabled(bool enabled) { m_enabled = enabled; }
	void setTestID(unsigned long testID) { m_testID = testID; }
	void setInterval(unsigned long interval) { m_interval = interval; }
	void setDescription(std::string &description) { m_description = description; }

	void setRequest(const HTTPRequest &request) { m_request = request; }
	void setScript(const Script &script) { m_script = script; }
	
	void setModifiedTime(Time &time) { m_modifiedTime = time; }
	void setModifiedTime(unsigned long timestamp) { m_modifiedTime = timestamp; }
	
	void incrementNextTime();
	
	unsigned long	getID() const { return m_id; }
	bool			isEnabled() const { return m_enabled; }
	unsigned long	getInterval() const { return m_interval; }
	std::string		getDescription() const { return m_description; }
	Time			getNextTime() const { return m_nextTime; }
	unsigned long	getTestID() const { return m_testID; }

	bool			isSingle() const { return m_single; }

	HTTPRequest & getRequest() { return m_request; }
	Script & getScript() { return m_script; }
	
	Time			getModifiedTime() { return m_modifiedTime; }
	
protected:
	unsigned long	m_id;
	bool			m_enabled;
	unsigned long	m_interval;
	std::string		m_description;
	
	Time			m_nextTime;
		
	unsigned long	m_testID;

	bool			m_single;
	Time			m_modifiedTime;

	HTTPRequest		m_request;
	Script			m_script;
};

class Scheduler : public Thread
{
public:
	Scheduler(SQLiteDB *pDB);
	virtual ~Scheduler();
	
	virtual void run();
	
	void buildScheduledItemsFromDB(SQLiteDB *pDB);

	ScheduledResultsSaver *	getScheduledResultsSaver() { return m_pSaver; }

protected:
	SQLiteDB	*m_pMainDB;
	
	Mutex		m_scheduledItemsLock;
	std::vector<ScheduledItem> m_aScheduledSingleItems;
	std::vector<ScheduledItem> m_aScheduledScriptItems;
	
	ScheduledResultsSaver	*	m_pSaver;
};

#endif
