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

#ifndef URI_HELPER_H
#define URI_HELPER_H

#include <string>
#include <deque>

class URIBuilder
{
public:
	URIBuilder(const std::string &base, const std::string &relative);
	
	void fixRelative();
	std::string getFullLocation();
	
	std::string getHostname() const;
	std::string getProtocolAndHostname() const;
	
protected:
	std::string		m_base;
	std::string		m_relative;
	
	std::string		m_hostname;
	
	std::deque<std::string> m_aParts;
};


// more optimised version which is used for more minimal use-cases...
class URIBuilderFast
{
public:
	URIBuilderFast(const std::string& base);
	
	void setBase(const std::string& base);

	std::string getFullLocation(const std::string& relative) const;
	
protected:
	std::string		m_base;
	
	std::deque<std::string>	m_aParts;
	
	std::string		m_hostname;
	std::string		m_fullLocationStart;
	
	bool			m_baseNotRelative;
	bool			m_secure;
	bool			m_error; // no hostname
};


#endif
