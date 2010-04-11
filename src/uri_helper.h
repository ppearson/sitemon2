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
	
protected:
	std::string		m_base;
	std::string		m_relative;
	
	std::string		m_hostname;
	
	std::deque<std::string> m_aParts;
};


#endif