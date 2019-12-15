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

#include "profile_load_request_thread.h"

#include "../script_result.h"

ProfileLoadRequestThread::ProfileLoadRequestThread(ProfileThreadData *data) : m_active(false), m_pSaver(NULL)
{
	m_Script.copyScript(data->m_pScript);
	m_threadID = data->m_thread;
	
	m_debugging = data->m_debugging;
	
	if (data->m_pSaver)
		m_pSaver = data->m_pSaver;
	else
		m_pSaver = NULL;
	
	delete data;
}

ProfileLoadRequestThread::~ProfileLoadRequestThread()
{
	
}

void ProfileLoadRequestThread::run()
{
	m_active = true;
	
	while (m_active)
	{
		HTTPEngine engine;
		
		if (m_debugging)
			printf("Starting thread %i...\n", m_threadID);

		ScriptResult result;
		
		int step = 1;
		for (std::vector<HTTPRequest>::const_iterator it = m_Script.begin(); it != m_Script.end(); ++it, step++)
		{
			const HTTPRequest &request = *it;
			
			HTTPResponse response;
			response.m_thread = m_threadID;
			
			if (engine.performRequest(request, response))
			{
				result.addResponse(response);
				
				if (m_debugging)
					printf("Thread:\t%i, Step\t%i:\tOK\n", m_threadID, step);
			}
			else
			{
				result.addResponse(response);
				
				if (m_debugging)
					printf("Thread:\t%i, Step\t%i\tError: %i\n", m_threadID, step, response.errorCode);
				
				break; // break out and end, as there's been an issue
			}
			
			if (request.getPauseTime() > 0)
			{
				sleep(request.getPauseTime());
			}
		}

		if (m_pSaver)
		{
			m_pSaver->addResult(result);
		}
		
		sleep(1);
	}
}

void ProfileLoadRequestThread::shutdownThreadAndKill()
{
	m_autoDestruct = true;
	m_active = false;
}

void ProfileLoadRequestThread::tellThreadToStop()
{
	m_active = false;
}
