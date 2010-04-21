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

#include <iostream>
#include <curl/curl.h>

#include "script.h"
#include "config.h"
#include "sitemon.h"
#include "http_server/http_server.h"
#include "utils/socket.h"
#include "scheduler/scheduler.h"

#include "scheduler/scheduler_db_helpers.h"
#include "http_server/http_server_db_helpers.h"

#include "script_debugger.h"

#ifdef _MSC_VER
BOOL CtrlHandler(DWORD fdwCtrlType);
#endif
static void printUsage();

int main(int argc, char *const argv[])
{
	char *szURL = 0;
	char *szScript = 0;
	char *szOutputFile = 0;
	
	bool debug = false;

	bool isScript = false;
	bool concurrent = false;
	
	bool loadTestProfile = false;
	bool loadTestHit = false;

	bool outputHeader = false;
	bool outputBody = false;
	
	bool acceptCompressed = false;
	bool downloadContent = false;

	bool runWeb = false;

	int threads = 0;

#ifdef _MSC_VER
	SetConsoleCtrlHandler((PHANDLER_ROUTINE)CtrlHandler, TRUE);
#endif

	if (argc == 1 || (argc == 2 && (strcmp(argv[1], "--help") == 0) || strcmp(argv[1], "/?") == 0))
	{
		printUsage();
		return 0;
	}
	else
	{
		for (int i = 1; i < argc; i++)
		{
			if (strcmp(argv[i], "-s") == 0 && i < argc)
			{
				isScript = true;
				szScript = (char *)argv[i++ + 1];
			}
			else if (strcmp(argv[i], "-sm") == 0 && i < argc)
			{
				isScript = true;
				concurrent = true;
				szScript = (char *)argv[i + 1];
				
				threads = atoi((char *)argv[i + 2]);
				
				i += 2;
				
				if (argc > i) // do we have an output file?
				{
					szOutputFile = (char *)argv[i + 1];
				}
			}
			else if (strcmp(argv[i], "-debug") == 0 && i < argc)
			{
				debug = true;
				
				szScript = (char *)argv[i + 1];
				
				i++;
				
				if (argc > i) // if we have a script output file
				{
					szOutputFile = (char *)argv[i + 1];
				}				
			}
			else if (strcmp(argv[i], "lt-profile") == 0)
			{
				loadTestProfile = true;
			}
			else if (strcmp(argv[i], "lt-hit") == 0)
			{
				loadTestHit = true;
			}
			else if (strcmp(argv[i], "-oh") == 0)
			{
				outputHeader = true;
			}
			else if (strcmp(argv[i], "-ob") == 0)
			{
				outputBody = true;
			}
			else if (strcmp(argv[i], "-ac") == 0)
			{
				acceptCompressed = true;
			}
			else if (strcmp(argv[i], "-dc") == 0)
			{
				downloadContent = true;
			}
			else if (strcmp(argv[i], "-web") == 0)
			{
				runWeb = true;
			}
			else
			{
				szURL = (char *)argv[i];
			}
		}
	}
	
	Config configFile;
#ifndef _MSC_VER
	configFile.loadConfigFile("/Users/peter/sm_config.xml");
#else
	configFile.loadConfigFile();
#endif
	
	curl_global_init(CURL_GLOBAL_ALL);
	
	if (runWeb)
	{
		std::string webContentPath = configFile.getWebContentPath();
		std::string dbPath = configFile.getDBPath();

		SQLiteDB *pMainDB = NULL;
	
		if (!dbPath.empty())
		{
			pMainDB = new SQLiteDB(dbPath);
		}
		
		if (!pMainDB->isThreadSafe())
		{
			std::cout << "SQLite is not thread safe!\n";
		}
		
		// create the needed tables first
		createNeededHTTPServerTables(pMainDB);
		createNeededSchedulerTables(pMainDB);
		
		Scheduler schedulerThread(pMainDB);
		schedulerThread.start();

		std::cout << "Scheduler thread started...\n";

		Thread::sleep(1); // to enable db tables to be created if needed

		//
		std::cout << "Starting web server on http://localhost:" << 8080 << "/...\n";

		Socket::initWinsocks();		
		
		HTTPServer server(webContentPath, pMainDB, 8080);
		// keep in mind this halts execution, by design
		server.start();
		
		curl_global_cleanup();

		Socket::cleanupWinsocks();

		if (pMainDB)
			delete pMainDB;
		
		return 0;
	}
	
	if (debug)
	{
		Script script;
		if (!script.loadScriptFile(szScript))
		{
			std::cout << "Can't open file: " << szScript << "\n";
			return -1;
		}
		
		ScriptDebugger debugger(script);
		
		if (szOutputFile) // if we have a specified output file, override any script settings with that
		{
			DebugSettings settings(szOutputFile);
			debugger.setDebugSettings(settings);			
		}		
		
		debugger.run();		
	}
	else if (loadTestProfile)
	{
	//	performProfileLoadTest();
		
	}
	else if (!isScript)
	{
		HTTPRequest request(szURL);

		if (acceptCompressed)
		{
			request.setAcceptCompressed(true);
		}
		
		if (downloadContent)
		{
			request.setDownloadContent(true);
		}

		performSingleRequest(request, outputHeader);
	}
	else
	{
		Script script;
		if (!script.loadScriptFile(szScript))
		{
			std::cout << "Can't open file: " << szScript << "\n";
			return -1;
		}

		// command prompt options override script settings for certain things

		if (acceptCompressed)
		{
			script.setAcceptCompressed(true);
		}
		
		if (downloadContent)
		{
			script.setDownloadContent(true);
		}

		// add stuff here to read the script file and see if there are any concurrent params
		// then we can just run the script, and it will do the concurrent stuff if it's specified

		if (!concurrent)
		{
			performScriptRequest(script);
		}
		else
		{
			std::string outputFile;
			if (szOutputFile)
				outputFile.assign(szOutputFile);
			performHitLoadTestScriptRequest(script, threads, outputFile);
		}
	}
	
	curl_global_cleanup();
	
    return 0;
}

void printUsage()
{
	printf("Sitemon version 1.0\nUsage:\nSingle test:\t\t\t\tsitemon [<options>] <URL>\n"
		   "Single Script test:\t\t\tsitemon [<options>] -s <script_path>\n"
		   "Script Load test:\t\t\tsitemon [<options>] -sm <script_path> <num threads> [output_file]\n"
		   "Debug script:\t\t\t\tsitemon debug <script_path> [body_response_file_path]\n"
		   "Run local web server for interface:\tsitemon -web\n"
		   "Options:\n-ac\t\t: Accept compressed content\n"
		   "-dc\t\t: Download linked JS and Image content\n"
		   "-ol <val>\t: Output logging level (to screen). 0 = minimal (default), 2 = max.\n"
		   "-oh\t\t: Output headers\n"
		   "-ob\t\t: Output body\n");
}

#ifdef _MSC_VER
BOOL CtrlHandler(DWORD fdwCtrlType)
{ 
	switch (fdwCtrlType)
	{ 
	case CTRL_C_EVENT:
	case CTRL_CLOSE_EVENT:
	default:
	return TRUE; 
	}
}
#endif