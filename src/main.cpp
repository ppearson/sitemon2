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

#include "utils/misc.h"
#include "script.h"
#include "config.h"
#include "sitemon.h"

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
	
	bool loadTestProfile = false;
	bool loadTestHit = false;

	bool outputHeader = false;
	bool outputBody = false;
	
	bool acceptCompressed = false;
	bool downloadContent = false;

	bool runWeb = false;

	int threads = 0;
	int minutes = 0;

#ifdef _MSC_VER
	SetConsoleCtrlHandler((PHANDLER_ROUTINE)CtrlHandler, TRUE);
#endif

	if (argc == 1 || (argc == 2 && (strcmp(argv[1], "--help") == 0 || strcmp(argv[1], "/?") == 0)))
	{
		printUsage();
		return 0;
	}
	else
	{
		// this is a mess, but due to the fact that certain arguments can be used for almost any command type,
		// it's the easiest way of doing things
		for (int i = 1; i < argc; i++)
		{
			if (strcmp(argv[i], "script") == 0)
			{
				isScript = true;
				szScript = (char *)argv[i++ + 1];
			}
			else if (strcmp(argv[i], "debug") == 0)
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
				
				// next param should be script or http address
				std::string item = (char *)argv[i + 1];
				
				if (item.substr(0, 4) == "http") // it's a URL, so we're doing a request for a constant time profile test
				{
					szURL = (char *)argv[i + 1];
					
					// next two must be number of threads and the time in minutes
					
					threads = atoi((char *)argv[i + 2]);
					minutes = atoi((char *)argv[i + 3]);
					
					i += 3;
					
					if (argc > i + 1) // do we have an output file?
					{
						szOutputFile = (char *)argv[i + 1];
						
						i++;
					}		
				}
				else // must be a script file
				{
					szScript = (char *)argv[i + 1];
					
					i++;
					
					// see if there are more than two more params - probably thread/minute params
					
					if (argc > i + 2)
					{
						threads = atoi((char *)argv[i + 1]);
						minutes = atoi((char *)argv[i + 2]);
						
						i += 2;						
					}
					
					if (argc > i) // do we have an output file?
					{
						szOutputFile = (char *)argv[i + 1];
						
						i++;
					}
				}
			}
			else if (strcmp(argv[i], "lt-hit") == 0)
			{
				loadTestHit = true;
				
				// next param should be script or http address
				std::string item = (char *)argv[i + 1];
				
				if (item.substr(0, 4) == "http") // it's a URL, so we're doing a simple http request
				{
					szURL = (char *)argv[i + 1];
					
					// next must be number of threads
					
					threads = atoi((char *)argv[i + 2]);
	
					i += 2;
					
					if (argc > i + 1) // do we have an output file?
					{
						szOutputFile = (char *)argv[i + 1];
						
						i++;
					}		
				}
				else // must be a script file
				{
					szScript = (char *)argv[i + 1];
					
					i++;
					
					// next one could be number of threads or output file
					
					if (argc > i)
					{
						char * nextArg = argv[i + 1];
						
						if (isNumber(nextArg))
						{
							threads = atoi((char *)argv[i + 1]);
					
							i++;
						}
						else // must be output file
						{
							szOutputFile = (char *)argv[i + 1];
							
							i++;
						}
					}
					
					if (argc > i + 1) // do we have an output file?
					{
						szOutputFile = (char *)argv[i + 1];
						
						i++;
					}
				}
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
	
	SitemonApp sitemon;
	
	sitemon.loadConfigSettings();
	
	curl_global_init(CURL_GLOBAL_ALL);
	
	if (runWeb)
	{
		sitemon.runWebServerAndScheduler();
		
		curl_global_cleanup();
		
		return 0;
	}
	
	if (debug)
	{
		Script script;
		if (!script.loadScriptFile(szScript))
		{
			std::cout << "Can't open script file: " << szScript << "\n";
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
		std::string outputFile;
		if (szOutputFile)
			outputFile.assign(szOutputFile);
		
		// if it's a single URL test
		if (szURL && !szScript)
		{
			HTTPRequest newRequest(szURL);
			
			// command prompt options override settings for certain things
			newRequest.setAcceptCompressed(acceptCompressed);
			newRequest.setDownloadContent(downloadContent);
			
			std::cout << "Starting constant Profile Load test with " << threads << " threads, for " << minutes << " minutes...\n";
			
			if (sitemon.performProfileLoadTest(newRequest, threads, minutes, outputFile))
			{
				std::cout << "Load test completed successfully.\n";
			}
			else
			{
				std::cout << "Problem starting load test.\n";
			}
		}
		else if (szScript && !szURL) // it's a script file
		{
			Script script;
			if (!script.loadScriptFile(szScript))
			{
				std::cout << "Can't open script file: " << szScript << "\n";
				return -1;
			}
			
			// command prompt options override script settings for certain things.
			// for scripts, only override if set from the command line...
			if (acceptCompressed)
			{
				script.setAcceptCompressed(true);
			}
			
			if (downloadContent)
			{
				script.setDownloadContent(true);
			}
			
			// see if the script has any load test settings
			if (script.hasLoadTestSettings())
			{
				std::string description = script.getDescription();
				
				std::cout << "Starting Profile Load Test with profile settings from script...\n";
				
				if (sitemon.performProfileLoadTest(script, outputFile))
				{
					std::cout << "Load test completed successfully.\n";
				}
				else
				{
					std::cout << "Problem starting load test.\n";
				}				
			}
			else
			{
				// constant profile script test
				
				std::cout << "Starting constant Profile Load test with " << threads << " threads, for " << minutes << " minutes...\n";
				
				if (sitemon.performProfileLoadTest(script, threads, minutes, outputFile))
				{
					std::cout << "Load test completed successfully.\n";
				}
				else
				{
					std::cout << "Problem starting load test.\n";
				}				
			}
		}
	}
	else if (loadTestHit)
	{
		std::string outputFile;
		if (szOutputFile)
			outputFile.assign(szOutputFile);
		
		// if it's a single URL test
		if (szURL && !szScript)
		{
			HTTPRequest newRequest(szURL);
			
			// command prompt options override settings for certain things
			newRequest.setAcceptCompressed(acceptCompressed);
			newRequest.setDownloadContent(downloadContent);
			
			std::cout << "Starting Hit Load test with " << threads << " threads...\n";
			
			if (sitemon.performHitLoadTest(newRequest, threads, outputFile))
			{
				std::cout << "Load test completed successfully.\n";
			}
			else
			{
				std::cout << "Problem starting load test.\n";
			}
		}
		else if (szScript && !szURL) // it's a script file
		{
			Script script;
			if (!script.loadScriptFile(szScript))
			{
				std::cout << "Can't open script file: " << szScript << "\n";
				return -1;
			}
			
			// command prompt options override script settings for certain things.
			// for scripts, only override if set from the command line...
			if (acceptCompressed)
			{
				script.setAcceptCompressed(true);
			}
			
			if (downloadContent)
			{
				script.setDownloadContent(true);
			}
			
			// see if the script has any load test settings
			if (script.hasLoadTestSettings())
			{
				std::string description = script.getDescription();
				
				std::cout << "Starting Hit Load Test with profile settings from script...\n";
				
				if (sitemon.performHitLoadTest(script, outputFile))
				{
					std::cout << "Load test completed successfully.\n";
				}
				else
				{
					std::cout << "Problem starting load test.\n";
				}				
			}
			else
			{
				// Hit load test with threads specified on command line
				
				std::cout << "Starting Hit Load test with " << threads << " threads...\n";
				
				if (sitemon.performHitLoadTest(script, threads, outputFile))
				{
					std::cout << "Load test completed successfully.\n";
				}
				else
				{
					std::cout << "Problem starting load test.\n";
				}				
			}
		}		
	}
	else if (!isScript)
	{
		HTTPRequest request(szURL);

		// command prompt options override settings for certain things
		request.setAcceptCompressed(acceptCompressed);
		request.setDownloadContent(downloadContent);

		sitemon.performSingleRequest(request, outputHeader, outputBody);
	}
	else
	{
		Script script;
		if (!script.loadScriptFile(szScript))
		{
			std::cout << "Can't open script file: " << szScript << "\n";
			return -1;
		}

		// command prompt options override script settings for certain things.
		// for scripts, only override if set from the command line...
		if (acceptCompressed)
		{
			script.setAcceptCompressed(true);
		}
		
		if (downloadContent)
		{
			script.setDownloadContent(true);
		}

		sitemon.performScriptRequest(script);
	}
	
	curl_global_cleanup();
	
    return 0;
}

void printUsage()
{
	printf("Sitemon version 0.7.2\nUsage:\nSingle test:\t\t\tsitemon [<options>] <URL>\n"
		   "Single Script test:\t\tsitemon [<options>] script <script_path>\n"
		   "Debug script:\t\t\tsitemon debug <script_path> [body_response_file_path.html]\n\n"
		   "Load Testing:\n"
		   "Hit test URL:\t\tsitemon lt-hit <URL> <number_of_requests> [<repeats>] [<output_results_file.csv>]\n"
		   "Hit test Script:\tsitemon lt-hit <script_path> <number_of_requests> [<repeats>] [<output_results_file.csv>]\n"
		   "Profile test URL:\tsitemon lt-profile <URL> <conc._requests> <time_in_min> [<output_results_file.csv>]\n"
		   "Profile test Script:\tsitemon lt-profile <script_path> [<conc._requests> <time_in_min>] [<output_results_file.csv>]\n\n"
		   "Run local web server for interface:\tsitemon -web\n\n"
		   "Options:\n-ac\t\t: Accept compressed content\n"
		   "-dc\t\t: Download linked CSS, JS and Image content\n"
		   "-oh\t\t: Output header\n"
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
		{
			// brutal, but easiest for the moment
			exit(1);
			return TRUE; 
		}
	}
}
#endif
