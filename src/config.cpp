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

#include "config.h"

Config::Config(ConfigSettings &configSettings) : m_configSettings(configSettings)
{
	
}

bool Config::loadConfigFile(const std::string &configFilePath)
{
	std::string finalPath;
	
	if (!configFilePath.empty())
	{
		finalPath = configFilePath;
	}
	else
	{
		char *szCurrentDir = getCurrentDirectory();
		if (szCurrentDir == 0)
		{
			return false;
		}
		
		finalPath = szCurrentDir;
		
		finalPath += "sm_config.xml";
	}

	TiXmlDocument doc(finalPath);
	
	if (!doc.LoadFile())
	{
		return false;
	}
	
	TiXmlHandle hDoc(&doc);
	TiXmlElement* pElem = NULL;
	TiXmlHandle hRoot(0);
	
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
	}	
	
	return true;
}

void Config::loadProxySettings(TiXmlElement *pElement)
{


}