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
	// clean up any dynamic parameter objects
	std::vector<HTTPRequest>::iterator it = m_aSteps.begin();

	for (; it != m_aSteps.end(); ++it)
	{
		it->cleanupDynamicParameters();
	}
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
	TiXmlElement* pElem = NULL, *pScriptElem = NULL;
	TiXmlHandle hRoot(0);
	
	pScriptElem = hDoc.FirstChildElement("script").Element();
	
	if (!pScriptElem) // no script item...
		return false;
	
	// see if there's a description attribute
	if (pScriptElem->Attribute("desc"))
	{
		m_description = pScriptElem->Attribute("desc");
	}

	// see if it's got a hostname tag
	pElem = pScriptElem->FirstChildElement("hostname");
	if (pElem)
	{
		if (pElem->GetText())
		{
			m_hostName = pElem->GetText();
		}
	}
	
	for (pElem = pScriptElem->FirstChildElement("request"); pElem; pElem = pElem->NextSiblingElement())
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
			// replace @ with hostname if we have one
			if (!m_hostName.empty() && content.find("@") != -1)
			{
				int hostPos = content.find("@");

				content.erase(hostPos, 1);
				content.insert(hostPos, m_hostName);

				request.setUrl(content);
			}
			else
			{
				request.setUrl(content);
			}
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
		else if (elementName == "extraction_items")
		{
			loadExtractionItemsElement(pItem, request);
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
			std::string name = pItem->Attribute("name");
			
			// if it's a dynamic parameter
			if (pItem->Attribute("type"))
			{
				std::string type = pItem->Attribute("type");
				if (type == "date")
				{
					if (pItem->Attribute("days_forward") && pItem->Attribute("format"))
					{
						int daysInFuture = atoi(pItem->Attribute("days_forward"));
						std::string format = pItem->Attribute("format");
						
						DynamicDateParameter *pParam = new DynamicDateParameter(name, format, daysInFuture);
						request.addDynamicParameter(pParam);
					}
				}
				else if (type == "extract")
				{
					if (pItem->Attribute("key"))
					{
						std::string key = pItem->Attribute("key");

						DynamicExtractionParameter *pParam = new DynamicExtractionParameter(name, key);
						request.addDynamicParameter(pParam);
					}
				}
			}
			else // otherwise, it's a standard one
			{			
				std::string content;
				if (pItem->GetText())
					content = pItem->GetText();

				request.addParameter(name, content);
			}
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

void Script::loadExtractionItemsElement(TiXmlElement *pElement, HTTPRequest &request)
{
	for (TiXmlElement *pItem = pElement->FirstChildElement(); pItem; pItem = pItem->NextSiblingElement())
	{
		if (pItem->ValueStr() == "extr_item")
		{
			std::string name = pItem->Attribute("name");
			std::string startText, endText;

			TiXmlElement *pSubItem = pItem->FirstChildElement("start_text");
			if (pSubItem)
			{
				startText = pSubItem->GetText();
			}
			
			pSubItem = pItem->FirstChildElement("end_text");
			if (pSubItem)
			{
				endText = pSubItem->GetText();
			}
			
			if (!name.empty() && !startText.empty() && !endText.empty())
			{
				ExtractionItem newItem(name, startText, endText, 0);
				request.addExtractionItem(newItem);
			}
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
