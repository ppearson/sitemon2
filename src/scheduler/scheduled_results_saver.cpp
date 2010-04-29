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

#include "scheduled_results_saver.h"

ScheduledResult::ScheduledResult(HTTPResponse &response, unsigned long testID) : m_response(response), m_testType(SINGLE_TEST), m_testID(testID)
{
	
}

ScheduledResult::ScheduledResult(ScriptResult &scriptResult, unsigned long testID) : m_scriptResult(scriptResult), m_testType(SCRIPT_TEST), m_testID(testID)
{
	
}

ScheduledResultSaver::ScheduledResultSaver(SQLiteDB *pDB) : m_pMainDB(pDB)
{
	
}

void ScheduledResultSaver::run()
{
	while (m_isRunning)
	{
		sleep(30); // sleep for 30 secs
		
		storeResults();
	}
}

void ScheduledResultSaver::addResult(HTTPResponse &response, unsigned long testID)
{
	m_mutex.lock();
	
	ScheduledResult newResult(response, testID);
	
	m_aSingleResults.push_back(newResult);
	
	m_mutex.unlock();	
}

void ScheduledResultSaver::addResult(ScriptResult &scriptResult, unsigned long testID)
{
	m_mutex.lock();
	
	ScheduledResult newResult(scriptResult, testID);
	
	m_aScriptResults.push_back(newResult);
	
	m_mutex.unlock();	
}

void ScheduledResultSaver::storeResults()
{
	m_mutex.lock();
	
	if (m_aSingleResults.empty() && m_aScriptResults.empty())
	{
		m_mutex.unlock();
		return;
	}
	
	SQLiteQuery q(*m_pMainDB, true);

	std::vector<ScheduledResult>::iterator it = m_aSingleResults.begin();
	
	// use transactions
	if (!q.execute("BEGIN IMMEDIATE"))
	{
		printf("problem starting results saving transaction\n");
		m_mutex.unlock();
		return;
	}
	
	char szTemp[1024];
	for (; it != m_aSingleResults.end();)
	{
		ScheduledResult &result = *it;
		
		std::string sql = "insert into scheduled_single_test_results values (";
		
		memset(szTemp, 0, 1024);
		sprintf(szTemp, "%ld, datetime(%ld, 'unixepoch'), %ld, %ld, %f, %f, %f, %f, %ld, %ld, %ld, %ld, %ld)", result.m_testID, result.m_response.timestamp, result.m_response.errorCode,
				result.m_response.responseCode, result.m_response.lookupTime, result.m_response.connectTime, result.m_response.dataStartTime, result.m_response.totalTime,
				result.m_response.redirectCount, result.m_response.contentSize, result.m_response.downloadSize, result.m_response.componentContentSize, result.m_response.componentDownloadSize);
		
		sql.append(szTemp);
		
		if (q.execute(sql))
		{
			long runID = q.getInsertRowID();
			
			// save component results if needed
			
			const std::vector<HTTPComponentResponse> &components = result.m_response.getComponents();
			
			std::vector<HTTPComponentResponse>::const_iterator itComponent = components.begin();
			for (; itComponent != components.end(); ++itComponent)
			{
				const HTTPComponentResponse &compResult = *itComponent;
				std::string sqlComponentResults = "insert into scheduled_single_test_component_results values(";
				memset(szTemp, 0, 1024);
				sprintf(szTemp, "%ld, %ld, %ld, %ld, '%s', %f, %f, %f, %f, %ld, %ld)", result.m_testID, runID, compResult.errorCode, compResult.responseCode, compResult.requestedURL.c_str(),
						compResult.lookupTime, compResult.connectTime, compResult.dataStartTime, compResult.totalTime, compResult.contentSize, compResult.downloadSize);
				sqlComponentResults.append(szTemp);
				
				q.execute(sqlComponentResults);	
			}
		
			// we can delete it now, although we don't know for certain that the commit was successful...
						
			it = m_aSingleResults.erase(it);			
		}
		else
		{
			// todo: need to see if the error wasn't busy and if so probably delete it, but for now just get the next one

			++it;
		}
	}
	
	if (!q.execute("COMMIT"))
	{
		printf("problem committing results transaction\n");
		m_mutex.unlock();
		return;
	}
	
	m_mutex.unlock();
}
