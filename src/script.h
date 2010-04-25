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
#include "debug_settings.h"

enum LoadTestType
{
	LOAD_HIT_TEST,
	LOAD_PROFILE_TEST
};

struct LoadTestProfileSeg
{
	LoadTestProfileSeg(int threads, int duration) : m_threads(threads), m_duration(duration) { }

	int	m_threads;
	int	m_duration;
};

class LoadTestSettings
{
public:
	LoadTestSettings();
	
	bool loadLoadTestElement(TiXmlElement *pElement);
	void loadSegmentsElement(TiXmlElement *pElement);
	
	inline std::vector<LoadTestProfileSeg>::iterator begin() { return m_aProfileSegments.begin(); }
	inline std::vector<LoadTestProfileSeg>::iterator end() { return m_aProfileSegments.end(); }
	
	bool			m_set;
	
	LoadTestType	m_type;
	
protected:
	

	void addProfile(LoadTestProfileSeg &seg);

	// hit settings
	int		m_threads;
	int		m_repeats;

	// profile settings
	std::vector<LoadTestProfileSeg>	m_aProfileSegments;
};

class Script
{
public:
	Script();
	Script(HTTPRequest *pRequest);
	~Script();
	
	std::string getDescription() { return m_description; }

	void copyScript(Script *pScript);
	
	bool loadScriptFile(const std::string &file);

	inline std::vector<HTTPRequest>::iterator begin() { return m_aSteps.begin(); }
	inline std::vector<HTTPRequest>::iterator end() { return m_aSteps.end(); }
	
	int getStepCount() { return m_aSteps.size(); }

	// functions to set stuff for all steps
	void setAcceptCompressed(bool acceptCompressed);
	void setDownloadContent(bool downloadContent);
	
	bool hasLoadTestSettings() { return m_hasLoadTestSettings; }
	LoadTestSettings &getLoadTestSettings() { return m_loadTestSettings; }

protected:
	void loadRequestElement(TiXmlElement *pElement);
	void loadParamsElement(TiXmlElement *pElement, HTTPRequest &request);
	void loadCookiesElement(TiXmlElement *pElement, HTTPRequest &request);
	
protected:
	std::string		m_description;
	std::vector<HTTPRequest> m_aSteps;
	
	DebugSettings	m_debugSettings;
	bool			m_scriptHasDebugSettings;
	
	LoadTestSettings	m_loadTestSettings;
	bool				m_hasLoadTestSettings;
	
	friend class ScriptDebugger;
};


#endif