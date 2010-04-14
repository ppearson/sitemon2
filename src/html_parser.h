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

#ifndef HTML_PARSER_H
#define HTML_PARSER_H

#include <string>
#include <set>

class HTMLParser
{
public:
	HTMLParser(const std::string &content, const std::string &currentPath);
	~HTMLParser() { }
	
	bool parse();
	
	std::set<std::string> &getImages() { return m_aImages; }
	std::set<std::string> &getScripts() { return m_aScripts; }
	
protected:
	bool extractQuotedAttribute(const std::string &tagContent, const std::string &name, std::string &value);

protected:
	std::string		m_content;
	std::string		m_currentPath;
	
	std::set<std::string> m_aImages;
	std::set<std::string> m_aScripts;
};

#endif