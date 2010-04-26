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

#include "utils/misc.h"
#include "script.h"

Script::Script() : m_scriptHasDebugSettings(false), m_hasLoadTestSettings(false)
{
	
}

Script::Script(HTTPRequest *pRequest) : m_scriptHasDebugSettings(false), m_hasLoadTestSettings(false)
{
	m_aSteps.push_back(*pRequest);
}

Script::~Script()
{
	
}

void Script::copyScript(Script *pScript)
{
	m_aSteps = pScript->m_aSteps;
}

bool Script::loadScriptFile(const std::string &file)
{
	std::string finalPath;

	// handle relative paths
	if (!isFullPath(file))
	{
		char *szCurrentDir = getCurrentDirectory();
		if (szCurrentDir == 0)
		{
			printf("can't get current dir - try using a full path\n");
			return false;
		}

		std::string strFullPath = szCurrentDir;
		strFullPath += file;

		finalPath = strFullPath;
	}
	else
	{
		finalPath = file;
	}

	TiXmlDocument doc(finalPath);
	
	if (!doc.LoadFile())
	{
		printf("Can't load xml file.\n");
		return false;
	}
	
	TiXmlHandle hDoc(&doc);
	TiXmlElement* pElem = NULL;
	TiXmlHandle hRoot(0);
	
	pElem = hDoc.FirstChildElement("script").Element();
	
	if (!pElem) // no script item...
		return false;
	
	// see if there's a description attribute
	if (pElem->Attribute("desc"))
	{
		m_description = pElem->Attribute("desc");
	}
	
	for (pElem = pElem->FirstChildElement("request"); pElem; pElem = pElem->NextSiblingElement())
	{
		loadRequestElement(pElem);
	}
	
	// if there's a debug tag, parse that into the script
	pElem = hDoc.FirstChildElement("debug").Element();
	
	if (pElem)
	{
		m_debugSettings.loadDebugElement(pElem);
		m_scriptHasDebugSettings = true;
	}
	
	// if the script doc has load test settings, import those
	pElem = hDoc.FirstChildElement("load_test").Element();
	
	if (pElem)
	{
		m_loadTestSettings.loadLoadTestElement(pElem);
		m_hasLoadTestSettings = true;
	}

	return true;
}

void Script::loadRequestElement(TiXmlElement *pElement)
{
	if (pElement->ValueStr() != "request")
		return;
	
	HTTPRequest request;
	
	std::string description;
	
	if (pElement->Attribute("desc"))
	{
		description.assign(pElement->Attribute("desc"));
		if (!description.empty())
			request.setDescription(description);
	}
		
	for (TiXmlElement *pItem = pElement->FirstChildElement(); pItem; pItem = pItem->NextSiblingElement())
	{
		const std::string elementName = pItem->ValueStr();

		std::string content;
		if (pItem->GetText())
			content = pItem->GetText();	
		
		if (elementName == "url")
		{
			request.setUrl(content);
		}
		else if (elementName == "desc")
		{
			request.setDescription(content);
		}
		else if (elementName == "type")
		{
			if (content == "POST")
				request.setRequestType(HTTP_POST);
		}
		else if (elementName == "accept_compressed")
		{
			if (content == "1")
				request.setAcceptCompressed(true);
		}
		else if (elementName == "pause")
		{
			int pauseTime = atoi(content.c_str());
			request.setPauseTime(pauseTime);
		}
		else if (elementName == "params")
		{
			loadParamsElement(pItem, request);
		}
		else if (elementName == "cookies")
		{
			loadCookiesElement(pItem, request);
		}
		else if (elementName == "referrer")
		{
			request.setReferrer(content);
		}
		else if (elementName == "expected_phrase")
		{
			request.setExpectedPhrase(content);
		}
		else if (elementName == "connect_timeout")
		{
			if (!content.empty())
			{
				long timeout = atol(content.c_str());
				request.setConnectTimeout(timeout);
			}
		}
		else if (elementName == "total_timeout")
		{
			if (!content.empty())
			{
				long timeout = atol(content.c_str());
				request.setTotalTimeout(timeout);
			}
		}
	}
	
	m_aSteps.push_back(request);
}

void Script::loadParamsElement(TiXmlElement *pElement, HTTPRequest &request)
{
	for (TiXmlElement *pItem = pElement->FirstChildElement(); pItem; pItem = pItem->NextSiblingElement())
	{
		if (pItem->ValueStr() == "param")
		{
			std::string content;
			if (pItem->GetText())
				content = pItem->GetText();
			std::string name = pItem->Attribute("name");

			request.addParameter(name, content);
		}
	}
}

void Script::loadCookiesElement(TiXmlElement *pElement, HTTPRequest &request)
{
	for (TiXmlElement *pItem = pElement->FirstChildElement(); pItem; pItem = pItem->NextSiblingElement())
	{
		if (pItem->ValueStr() == "cookie")
		{
			std::string content;
			if (pItem->GetText())
				content = pItem->GetText();
			std::string name = pItem->Attribute("name");

			request.addCookie(name, content);
		}
	}
}

void Script::setAcceptCompressed(bool acceptCompressed)
{
	std::vector<HTTPRequest>::iterator it = m_aSteps.begin();

	for (; it != m_aSteps.end(); ++it)
	{
		it->setAcceptCompressed(acceptCompressed);
	}
}

void Script::setDownloadContent(bool downloadContent)
{
	std::vector<HTTPRequest>::iterator it = m_aSteps.begin();
	
	for (; it != m_aSteps.end(); ++it)
	{
		it->setDownloadContent(downloadContent);
	}
}

LoadTestSettings::LoadTestSettings()
{
	
}

void LoadTestSettings::addProfile(LoadTestProfileSeg &seg)
{
	m_aProfileSegments.push_back(seg);
}

bool LoadTestSettings::loadLoadTestElement(TiXmlElement *pElement)
{
	for (TiXmlElement *pItem = pElement->FirstChildElement(); pItem; pItem = pItem->NextSiblingElement())
	{
		const std::string elementName = pItem->ValueStr();
		
		std::string content;
		if (pItem->GetText())
			content = pItem->GetText();
		
		if (elementName == "type")
		{
			if (content == "profile")
			{
				m_type = LOAD_PROFILE_TEST;
			}
			else if (content == "hit")
			{
				m_type = LOAD_HIT_TEST;
			}
		}
		else if (elementName == "segments")
		{
			loadSegmentsElement(pItem);
		}
		else if (elementName == "threads")
		{
			if (!content.empty())
			{
				m_threads = atoi(content.c_str());
			}
		}
		else if (elementName == "repeats")
		{
			if (!content.empty())
			{
				m_repeats = atoi(content.c_str());
			}
		}
	}
	
	return true;
}

void LoadTestSettings::loadSegmentsElement(TiXmlElement *pElement)
{
	for (TiXmlElement *pItem = pElement->FirstChildElement(); pItem; pItem = pItem->NextSiblingElement())
	{
		if (pItem->ValueStr() == "seg")
		{
			std::string content;
			if (pItem->GetText())
				content = pItem->GetText();
			
			std::string threadsAttr = pItem->Attribute("threads");
			
			if (!content.empty() && !threadsAttr.empty())
			{
				int threads = atoi(threadsAttr.c_str());
				int duration = atoi(content.c_str());
				
				LoadTestProfileSeg newSeg(threads, duration);
				
				addProfile(newSeg);
			}
		}
	}	
}
