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

#include "hit_load_request_thread.h"

HitLoadRequestThread::HitLoadRequestThread(RequestThreadData *data)
{
	m_Script.copyScript(data->m_pScript);
	m_threadID = data->m_thread;
	m_repeats = data->m_repeats;
	
	m_debugging = data->m_debugging;
	
	delete data;
}

HitLoadRequestThread::~HitLoadRequestThread()
{

}

void HitLoadRequestThread::run()
{
	int runs = 1;
	
	runs += m_repeats;
	
	for (int i = 0; i < runs; i++)
	{
		HTTPEngine engine(false);
		
		engine.initCURLHandle();
		
		for (std::vector<HTTPRequest>::iterator it = m_Script.begin(); it != m_Script.end(); ++it)
		{
			HTTPRequest &request = *it;
			
			HTTPResponse response; // don't store content
			response.m_thread = m_threadID;

			if (m_debugging)
				printf("Starting thread %i...\n", m_threadID);

			if (engine.performRequest(request, response))
			{
				m_aResponses.push_back(response);
				
				if (m_debugging)
					printf("Thread: %i\tOK\n", m_threadID);
			}
			else
			{
				m_aResponses.push_back(response);
				
				if (m_debugging)
					printf("Thread: %i\tError: %i\n", m_threadID, response.errorCode);

				break; // break out and end, as there's been an issue
			}
			
			if (request.getPauseTime() > 0)
			{
				sleep(request.getPauseTime());
			}
		}
		
		if (m_debugging && i > 0)
			printf("Thread: %i\t is repeating\n", m_threadID);
	}
}
