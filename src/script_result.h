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

#ifndef SCRIPT_RESULT_H
#define SCRIPT_RESULT_H

#include <vector>

#include "http_response.h"

class ScriptResult
{
public:
	ScriptResult();
	
	void addResponse(HTTPResponse &response);
	
	time_t				getRequestStartTime() { return m_requestStartTime; }
	HTTPResponseError	getOverallError() { return m_overallError; }
	long				getLastResponseCode() { return m_lastResponseCode; }
	
	HTTPResponse &		getFirstResponse() { return m_stepResponses.front(); }
	
	int					getResponseCount() { return m_stepResponses.size(); }
	
	inline std::vector<HTTPResponse>::iterator begin() { return m_stepResponses.begin(); }
	inline std::vector<HTTPResponse>::iterator end() { return m_stepResponses.end(); }
	
protected:
	time_t				m_requestStartTime;
	HTTPResponseError	m_overallError;
	long				m_lastResponseCode;
	int					m_problemStep;
	
	std::vector<HTTPResponse>	m_stepResponses;	
};

#endif
