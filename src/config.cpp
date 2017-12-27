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

#include <stdio.h>
#include <stdlib.h>

#include "utils/misc.h"

#include "config.h"

Config::Config(ConfigSettings &configSettings) : m_configSettings(configSettings)
{

}

bool Config::loadConfigFile()
{
	std::string dirPath;
#ifndef _DEBUG
	dirPath = getExecutablePath();
#else
	dirPath = getCurrentDirectory();
#endif
	
	// if we have an env variable set for this, use that...
	const char* smPathEnv = getenv("SITEMON_PATH");
	if (smPathEnv && strlen(smPathEnv) > 0)
	{
		dirPath.assign(smPathEnv);
	}
	
	if (dirPath.substr(dirPath.size() - 1, 1) != "/")
	{
		dirPath += "/";
	}
	
	std::string finalPath = dirPath;
	
	// try XML config file first...
	if (loadXMLConfigFile(finalPath + "sm_config.xml"))
	{
		return true;
	}
	
	return false;
}

bool Config::loadXMLConfigFile(const std::string& filename)
{
	TiXmlDocument doc(filename);

	if (!doc.LoadFile())
	{
		return false;
	}

	TiXmlHandle hDoc(&doc);
	TiXmlElement* pElem = NULL;

	pElem = hDoc.FirstChildElement("config").Element();

	if (!pElem)
		return false;

	for (TiXmlElement *pItem = pElem->FirstChildElement(); pItem; pItem = pItem->NextSiblingElement())
	{
		const std::string &elementName = pItem->ValueStr();

		std::string content;
		if (pItem->GetText())
			content = pItem->GetText();

		if (elementName == "web_content_path")
		{
			m_configSettings.m_webContentPath = content;
		}
		else if (elementName == "mon_db_path")
		{
			m_configSettings.m_monitoringDBPath = content;
		}
		else if (elementName == "loadtest_db_path")
		{
			m_configSettings.m_loadTestingDBPath = content;
		}
		else if (elementName == "web_server_port")
		{
			if (!content.empty())
			{
				int port = atoi(content.c_str());
				m_configSettings.m_webServerPort = port;
			}
		}
		else if (elementName == "proxy")
		{
			loadXMLProxySettings(pItem);
		}
	}
	
	return true;
}

void Config::loadXMLProxySettings(TiXmlElement* pElement)
{
	m_configSettings.m_useProxy = true;

	if (pElement->Attribute("enabled"))
	{
		const char *pEnabled = pElement->Attribute("enabled");

		int enabled = atoi(pEnabled);

		if (enabled == 0)
		{
			m_configSettings.m_useProxy = false;
		}
	}

	for (TiXmlElement *pItem = pElement->FirstChildElement(); pItem; pItem = pItem->NextSiblingElement())
	{
		const std::string elementName = pItem->ValueStr();

		std::string content;
		if (pItem->GetText())
			content = pItem->GetText();

		if (elementName == "host")
		{
			m_configSettings.m_proxySettings.m_proxyHost = content;
		}
		else if (elementName == "port")
		{
			m_configSettings.m_proxySettings.m_proxyPort = content;
		}
		else if (elementName == "user")
		{
			m_configSettings.m_proxySettings.m_proxyUser = content;
		}
		else if (elementName == "pass")
		{
			m_configSettings.m_proxySettings.m_proxyPass = content;
		}
	}
}
