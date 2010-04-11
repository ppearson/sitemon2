#ifndef CONFIG_H
#define CONFIG_H

#include "utils/tinyxml.h"

class Config
{
public:
	Config() { }
	
	bool loadConfigFile(const std::string &configFilePath = "");
	
	std::string	getWebContentPath() { return m_webContentPath; }
	std::string	getDBPath() { return m_dbPath; }
	
protected:
	std::string		m_configFilePath;
	
	std::string		m_webContentPath;
	std::string		m_dbPath;
	
};


#endif