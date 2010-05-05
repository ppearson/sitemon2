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

#include "uri_helper.h"
#include "utils/string_helper.h"

URIBuilder::URIBuilder(const std::string &base, const std::string &relative) : m_base(base), m_relative(relative)
{
	toLower(m_base);
//	toLower(m_relative);
	
	fixRelative();
}

std::string URIBuilder::getFullLocation()
{
	if (m_relative.substr(0, 7) == "http://" || m_relative.substr(0, 8) == "https://")
	{
		return m_relative;
	}
	
	if (m_base.substr(0, 7) != "http://" && m_base.substr(0, 8) != "https://")
		return "";
	
	bool bSecure = false;
	
	if (m_base.find("https://") != -1)
		bSecure = true;
	
	const std::string &itemsString = bSecure ? m_base.substr(8) : m_base.substr(7);
	
	std::vector<std::string> aParts;
	
	split(itemsString, aParts, "/");
	
	if (aParts.empty()) // not even a hostname specified
		return "";
	
	// first part is hostname
	m_hostname = aParts[0];
	
	std::string fullLocation;
	
	if (bSecure)
	{
		fullLocation = "https://" + m_hostname;
	}
	else
	{
		fullLocation = "http://" + m_hostname;
	}
	
	std::copy(aParts.begin() + 1, aParts.end(), std::inserter(m_aParts, m_aParts.end()));
	
	// check that relative path isn't actually a full path
	std::string relative;
	int partStart = 0;
	if (m_relative.substr(0, 1) == "/")
	{
		fullLocation += m_relative;
		return fullLocation;
	}
	else
	{
		relative = m_relative;
	}
	
	// now we need to do the same for the relative path
	
	std::vector<std::string> aRelativeParts;
	split(relative, aRelativeParts, "/");
	
	std::vector<std::string>::iterator it = aRelativeParts.begin() + partStart;
	std::vector<std::string>::iterator itEnd = aRelativeParts.end();
	for (; it != itEnd; ++it)
	{
		std::string &part = *it;
		
		if (part == "..")
		{
			// need to make sure we can pop them
			if (m_aParts.empty())
				return m_base;
				
			m_aParts.pop_back();
		}
		else
			m_aParts.push_back(*it);
	}	
	
	std::deque<std::string>::iterator itFinal = m_aParts.begin();
	std::deque<std::string>::iterator itFinalEnd = m_aParts.end();
	for (; itFinal != itFinalEnd; ++itFinal)
	{
		std::string &part = *itFinal;
		
		fullLocation += "/";
		fullLocation += part;				
	}
	
	return fullLocation;
}

void URIBuilder::fixRelative()
{
	// replace &amp with &

	int pos = 0;
	while ((pos = m_relative.find("&amp;", pos)) != -1)
	{
		m_relative.replace(pos, 5, "&");
	}
}