/*
 *  script.cpp
 *  sitemon
 *
 *  Created by Peter Pearson on 28/02/2010.
 *  Copyright 2010 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef _MSC_VER
#include <sys/select.h>
#include <sys/time.h>
#include <unistd.h>
#else
#include <direct.h>
#endif

#include "script.h"

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
		char *szCurrentDir = getcwd(NULL, 0);
		if (szCurrentDir == 0)
		{
			printf("can't get current dir - try using a full path\n");
			return false;
		}

		std::string strFullPath = szCurrentDir;
#ifdef _MSC_VER
		strFullPath += "\\";
#else
		strFullPath += "/";
#endif
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
