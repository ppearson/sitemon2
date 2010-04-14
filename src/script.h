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

#ifndef SCRIPT_H
#define SCRIPT_H

#include <vector>

#include "utils/tinyxml.h"

#include "http_request.h"

class Script
{
public:
	Script() { }
	Script(HTTPRequest *pRequest);

	void copyScript(Script *pScript);
	
	bool loadScriptFile(const std::string &file);

	inline std::vector<HTTPRequest>::iterator begin() { return m_aSteps.begin(); }
	inline std::vector<HTTPRequest>::iterator end() { return m_aSteps.end(); }

	// functions to set stuff for all steps
	void setAcceptCompressed(bool acceptCompressed);
	void setDownloadContent(bool downloadContent);

protected:
	void loadRequestElement(TiXmlElement *pElement);
	void loadParamsElement(TiXmlElement *pElement, HTTPRequest &request);
	void loadCookiesElement(TiXmlElement *pElement, HTTPRequest &request);	
	
protected:
	std::vector<HTTPRequest> m_aSteps;	
};


#endif