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

#include <ctime>

#include "load_test_results_saver.h"

#include "../utils/misc.h"
#include "load_test_db_helpers.h"

StepResults::StepResults(int step, const std::string &description) : m_step(step), m_description(description)
{
	
}

LoadTestResultsSaver::LoadTestResultsSaver() : m_haveScript(false)
{
	
}

bool LoadTestResultsSaver::initStorage(const std::string &filePath)
{
	m_database = false;
	
	if (filePath.empty())
		return false;
	
	m_filePath = filePath;
	
	m_continualSaving = false;
	
	return true;
}

bool LoadTestResultsSaver::initStorage(const std::string &description, SQLiteDB *pDB)
{
	m_database = true;
	
	if (!pDB)
		return false;
	
	m_pMainDB = pDB;
	
	m_testRunDescription = description;
	
	m_continualSaving = true;	
	
	return initDatabase();
}

bool LoadTestResultsSaver::initDatabase()
{
	// make sure the results tables exists
	if (!createNeededLoadTestTables(m_pMainDB))
		return false;
	
	// create the run record ID
	if (!createRunIDRecord(m_pMainDB, m_testRunDescription, m_loadTestRunID))
		return false;
	
	return true;
}

void LoadTestResultsSaver::run()
{
	while (m_isRunning)
	{
		sleep(10); // sleep for 10 secs
		
		if (m_continualSaving)
		{
			storeResults();
		}
	}
}

// this assumes we've been given all the results
void LoadTestResultsSaver::stop()
{
	m_isRunning = false;
	
	storeResults();
}

void LoadTestResultsSaver::addResult(const ScriptResult &result)
{
	m_lock.lock();

	m_aResults.push_back(result);

	m_lock.unlock();
}

void LoadTestResultsSaver::storeResults()
{
	m_lock.lock();

	if (!m_aResults.empty())
	{
		if (m_database)
		{
			databaseStore();
		}
		else
		{
			fileStore();
		}
	}

	m_lock.unlock();
}

// if Continual Saving is set, we assume we save things progressivly
// for the database, this means we remove items from the results vector after they've been saved
void LoadTestResultsSaver::databaseStore()
{
	// m_continualSaving
	
	if (m_aResults.empty())
		return;
	
	if (!m_pMainDB)
		return;
	
	SQLiteQuery q(*m_pMainDB, true);
	
	// use transactions
	if (!q.execute("BEGIN IMMEDIATE"))
	{
		printf("problem starting results saving transaction\n");
		return;
	}
	
	std::vector<ScriptResult>::iterator itRes = m_aResults.begin();
	std::vector<ScriptResult>::iterator itResEnd = m_aResults.end();
	
	char szTemp[1024];
	
	for (; itRes != itResEnd; ++itRes)
	{
		ScriptResult &result = *itRes;
		
		std::string sql = "insert into load_test_overall_results values (";
		
		memset(szTemp, 0, 1024);
		sprintf(szTemp, "%ld, datetime(%ld, 'unixepoch'), %i, %ld, %i)", m_loadTestRunID, result.getRequestStartTime().get32bitLong(),
				result.getOverallError(), result.getLastResponseCode(), result.getResponseCount());
		
		sql.append(szTemp);
		
		if (q.execute(sql))
		{
			unsigned long overallResultID = q.getInsertRowID();
		
			std::vector<HTTPResponse>::iterator itResponse = result.begin();
			std::vector<HTTPResponse>::iterator itResponseEnd = result.end();
			for (int step = 1; itResponse != itResponseEnd; ++itResponse, step++)
			{
				HTTPResponse &httpResponse = *itResponse;
				
				std::string sqlPageResult = "insert into load_test_page_results values (";
				
				memset(szTemp, 0, 1024);
				sprintf(szTemp, "%ld, %ld, %i, datetime(%ld, 'unixepoch'), '%s', '%s', %i, %ld, %f, %f, %f, %f, %ld, %ld, %ld, %ld, %ld)", m_loadTestRunID, overallResultID, step,
								httpResponse.timestamp.get32bitLong(), httpResponse.requestedURL.c_str(), httpResponse.finalURL.c_str(), httpResponse.errorCode,
								httpResponse.responseCode, httpResponse.lookupTime, httpResponse.connectTime, httpResponse.dataStartTime, httpResponse.totalTime,
								httpResponse.redirectCount, httpResponse.contentSize, httpResponse.downloadSize, httpResponse.totalContentSize, httpResponse.totalDownloadSize);
				sqlPageResult.append(szTemp);
				
				if (!q.execute(sqlPageResult))
				{
					printf("Couldn't insert load test page result into db...\n");
				}					
			}
		}
		else
		{
			printf("Couldn't insert load test overall result into db...\n");
		}
	}
	
	if (!q.execute("COMMIT"))
	{
		printf("problem committing results transaction\n");
		return;
	}
	
	m_aResults.clear();
}

void LoadTestResultsSaver::fileStore()
{
	if (m_filePath.empty())
		return;
	
	std::string finalPath;
	
	// handle relative paths
#ifdef _MSC_VER
	if (m_filePath.find(":") == -1)
#else
	if (m_filePath[0] != '/')
#endif
	{
		char *szCurrentDir = getCurrentDirectory();
		if (szCurrentDir == 0)
		{
			printf("can't get current dir - try using a full path\n");
			return;
		}
		
		std::string strFullPath = szCurrentDir;
		strFullPath += m_filePath;
		
		finalPath = strFullPath;
	}
	else
	{
		finalPath = m_filePath;
	}
	
	FILE *fp = fopen(finalPath.c_str(), "w+");
	
	if (!fp)
	{
		printf("Can't create output file: %s.\n", finalPath.c_str());
		return;
	}
	
	char szTime[64];
	
	time_t runTime;
	struct tm * pTimeinfo;
	
	// if there's only one step, there's no point outputting overall stats for the scripts, we'll just go straight
	// to detailed for that single step
		
	if (m_script.getStepCount() == 1)
	{
		fprintf(fp, "Time, Error, Response code, DNS lookup time, Connection time, Data start time, Total time, Content Size\n");
		
		std::vector<ScriptResult>::iterator itRes = m_aResults.begin();
		std::vector<ScriptResult>::iterator itResEnd = m_aResults.end();
		for (; itRes != itResEnd; ++itRes)
		{
			HTTPResponse &resp = (*itRes).getFirstResponse();
			
			memset(szTime, 0, 64);
			runTime = resp.timestamp.getNativeTime();
			pTimeinfo = localtime(&runTime);
			strftime(szTime, 64, "%H:%M:%S", pTimeinfo);
			
			fprintf(fp, "%s, %i, %ld, %f, %f, %f, %f, %ld,\n", szTime, resp.errorCode, resp.responseCode, resp.lookupTime, resp.connectTime, resp.dataStartTime,
					resp.totalTime, resp.contentSize);				
		}		
	}
	else // otherwise, print a summary of each overall request, then each step separately underneath
	{
		// build up results set on a per-step basis - isn't exactly efficient (cpu, memory reallocation-wise), but it's concise,
		// and it's only for file output, so shouldn't be rediculous amount of results (hopefully)
		
		std::map<int, StepResults> aFullResults;
		
		std::vector<HTTPRequest>::iterator itScriptSteps = m_script.begin();
		std::vector<HTTPRequest>::iterator itScriptStepsEnd = m_script.end();
		for (int step = 1; itScriptSteps != itScriptStepsEnd; ++itScriptSteps, step++)
		{
			StepResults newStep(step, (*itScriptSteps).getDescription());
			
			aFullResults.insert(std::pair<int, StepResults>(step, newStep));
		}
		
		fprintf(fp, "Overall:\nRequest Time, Overall Error, Last Response code, \n");
		
		std::vector<ScriptResult>::iterator itRes = m_aResults.begin();
		std::vector<ScriptResult>::iterator itResEnd = m_aResults.end();
		for (; itRes != itResEnd; ++itRes)
		{
			ScriptResult &result = *itRes;
			
			// add individual results to step-based map of results
			
			std::vector<HTTPResponse>::iterator itResponse = result.begin();
			std::vector<HTTPResponse>::iterator itResponseEnd = result.end();
			for (int step = 1; itResponse != itResponseEnd; ++itResponse, step++)
			{
				HTTPResponse &httpResponse = *itResponse;
				
				std::map<int, StepResults>::iterator itFind = aFullResults.find(step);
				
				if (itFind != aFullResults.end())
				{
					StepResults &storageStep = (*itFind).second;
					
					storageStep.addResult(httpResponse);
				}				
			}
			
			memset(szTime, 0, 64);
			runTime = result.getRequestStartTime().getNativeTime();
			pTimeinfo = localtime(&runTime);
			strftime(szTime, 64, "%H:%M:%S", pTimeinfo);
			
			fprintf(fp, "%s, %i, %ld,\n", szTime, result.getOverallError(), result.getLastResponseCode());		
		}
		
		fprintf(fp, "Step, Time, Error, Response code, DNS lookup time, Connection time, Data start time, Total time, Content Size\n");
		
		// now print all the detailed responses for each step
		std::map<int, StepResults>::iterator itStepResults = aFullResults.begin();
		std::map<int, StepResults>::iterator itStepResultsEnd = aFullResults.end();
		for (; itStepResults != itStepResultsEnd; ++itStepResults)
		{
			StepResults &stepResults = (*itStepResults).second;
			
			if (stepResults.hasResults())
			{
				std::vector<HTTPResponse>::iterator itResponses = stepResults.begin();
				std::vector<HTTPResponse>::iterator itResponsesEnd = stepResults.end();
				for (; itResponses != itResponsesEnd; ++itResponses)
				{
					HTTPResponse &resp = (*itResponses);
					
					memset(szTime, 0, 64);
					runTime = resp.timestamp.getNativeTime();
					pTimeinfo = localtime(&runTime);
					strftime(szTime, 64, "%H:%M:%S", pTimeinfo);
					
					fprintf(fp, "%i, %s, %i, %ld, %f, %f, %f, %f, %ld,\n", stepResults.m_step, szTime, resp.errorCode, resp.responseCode, resp.lookupTime, resp.connectTime,
							resp.dataStartTime, resp.totalTime, resp.contentSize);				
				}
			}
		}
	}		
	
	fclose(fp);
}