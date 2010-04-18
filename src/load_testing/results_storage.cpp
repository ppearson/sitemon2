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

#include "../utils/misc.h"
#include "results_storage.h"

void ConcurrentHitResults::addResult(int step, HTTPResponse &response)
{
	std::map<int, std::vector<HTTPResponse> >::iterator itStepFind;
	itStepFind = m_aResults.find(step);
	
	if (itStepFind == m_aResults.end())
	{
		std::vector<HTTPResponse> newVector;
		
		newVector.push_back(response);
		
		m_aResults[step] = newVector;		
	}
	else
	{
		(*itStepFind).second.push_back(response);		
	}
}

void ConcurrentHitResults::addResults(std::vector<HTTPResponse> &results)
{
	std::vector<HTTPResponse>::iterator it = results.begin();
	
	for (int step = 1; it != results.end(); ++it, step++)
	{
		addResult(step, *it);		
	}
}

bool ConcurrentHitResults::outputResultsToCSV(std::string path)
{
	if (path.empty())
	{
		return false;
	}
	
	std::string finalPath;
	
	// handle relative paths
#ifdef _MSC_VER
	if (path.find(":") == -1)
#else
	if (path[0] != '/')
#endif
	{
		char *szCurrentDir = getCurrentDirectory();
		if (szCurrentDir == 0)
		{
			printf("can't get current dir - try using a full path\n");
			return false;
		}
		
		std::string strFullPath = szCurrentDir;
		strFullPath += path;
		
		finalPath = strFullPath;
	}
	else
	{
		finalPath = path;
	}
	
	FILE *fp = fopen(finalPath.c_str(), "w+");
	
	if (!fp)
	{
		printf("Can't create output file: %s.\n", finalPath.c_str());
		return false;
	}
	
	std::map<int, std::vector<HTTPResponse> >::iterator itStep = m_aResults.begin();
	
	char szTime[64];
	
	time_t runTime;
	struct tm * pTimeinfo;
	
	for (int step = 1; itStep != m_aResults.end(); ++itStep, step++)
	{
		std::vector<HTTPResponse> &responses = (*itStep).second;
		
		fprintf(fp, "Step %i\n", step);
		fprintf(fp, "Thread, Repeat, Time, Error, Response code, DNS lookup time, Connection time, Data start time, Total time, Content Size\n");
		
		std::vector<HTTPResponse>::iterator itResp = responses.begin();
		
		for (; itResp != responses.end(); ++itResp)
		{
			HTTPResponse &resp = (*itResp);
			
			memset(szTime, 0, 64);
			runTime = resp.timestamp;
			pTimeinfo = localtime(&runTime);
			strftime(szTime, 64, "%H:%M:%S", pTimeinfo);
			
			fprintf(fp, "%i, %i, %s, %ld, %ld, %f, %f, %f, %f, %ld,\n", resp.m_thread, resp.m_repeat, szTime, resp.errorCode, resp.responseCode, resp.lookupTime, resp.connectTime, resp.dataStartTime,
						resp.totalTime, resp.contentSize);
			
		}		
	}	
	
	fclose(fp);
	
	return true;
}