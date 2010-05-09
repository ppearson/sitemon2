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

#ifndef CONFIG_H
#define CONFIG_H

#include "utils/tinyxml.h"

struct ProxySettings
{
	std::string		m_proxyHost;
	std::string		m_proxyPort;
	std::string		m_proxyUser;
	std::string		m_proxyPass;	
};

struct ConfigSettings
{
	std::string		m_webContentPath;
	std::string		m_monitoringDBPath;
	std::string		m_loadTestingDBPath;
	
	bool			m_useProxy;
	ProxySettings	m_proxySettings;
	
	//
	
	std::string	getWebContentPath() { return m_webContentPath; }
	std::string	getMonitoringDBPath() { return m_monitoringDBPath; }
	std::string getLoadTestingDBPath() { return m_loadTestingDBPath; }
	
	bool shouldUseProxy() { return m_useProxy; }
	ProxySettings & getProxySettings() { return m_proxySettings; }
};

class Config
{
public:
	Config(ConfigSettings &configSettings);
	
	bool loadConfigFile(const std::string &configFilePath = "");
	
	void loadProxySettings(TiXmlElement *pElement);
	
protected:
	std::string		m_configFilePath;
	ConfigSettings &m_configSettings;
};


#endif