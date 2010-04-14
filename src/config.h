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