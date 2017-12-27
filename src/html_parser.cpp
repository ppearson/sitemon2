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

// primitive HTML parser - at some point would be nice to re-write this to handle content coming back progressively
// and use callbacks to add components instead of needing the whole HTML content before it starts parsing

// if speed (CPU time) is an issue in the future, this will probably need to be re-written to use standard
// char string parsing instead of std::string, as it's probably more efficient

#include "html_parser.h"
#include "uri_helper.h"
#include "utils/string_helper.h"

HTMLParser::HTMLParser(const std::string &content, const std::string &currentPath) : m_content(content)
{
	int lastSlash = currentPath.rfind("/");
	
	if (lastSlash != currentPath.size() - 1 && lastSlash != 6)
		m_currentPath = currentPath.substr(0, lastSlash);
	else
		m_currentPath = currentPath;
}

bool HTMLParser::parse()
{
	int pos = 0;
	int tagStart = 0;
	int tagEnd = 0;
	int tagNameEnd = 0;
	
	while ((tagStart = m_content.find("<", pos)) != -1)
	{
		tagStart++;
		tagNameEnd = m_content.find(" ", tagStart);
		tagEnd = m_content.find(">", tagStart);
		
		if (tagNameEnd != -1 && tagNameEnd < tagEnd)
		{
			std::string tagName = m_content.substr(tagStart, tagNameEnd - tagStart);
			toLower(tagName);
			
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
					
					// we can ignore the stuff within the opening and closing tags, as we don't want to parse this
					
					int finalEnd = m_content.find("</script>", tagEnd);
					
					if (finalEnd != -1)
					{
						tagEnd = finalEnd + 9;
					}
				}
				
				pos = tagEnd;
			}
			else
			{
				// unmatched tag, so break out
				break;
			}
		}
		else
		{
			pos = tagEnd;
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