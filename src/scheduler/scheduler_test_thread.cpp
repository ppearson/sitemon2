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

#include "scheduler_test_thread.h"
#include "../utils/sqlite_query.h"
#include "scheduler_db_helpers.h"

SchedulerTestThread::SchedulerTestThread(ScheduledResultsSaver *pSaver, ScheduledItem &item)
				: m_pSaver(pSaver), m_single(item.isSingle()), m_testID(item.getTestID())
{
	m_autoDestruct = true;

	// copy across the item details
	if (m_single)
	{
		m_request = item.getRequest();
	}
	else
	{
		m_script = item.getScript();
	}
}

void SchedulerTestThread::run()
{
	HTTPEngine engine;

	if (m_single)
	{
		HTTPResponse response;

		engine.performRequest(m_request, response);
		
		if (m_pSaver)
		{
			m_pSaver->addResult(response, m_testID);
		}
	}
	else
	{
		ScriptResult result;
		
		for (std::vector<HTTPRequest>::const_iterator it = m_script.begin(); it != m_script.end(); ++it)
		{
			const HTTPRequest &request = *it;
			
			HTTPResponse response;
				
			if (engine.performRequest(request, response))
			{
				result.addResponse(response);
			}
			else
			{
				result.addResponse(response);
				
				break; // break out and end, as there's been an issue
			}
			
			if (request.getPauseTime() > 0)
			{
				sleep(request.getPauseTime());
			}
		}
		
		if (m_pSaver)
		{
			m_pSaver->addResult(result, m_testID);
		}
	}	
}
