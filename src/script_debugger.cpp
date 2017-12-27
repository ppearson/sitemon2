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
#include <fstream>

#include "script_debugger.h"
#include "debug_settings.h"
#include "http_engine.h"
#include "utils/misc.h"

ScriptDebugger::ScriptDebugger(Script &script) : m_script(script)
{
	// if script has debug settings, copy them over
	if (script.m_scriptHasDebugSettings)
	{
		m_debugSettings = script.m_debugSettings;
	}
}

void ScriptDebugger::setDebugSettings(DebugSettings &settings)
{
	m_debugSettings = settings;
}

void ScriptDebugger::run()
{
	std::ofstream debugLogFileStream;
	bool loggingToFile = false;

	if (m_debugSettings.m_outputBodyResponse)
	{
		std::string fullPath;
		std::string outputPath = m_debugSettings.m_outputPath;
		
		if (!isFullPath(outputPath))
		{
			std::string currentDir = getCurrentDirectory();
			if (currentDir.empty())
			{
				printf("can't get current dir - try using a full path\n");
				return;
			}
			
			fullPath = currentDir;
			fullPath += outputPath;
		}
		else
		{
			fullPath = outputPath;
		}
		
		debugLogFileStream.open(fullPath.c_str(), std::ios::out | std::ios::binary);
		
		if (debugLogFileStream)
		{
			debugLogFileStream.clear();
			loggingToFile = true;
		}
	}
	
	HTTPEngine engine(true); // we want debug enabled
	
	int step = 1;
	
	for (std::vector<HTTPRequest>::iterator it = m_script.begin(); it != m_script.end(); ++it, step++)
	{
		HTTPRequest &request = *it;
		HTTPResponse response;
		
		std::cout << "Running step " << step << ": " << request.getDescription() << "... ";
		
		if (engine.performRequest(request, response))
		{
			std::cout << "completed successfully.\n";
		}
		else
		{
			std::cout << "failed:\n " << " failed with error code: " << response.errorCode << " (" << response.errorString << "), response code: " << response.responseCode << "\n";
			
			if (loggingToFile)
			{
				if (m_debugSettings.m_outputBodyResponse)
				{
					debugLogFileStream << response.content << "\n";
					std::cout << "Body response for this step written to: " << m_debugSettings.m_outputPath << "\n";
				}
			}

			if (m_debugSettings.m_outputHeaderRequest)
			{
				std::cout << "Header sent:\n" << engine.getRequestHeader() << "\n";
			}
			
			break;
		}
	}
	
	if (loggingToFile)
	{
		debugLogFileStream << std::flush;
		debugLogFileStream.close();
	}
}
