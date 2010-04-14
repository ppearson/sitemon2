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

Script::Script(HTTPRequest *pRequest)
{
	m_aSteps.push_back(*pRequest);
}

void Script::copyScript(Script *pScript)
{
	m_aSteps = pScript->m_aSteps;
}

bool Script::loadScriptFile(const std::string &file)
{
	std::string finalPath;

	// handle relative paths
#ifdef _MSC_VER
	if (file.find(":") == -1)
#else
	if (file[0] != '/')
#endif
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
	
	for (pElem = pElem->FirstChildElement("request"); pElem; pElem = pElem->NextSiblingElement())
	{
		loadRequestElement(pElem);
	}

	return true;
}

void Script::loadRequestElement(TiXmlElement *pElement)
{
	HTTPRequest request;
	
	for (TiXmlElement *pItem = pElement->FirstChildElement(); pItem; pItem = pItem->NextSiblingElement())
	{
		const std::string &elementName = pItem->ValueStr();

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
