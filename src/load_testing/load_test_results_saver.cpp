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

#include "load_test_results_saver.h"

#include "../utils/misc.h"

StepResults::StepResults(int step, const std::string &description) : m_step(step), m_description(description)
{
	
}

LoadTestResultsSaver::LoadTestResultsSaver(bool continualSaving, const std::string &filePath) : m_continualSaving(continualSaving), m_filePath(filePath),
						m_haveScript(false)
{
	m_database = false;
}

LoadTestResultsSaver::LoadTestResultsSaver(bool continualSaving, SQLiteDB *pDB) : m_continualSaving(continualSaving), m_pMainDB(pDB), m_haveScript(false)
{
	m_database = true;
}

bool LoadTestResultsSaver::initStorage()
{
	if (m_database)
	{
		// init the database
		
		if (!m_pMainDB)
			return false;		
	}
	else
	{
		if (m_filePath.empty())
			return false;
	}
	
	return true;
}

void LoadTestResultsSaver::run()
{
	while (m_isRunning)
	{
		sleep(5); // sleep for 5 secs
		
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

void LoadTestResultsSaver::addResult(ScriptResult &result)
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
		for (; itRes != m_aResults.end(); ++itRes)
		{
			HTTPResponse &resp = (*itRes).getFirstResponse();
			
			memset(szTime, 0, 64);
			runTime = resp.timestamp;
			pTimeinfo = localtime(&runTime);
			strftime(szTime, 64, "%H:%M:%S", pTimeinfo);
			
			fprintf(fp, "%s, %ld, %ld, %f, %f, %f, %f, %ld,\n", szTime, resp.errorCode, resp.responseCode, resp.lookupTime, resp.connectTime, resp.dataStartTime,
					resp.totalTime, resp.contentSize);				
		}		
	}
	else // otherwise, print a summary of each overall request, then each step separately underneath
	{
		// build up results set on a per-step basis - isn't exactly efficient (cpu, memory reallocation-wise), but it's concise,
		// and it's only for file output, so shouldn't be rediculous amount of results (hopefully)
		
		std::map<int, StepResults> aFullResults;
		
		std::vector<HTTPRequest>::iterator itScriptSteps = m_script.begin();
		for (int step = 1; itScriptSteps != m_script.end(); ++itScriptSteps, step++)
		{
			StepResults newStep(step, (*itScriptSteps).getDescription());
			
			aFullResults.insert(std::pair<int, StepResults>(step, newStep));
		}
		
		fprintf(fp, "Overall:\nRequest Time, Overall Error, Last Response code, \n");
		
		std::vector<ScriptResult>::iterator itRes = m_aResults.begin();
		for (; itRes != m_aResults.end(); ++itRes)
		{
			ScriptResult &result = *itRes;
			
			// add individual results to step-based map of results
			
			std::vector<HTTPResponse>::iterator itResponse = result.begin();
			for (int step = 1; itResponse != result.end(); ++itResponse, step++)
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
			runTime = result.getRequestStartTime();
			pTimeinfo = localtime(&runTime);
			strftime(szTime, 64, "%H:%M:%S", pTimeinfo);
			
			fprintf(fp, "%s, %i, %ld,\n", szTime, result.getOverallError(), result.getLastResponseCode());		
		}
		
		// now print all the detailed responses for each step
		std::map<int, StepResults>::iterator itStepResults = aFullResults.begin();
		for (; itStepResults != aFullResults.end(); ++itStepResults)
		{
			StepResults &stepResults = (*itStepResults).second;
			
			if (stepResults.hasResults())
			{
				fprintf(fp, "Step %i, %s\n", stepResults.m_step, stepResults.m_description.c_str());
				fprintf(fp, "Time, Error, Response code, DNS lookup time, Connection time, Data start time, Total time, Content Size\n");
				
				std::vector<HTTPResponse>::iterator itResponses = stepResults.begin();
				for (; itResponses != stepResults.end(); ++itResponses)
				{
					HTTPResponse &resp = (*itResponses);
					
					memset(szTime, 0, 64);
					runTime = resp.timestamp;
					pTimeinfo = localtime(&runTime);
					strftime(szTime, 64, "%H:%M:%S", pTimeinfo);
					
					fprintf(fp, "%s, %i, %ld, %f, %f, %f, %f, %ld,\n", szTime, resp.errorCode, resp.responseCode, resp.lookupTime, resp.connectTime, resp.dataStartTime,
							resp.totalTime, resp.contentSize);				
				}
			}
		}
	}		
	
	fclose(fp);
}