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

#include "http_server/http_server.h"
#include "utils/socket.h"
#include "scheduler/scheduler.h"

#include "scheduler/scheduler_db_helpers.h"
#include "http_server/http_server_db_helpers.h"
#include "load_testing/load_test_db_helpers.h"

void SitemonApp::loadConfigSettings()
{
	Config configFile(m_configSettings);
#ifndef _MSC_VER
	configFile.loadConfigFile("/Users/peter/sm_config.xml");
#else
	configFile.loadConfigFile();
#endif
	
}

bool SitemonApp::runWebServerAndScheduler()
{
	std::string webContentPath = m_configSettings.m_webContentPath;
	std::string monitoringDBPath = m_configSettings.m_monitoringDBPath;
	std::string loadTestingDBPath = m_configSettings.m_loadTestingDBPath;
	
	SQLiteDB *pMonitoringDB = NULL;
	
	if (!monitoringDBPath.empty())
	{
		pMonitoringDB = new SQLiteDB(monitoringDBPath);
	}
	
	if (!pMonitoringDB->isThreadSafe())
	{
		std::cout << "SQLite is not thread safe!\n";
	}

	SQLiteDB *pLoadTestingDB = NULL;
	
	if (!loadTestingDBPath.empty())
	{
		pLoadTestingDB = new SQLiteDB(loadTestingDBPath);
	}
	
	// create the needed tables first
	createNeededHTTPServerTables(pMonitoringDB);
	createNeededSchedulerTables(pMonitoringDB);

	if (pLoadTestingDB)
	{
		createNeededLoadTestTables(pLoadTestingDB);
	}

	Thread::sleep(1); // to enable db tables to be created if needed
	
	Scheduler schedulerThread(pMonitoringDB);
	schedulerThread.start();
	
	std::cout << "Scheduler thread started...\n";
	
	int port = m_configSettings.m_webServerPort;
		
	std::cout << "Starting web server on http://localhost:" << port << "/...\n";
	
	Socket::initWinsocks();		
	
	HTTPServer server(webContentPath, pMonitoringDB, pLoadTestingDB, port);
	// keep in mind this halts execution, by design
	server.start();
	
	Socket::cleanupWinsocks();
	
	if (pMonitoringDB)
		delete pMonitoringDB;

	if (pLoadTestingDB)
		delete pLoadTestingDB;
	
	return true;
}

bool SitemonApp::performSingleRequest(HTTPRequest &request, bool outputHeader)
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

bool SitemonApp::performScriptRequest(Script &script)
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

bool SitemonApp::performHitLoadTest(HTTPRequest &request, int threads, const std::string &outputPath)
{
	LoadTestResultsSaver saver;
	
	SQLiteDB *pLoadTestingDB = NULL;
	
	HitTestEngine engine;
	
	if (!outputPath.empty())
	{
		if (outputPath == ":db")
		{
			std::string loadTestingDBPath = m_configSettings.m_loadTestingDBPath;
			
			if (loadTestingDBPath.empty())
			{
				std::cout << "No load testing DB path specified in Sitemon's config file...\n";
				return false;
			}			
			
			pLoadTestingDB = new SQLiteDB(loadTestingDBPath);
			saver.initStorage("", pLoadTestingDB);
		}
		else
		{
			saver.initStorage(outputPath);
		}

		engine.setResultsSaver(&saver);
	}	
	
	engine.initialise(request, threads);
	
	saver.start();	
	if (engine.start())
	{
		if (!outputPath.empty())
		{
			saver.stop();
		}
	}
	
	if (pLoadTestingDB)
		delete pLoadTestingDB;
	
	return true;
}

bool SitemonApp::performHitLoadTest(Script &script, int threads, const std::string &outputPath)
{
	LoadTestResultsSaver saver;
	
	SQLiteDB *pLoadTestingDB = NULL;

	HitTestEngine engine;
	
	if (!outputPath.empty())
	{
		if (outputPath == ":db")
		{
			std::string loadTestingDBPath = m_configSettings.m_loadTestingDBPath;
			
			if (loadTestingDBPath.empty())
			{
				std::cout << "No load testing DB path specified in Sitemon's config file...\n";
				return false;
			}			
			
			pLoadTestingDB = new SQLiteDB(loadTestingDBPath);
			saver.initStorage("", pLoadTestingDB);
		}
		else
		{
			saver.initStorage(outputPath);
		}

		engine.setResultsSaver(&saver);
	}
		
	engine.initialise(script, threads);
	
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

bool SitemonApp::performHitLoadTest(Script &script, const std::string &outputPath)
{
	if (!script.hasLoadTestSettings())
		return false;
	
	LoadTestResultsSaver saver;
	
	SQLiteDB *pLoadTestingDB = NULL;

	HitTestEngine engine;
	
	if (!outputPath.empty())
	{
		if (outputPath == ":db")
		{
			std::string loadTestingDBPath = m_configSettings.m_loadTestingDBPath;
			
			if (loadTestingDBPath.empty())
			{
				std::cout << "No load testing DB path specified in Sitemon's config file...\n";
				return false;
			}			
			
			pLoadTestingDB = new SQLiteDB(loadTestingDBPath);
			saver.initStorage("", pLoadTestingDB);
		}
		else
		{
			saver.initStorage(outputPath);
		}

		engine.setResultsSaver(&saver);
	}
	
	LoadTestSettings &ltSettings = script.getLoadTestSettings();
	
	
	engine.initialise(script, ltSettings.getHitThreads(), ltSettings.getHitRepeats());
		
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

bool SitemonApp::performProfileLoadTest(HTTPRequest &request, int threads, int duration, const std::string &outputPath)
{
	LoadTestResultsSaver saver;
	
	SQLiteDB *pLoadTestingDB = NULL;

	ProfileTestEngine engine;
	
	if (!outputPath.empty())
	{
		if (outputPath == ":db")
		{
			std::string loadTestingDBPath = m_configSettings.m_loadTestingDBPath;
			
			if (loadTestingDBPath.empty())
			{
				std::cout << "No load testing DB path specified in Sitemon's config file...\n";
				return false;
			}			
			
			pLoadTestingDB = new SQLiteDB(loadTestingDBPath);
			saver.initStorage("", pLoadTestingDB);
		}
		else
		{
			saver.initStorage(outputPath);
		}

		engine.setResultsSaver(&saver);
	}	
	
	engine.initialise(request, 2);
	engine.addProfileSegment(threads, duration);
	
	saver.start();
	engine.start();
	
	saver.stop();
	
	return true;
}

bool SitemonApp::performProfileLoadTest(Script &script, int threads, int duration, const std::string &outputPath)
{
	LoadTestResultsSaver saver;
	
	SQLiteDB *pLoadTestingDB = NULL;

	ProfileTestEngine engine;
	
	if (!outputPath.empty())
	{
		if (outputPath == ":db")
		{
			std::string loadTestingDBPath = m_configSettings.m_loadTestingDBPath;
			
			if (loadTestingDBPath.empty())
			{
				std::cout << "No load testing DB path specified in Sitemon's config file...\n";
				return false;
			}			
			
			pLoadTestingDB = new SQLiteDB(loadTestingDBPath);
			saver.initStorage("", pLoadTestingDB);
		}
		else
		{
			saver.initStorage(outputPath);
		}

		engine.setResultsSaver(&saver);
	}	

	engine.initialise(script);
	engine.addProfileSegment(threads, duration);
	
	saver.start();
	engine.start();

	saver.stop();
	
	return true;
}

bool SitemonApp::performProfileLoadTest(Script &script, const std::string &outputPath)
{
	LoadTestSettings &settings = script.getLoadTestSettings();
	
	if (settings.m_type != LOAD_PROFILE_TEST)
		return false;
	
	LoadTestResultsSaver saver;
	
	SQLiteDB *pLoadTestingDB = NULL;

	ProfileTestEngine engine;
	
	if (!outputPath.empty())
	{
		if (outputPath == ":db")
		{
			std::string loadTestingDBPath = m_configSettings.m_loadTestingDBPath;
			
			if (loadTestingDBPath.empty())
			{
				std::cout << "No load testing DB path specified in Sitemon's config file...\n";
				return false;
			}			
			
			pLoadTestingDB = new SQLiteDB(loadTestingDBPath);
			saver.initStorage("", pLoadTestingDB);
		}
		else
		{
			saver.initStorage(outputPath);
		}

		engine.setResultsSaver(&saver);
	}	
	
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

void SitemonApp::outputResponse(HTTPRequest &request, HTTPResponse &response)
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
