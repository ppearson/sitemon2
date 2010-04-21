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
#include "load_testing/results_storage.h"
#include "html_parser.h"

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

bool performHitLoadTestRequest(HTTPRequest &request, int threads, const std::string &outputPath)
{
	HitTestEngine engine;
	engine.initialise(request, threads);
	
	if (engine.start())
	{
		if (!outputPath.empty())
		{
			ConcurrentHitResults &results = engine.getResults();
			
			results.outputResultsToCSV(outputPath);
		}
	}
	
	return true;
}

bool performHitLoadTestScriptRequest(Script &script, int threads, const std::string &outputPath)
{
	HitTestEngine engine;
	engine.initialise(script, threads);
	
	if (engine.start())
	{
		if (!outputPath.empty())
		{
			ConcurrentHitResults &results = engine.getResults();
			
			results.outputResultsToCSV(outputPath);
		}
	}
	
	return true;
}

bool performProfileLoadTest(HTTPRequest &request, int duration, int threads, const std::string &outputPath)
{
	ProfileTestEngine engine;
	
	engine.initialise(request);
	engine.addProfileSegment(threads, duration);
	
	engine.start();
	
	return true;
}

bool performProfileLoadTest(Script &script, int duration, int threads, const std::string &outputPath)
{
	ProfileTestEngine engine;
	
	engine.initialise(script);
	engine.addProfileSegment(threads, duration);
	
	engine.start();
	
	return true;
}

bool performProfileLoadTest(Script &script, const std::string &outputPath)
{
	// need to load profile details from script
	
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
