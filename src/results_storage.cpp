/*
 *  results_storage.cpp
 *  sitemon
 *
 *  Created by Peter Pearson on 02/04/2010.
 *  Copyright 2010 __MyCompanyName__. All rights reserved.
 *
 */

#ifdef _MSC_VER
#include <direct.h>
#endif

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
		char *szCurrentDir = getcwd(NULL, 0);
		if (szCurrentDir == 0)
		{
			printf("can't get current dir - try using a full path\n");
			return false;
		}
		
		std::string strFullPath = szCurrentDir;
#ifdef _MSC_VER
		strFullPath += "\\";
#else
		strFullPath += "/";
#endif
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
	
	for (int step = 1; itStep != m_aResults.end(); ++itStep, step++)
	{
		std::vector<HTTPResponse> &responses = (*itStep).second;
		
		fprintf(fp, "Step %i\n", step);
		fprintf(fp, "Thread, Response code, DNS lookup time, Connection time, Data start time, Total time, Content Size\n");
		
		std::vector<HTTPResponse>::iterator itResp = responses.begin();
		
		for (; itResp != responses.end(); ++itResp)
		{
			HTTPResponse &resp = (*itResp);
			fprintf(fp, "Thread %i, %ld, %f, %f, %f, %f, %ld,\n", resp.m_thread, resp.responseCode, resp.lookupTime, resp.connectTime, resp.dataStartTime,
						resp.totalTime, resp.contentSize);
			
		}		
	}	
	
	fclose(fp);
	
	return true;
}