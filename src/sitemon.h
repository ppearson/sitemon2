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

#ifndef SITEMON_H
#define SITEMON_H

#include "http_request.h"
#include "http_response.h"
#include "script.h"
#include "config.h"

class SitemonApp
{
public:
	SitemonApp() { }
	
	void loadConfigSettings();
	
	bool runWebServerAndScheduler();
	
	bool performSingleRequest(const HTTPRequest &request, bool outputHeader, bool outputBody);
	bool performScriptRequest(Script &script);

	bool performHitLoadTest(const HTTPRequest &request, int threads, const std::string &outputPath);
	bool performHitLoadTest(Script &script, int threads, const std::string &outputPath);
	bool performHitLoadTest(Script &script, const std::string &outputPath);

	bool performProfileLoadTest(const HTTPRequest &request, int threads, int duration, const std::string &outputPath);
	bool performProfileLoadTest(Script &script, int threads, int duration, const std::string &outputPath);
	bool performProfileLoadTest(Script &script, const std::string &outputPath);
	
	void outputResponse(const HTTPRequest &request, const HTTPResponse &response);
	
protected:
	ConfigSettings	m_configSettings;
	
};

#endif
