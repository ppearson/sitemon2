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

ScheduledResult::ScheduledResult(const HTTPResponse &response, unsigned long testID) : m_response(response), m_testType(SINGLE_TEST), m_testID(testID)
{
	
}

ScheduledResult::ScheduledResult(const ScriptResult &scriptResult, unsigned long testID) : m_scriptResult(scriptResult), m_testType(SCRIPT_TEST), m_testID(testID)
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

void ScheduledResultSaver::addResult(const HTTPResponse &response, unsigned long testID)
{
	m_mutex.lock();
	
	ScheduledResult newResult(response, testID);
	
	m_aSingleResults.push_back(newResult);
	
	m_mutex.unlock();	
}

void ScheduledResultSaver::addResult(const ScriptResult &scriptResult, unsigned long testID)
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
	
	storeSingleResults();
	storeScriptResults();
		
	m_mutex.unlock();
}

void ScheduledResultSaver::storeSingleResults()
{
	SQLiteQuery q(*m_pMainDB, true);
	
	std::vector<ScheduledResult>::iterator it = m_aSingleResults.begin();
	
	// use transactions
	if (!q.execute("BEGIN IMMEDIATE"))
	{
		printf("problem starting results saving transaction\n");
		return;
	}
	
	char szTemp[1024];
	for (; it != m_aSingleResults.end();)
	{
		ScheduledResult &result = *it;
		
		std::string sql = "insert into scheduled_single_test_results values (";
		
		memset(szTemp, 0, 1024);
		sprintf(szTemp, "%ld, datetime(%ld, 'unixepoch'), %i, %ld, %f, %f, %f, %f, %ld, %ld, %ld, %ld, %ld)", result.m_testID, result.m_response.timestamp.get32bitLong(), result.m_response.errorCode,
				result.m_response.responseCode, result.m_response.lookupTime, result.m_response.connectTime, result.m_response.dataStartTime, result.m_response.totalTime,
				result.m_response.redirectCount, result.m_response.contentSize, result.m_response.downloadSize, result.m_response.componentContentSize, result.m_response.componentDownloadSize);
		
		sql.append(szTemp);
		
		if (q.execute(sql))
		{
			long runID = q.getInsertRowID();
			
			// save component results if needed
			
			const std::vector<HTTPComponentResponse> &components = result.m_response.getComponents();
			
			std::vector<HTTPComponentResponse>::const_iterator itComponent = components.begin();
			std::vector<HTTPComponentResponse>::const_iterator itComponentEnd = components.end();
			for (; itComponent != itComponentEnd; ++itComponent)
			{
				const HTTPComponentResponse &compResult = *itComponent;
				std::string sqlComponentResults = "insert into scheduled_single_test_component_results values(";
				memset(szTemp, 0, 1024);
				sprintf(szTemp, "%ld, %ld, %i, %ld, '%s', %f, %f, %f, %f, %ld, %ld)", result.m_testID, runID, compResult.errorCode, compResult.responseCode, compResult.requestedURL.c_str(),
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
		return;
	}
}

void ScheduledResultSaver::storeScriptResults()
{
	SQLiteQuery q(*m_pMainDB, true);
	
	std::vector<ScheduledResult>::iterator it = m_aScriptResults.begin();
	
	// use transactions
	if (!q.execute("BEGIN IMMEDIATE"))
	{
		printf("problem starting results saving transaction\n");
		return;
	}
	
	char szTemp[1024];
	for (; it != m_aScriptResults.end();)
	{
		ScheduledResult &result = *it;
		
		std::string sql = "insert into scheduled_script_test_results values (";
		
		memset(szTemp, 0, 1024);
		sprintf(szTemp, "%ld, datetime(%ld, 'unixepoch'), %i, %ld, %i)", result.m_testID, result.m_scriptResult.getRequestStartTime().get32bitLong(), result.m_scriptResult.getOverallError(),
				result.m_scriptResult.getLastResponseCode(), result.m_scriptResult.getResponseCount());
		
		sql.append(szTemp);
		
		if (q.execute(sql))
		{
			long runID = q.getInsertRowID();
			
			// now save individual results
			
			std::vector<HTTPResponse>::const_iterator itResponse = result.m_scriptResult.begin();
			std::vector<HTTPResponse>::const_iterator itResponseEnd = result.m_scriptResult.end();
			for (int page = 1; itResponse != itResponseEnd; ++itResponse, page++)
			{
				const HTTPResponse &resp = *itResponse;
				std::string sqlPageResults = "insert into scheduled_script_test_page_results values(";
				memset(szTemp, 0, 1024);
				sprintf(szTemp, "%ld, %ld, %i, datetime(%ld, 'unixepoch'), '%s', %i, %ld, %f, %f, %f, %f, %ld, %ld, %ld, %ld, %ld)", result.m_testID, runID, page, resp.timestamp.get32bitLong(), resp.requestedURL.c_str(),
						resp.errorCode, resp.responseCode, resp.lookupTime, resp.connectTime, resp.dataStartTime, resp.totalTime, resp.redirectCount, resp.contentSize, resp.downloadSize,
						resp.componentContentSize, resp.componentDownloadSize);
				sqlPageResults.append(szTemp);
				
				if (!q.execute(sqlPageResults))
				{
					printf("Problem saving individual page test result for script...\n");
				}
			}
			
			// we can delete it now, although we don't know for certain that the commit was successful...
			
			it = m_aScriptResults.erase(it);			
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
		return;
	}
}
