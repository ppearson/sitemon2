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