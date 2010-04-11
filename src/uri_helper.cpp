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
	if (m_relative.substr(0, 7) == "http://")
	{
		return m_relative;
	}
	
	if (m_base.substr(0, 7) != "http://")
		return "";
	
	const std::string &itemsString = m_base.substr(7);
	
	std::vector<std::string> aParts;
	
	split(itemsString, aParts, "/");
	
	if (aParts.empty()) // not even a hostname specified
		return "";
	
	// first part is hostname
	m_hostname = aParts[0];
	
	std::string fullLocation = "http://" + m_hostname;
	
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
	for (; it != aRelativeParts.end(); ++it)
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
	for (; itFinal != m_aParts.end(); ++itFinal)
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