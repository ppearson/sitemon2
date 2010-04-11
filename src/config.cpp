#ifdef _MSC_VER
#include <direct.h>
#endif

#include "config.h"

bool Config::loadConfigFile(const std::string &configFilePath)
{
	std::string finalPath;
	
	if (!configFilePath.empty())
	{
		finalPath = configFilePath;
	}
	else
	{
		char *szCurrentDir = getcwd(NULL, 0);
		if (szCurrentDir == 0)
		{
			return false;
		}
		
		finalPath = szCurrentDir;
#ifdef _MSC_VER
		finalPath += "\\";
#else
		finalPath += "/";
#endif
		
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
			m_webContentPath = content;
		}
		else if (elementName == "db_path")
		{
			m_dbPath = content;
		}
	}	
	
	return true;
}