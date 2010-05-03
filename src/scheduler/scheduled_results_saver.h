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

#ifndef SCHEDULED_RESULTS_SAVER_H
#define SCHEDULED_RESULTS_SAVER_H

#include <string>
#include <vector>

#include "../http_engine.h"
#include "../utils/thread.h"
#include "../utils/sqlite_query.h"
#include "../script_result.h"

enum TestType
{
	SINGLE_TEST,
	SCRIPT_TEST	
};

class ScheduledResult
{
public:
	ScheduledResult(const HTTPResponse &response, unsigned long testID);
	ScheduledResult(const ScriptResult &scriptResult, unsigned long testID);
	
	TestType	m_testType;
	unsigned long	m_testID;
	
	HTTPResponse	m_response;
	ScriptResult	m_scriptResult;
};


class ScheduledResultSaver : public Thread
{
public:
	ScheduledResultSaver(SQLiteDB *pDB);
	virtual ~ScheduledResultSaver() { }
	
	virtual void run();
	
	void addResult(const HTTPResponse &response, unsigned long testID);
	void addResult(const ScriptResult &scriptResult, unsigned long testID);
	
	void storeResults();
	void storeSingleResults();
	void storeScriptResults();
	
protected:
	
	std::vector<ScheduledResult>	m_aSingleResults;
	std::vector<ScheduledResult>	m_aScriptResults;
	Mutex							m_mutex;
	
	SQLiteDB *						m_pMainDB;	
};

#endif
