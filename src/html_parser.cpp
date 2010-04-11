// primitive HTML parser - at some point would be nice to re-write this to handle content coming back progressively
// and use callbacks to add components instead of needing the whole HTML content before it starts

#include "html_parser.h"
#include "uri_helper.h"
#include "utils/string_helper.h"

HTMLParser::HTMLParser(const std::string &content, const std::string &currentPath) : m_content(content)
{
	int lastSlash = currentPath.rfind("/");
	
	if (lastSlash != currentPath.size() - 1)
		m_currentPath = currentPath.substr(0, lastSlash);
	else
		m_currentPath = currentPath;
}

bool HTMLParser::parse()
{
	int pos = 0;
	int tagStart = 0;
	
	int tagNameEnd = 0;
	
	while ((tagStart = m_content.find("<", pos)) != -1)
	{
		tagStart++;
		tagNameEnd = m_content.find(" ", tagStart);
		
		if (tagNameEnd != -1)
		{
			std::string tagName = m_content.substr(tagStart, tagNameEnd - tagStart);
			toLower(tagName);
			
			int tagEnd = m_content.find(">", tagStart);
			
			if (tagEnd != -1)
			{
				const std::string &tagContent = m_content.substr(tagStart, tagEnd - tagStart);
			
				if (tagName == "img")
				{
					std::string path;
					if (extractQuotedAttribute(tagContent, "src", path))
					{						
						URIBuilder uriBuilder(m_currentPath, path);					
						std::string fullPath = uriBuilder.getFullLocation();
						
						if (!m_aImages.count(fullPath))
						{
							m_aImages.insert(fullPath);
						}
					}
				}
				else if (tagName == "script")
				{
					std::string path;
					if (extractQuotedAttribute(tagContent, "src", path))
					{						
						URIBuilder uriBuilder(m_currentPath, path);					
						std::string fullPath = uriBuilder.getFullLocation();
						
						if (!m_aScripts.count(fullPath))
						{
							m_aScripts.insert(fullPath);
						}
					}
				}
				
				pos = tagEnd;
			}
		}
		else
		{
			// unmatched tag, so break out
			break;
		}
	}	
	
	return true;
}

bool HTMLParser::extractQuotedAttribute(const std::string &tagContent, const std::string &name, std::string &value)
{
	int attributeStart = tagContent.find(name, 0);
	
	if (attributeStart == -1)
		return false;
	
	int afterAttributePos = attributeStart + name.size();

	// need to cope with single and double quotes, and potentially spaces either
	// side of the equals...
	int quoteStartPos = -1;
	int quoteEndPos = -1;
	
	int singleQuote = tagContent.find("'", afterAttributePos);
	int doubleQuote = tagContent.find("\"", afterAttributePos);
	
	if (singleQuote == -1 && doubleQuote == -1)
		return false;
	
	if (singleQuote != -1 && doubleQuote == -1) // only a single quote found
	{
		quoteStartPos = singleQuote + 1;
		quoteEndPos = tagContent.find("'", quoteStartPos);
	}
	else if (doubleQuote != -1 && singleQuote == -1) // only a double quote found
	{
		quoteStartPos = doubleQuote + 1;
		quoteEndPos = tagContent.find("\"", quoteStartPos);
	}
	else // both quotes are found! - use first, and hope the ending one matches the first
	{
		if (singleQuote < doubleQuote)
		{
			quoteStartPos = singleQuote + 1;
			quoteEndPos = tagContent.find("'", quoteStartPos);
		}
		else
		{
			quoteStartPos = doubleQuote + 1;
			quoteEndPos = tagContent.find("\"", quoteStartPos);
		}
	}
	
	value = tagContent.substr(quoteStartPos, quoteEndPos - quoteStartPos);	
	
	return true;
}