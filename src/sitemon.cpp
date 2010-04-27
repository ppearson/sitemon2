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

#include "http_engine.h"
#include "sitemon.h"
#include "load_testing/hit_test_engine.h"
#include "load_testing/profile_test_engine.h"
#include "html_parser.h"
#include "load_testing/load_test_results_saver.h"

bool performSingleRequest(HTTPRequest &request, bool outputHeader)
{
	HTTPEngine engine;
	HTTPResponse response;

	if (engine.performRequest(request, response))
	{
		if (outputHeader)
		{
			std::cout << response.header << "\n";
		}

		outputResponse(request, response);		
	}
	else
	{
		std::cout << response.errorString << "\n";

		return false;
	}

	return true;
}

bool performScriptRequest(Script &script)
{
	HTTPEngine engine;

	for (std::vector<HTTPRequest>::iterator it = script.begin(); it != script.end(); ++it)
	{
		HTTPRequest &request = *it;
		HTTPResponse response;

		std::cout << "Testing... " << request.getDescription() << "\n";

		if (engine.performRequest(request, response))
		{
			outputResponse(request, response);
		}
		else
		{
			std::cout << "Error code: " << response.errorCode << "\t:\t" << response.errorString << "\n";
			break;
		}
	}

	return true;
}

bool performHitLoadTest(HTTPRequest &request, int threads, const std::string &outputPath)
{
	LoadTestResultsSaver saver(false, outputPath);
	
	if (!saver.initStorage())
		return false;
	
	HitTestEngine engine;
	engine.initialise(request, threads);
	
	engine.setResultsSaver(&saver);
	
	saver.start();	
	if (engine.start())
	{
		if (!outputPath.empty())
		{
			saver.stop();
		}
	}
	
	return true;
}

bool performHitLoadTest(Script &script, int threads, const std::string &outputPath)
{
	LoadTestResultsSaver saver(false, outputPath);
	
	if (!saver.initStorage())
		return false;
	
	HitTestEngine engine;
	engine.initialise(script, threads);
	
	engine.setResultsSaver(&saver);
	
	saver.start();	
	if (engine.start())
	{
		if (!outputPath.empty())
		{
			saver.stop();
		}
	}
	
	return true;
}

bool performHitLoadTest(Script &script, const std::string &outputPath)
{
	if (!script.hasLoadTestSettings())
		return false;
	
	LoadTestResultsSaver saver(false, outputPath);
	
	if (!saver.initStorage())
		return false;
	
	LoadTestSettings &ltSettings = script.getLoadTestSettings();
	
	HitTestEngine engine;
	engine.initialise(script, ltSettings.getHitThreads(), ltSettings.getHitRepeats());
	
	engine.setResultsSaver(&saver);
	
	saver.start();
	if (engine.start())
	{
		if (!outputPath.empty())
		{
			saver.stop();
		}
	}
	
	return true;
}

bool performProfileLoadTest(HTTPRequest &request, int threads, int duration, const std::string &outputPath)
{
	LoadTestResultsSaver saver(false, outputPath);
	
	if (!outputPath.empty() && !saver.initStorage())
		return false;
	
	ProfileTestEngine engine;
	engine.setResultsSaver(&saver);
	
	engine.initialise(request, 2);
	engine.addProfileSegment(threads, duration);
	
	saver.start();
	engine.start();
	
	saver.stop();
	
	return true;
}

bool performProfileLoadTest(Script &script, int threads, int duration, const std::string &outputPath)
{
	LoadTestResultsSaver saver(false, outputPath);
	
	if (!outputPath.empty() && !saver.initStorage())
		return false;

	ProfileTestEngine engine;
	engine.setResultsSaver(&saver);

	engine.initialise(script);
	engine.addProfileSegment(threads, duration);
	
	saver.start();
	engine.start();

	saver.stop();
	
	return true;
}

bool performProfileLoadTest(Script &script, const std::string &outputPath)
{
	LoadTestSettings &settings = script.getLoadTestSettings();
	
	if (settings.m_type != LOAD_PROFILE_TEST)
		return false;
	
	LoadTestResultsSaver saver(false, outputPath);
	
	if (!saver.initStorage())
		return false;
	
	ProfileTestEngine engine;
	engine.setResultsSaver(&saver);
	
	engine.initialise(script);
	
	// add the segments
	std::vector<LoadTestProfileSeg>::iterator itSeg = settings.begin();
	for (; itSeg != settings.end(); ++itSeg)
	{
		engine.addProfileSegment((*itSeg).m_threads, (*itSeg).m_duration);
	}
	
	saver.start();
	engine.start();
	
	saver.stop();
	
	return true;
}

void outputResponse(HTTPRequest &request, HTTPResponse &response)
{
	std::cout << "Final URL:\t\t" << response.finalURL << "\n";
	std::cout << "Respone code:\t\t" << response.responseCode << "\n\n";

	std::cout << "DNS Lookup:\t\t" << response.lookupTime << " seconds.\n";
	std::cout << "Connection:\t\t" << response.connectTime << " seconds.\n";
	std::cout << "Data start:\t\t" << response.dataStartTime << " seconds.\n";

	if (response.redirectCount)
	{
		std::cout << "Redirect count:\t\t" << response.redirectCount << ".\n";
		std::cout << "Redirect time:\t\t" << response.redirectTime << " seconds.\n";
	}

//	std::cout << "Data transfer:\t\t" << response.dataTransferTime << " seconds.\n";
	std::cout << "Total time:\t\t" << response.totalTime << " seconds.\n\n";
	
	std::cout << "HTML Content size:\t" << response.contentSize << "\n";
	std::cout << "HTML Download size:\t" << response.downloadSize << "\n";

	if (response.contentSize > response.downloadSize)
	{
		int compression = 100 - (int)(((double)response.downloadSize / response.contentSize) * 100.0);

		std::cout << "Compression Savings:\t" << compression << "%\n";
		std::cout << "Content Encoding:\t" << response.contentEncoding << "\n";
	}
	
	if (request.getDownloadContent())
	{	
		std::cout << "Total Content size:\t" << response.totalContentSize << "\n";
		std::cout << "Total Download size:\t" << response.totalDownloadSize << "\n";
		
		if (response.componentProblem)
		{
			std::cout << "Issues downloading one or more components:\n";
			
			const std::vector<HTTPComponentResponse> &components = response.getComponents();
			
			std::vector<HTTPComponentResponse>::const_iterator it = components.begin();
			for (; it != components.end(); ++it)
			{
				const HTTPComponentResponse &compResponse = *it;
				
				if (compResponse.errorCode != HTTP_OK || compResponse.responseCode != 200)
				{
					printf("%i\t%ld\t%s\n", compResponse.errorCode, compResponse.responseCode, compResponse.requestedURL.c_str());					
				}				
			}
			
			
		}
	}
}
