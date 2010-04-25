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

#include "script_result.h"

ScriptResult::ScriptResult() : m_overallError(HTTP_OK), m_lastResponseCode(0)
{
	
}

void ScriptResult::addResponse(HTTPResponse &response)
{
	if (m_stepResponses.empty())
	{
		m_requestStartTime = response.timestamp;		
	}
	
	m_overallError = response.errorCode;
	m_lastResponseCode = response.responseCode;
		
	m_stepResponses.push_back(response);
}