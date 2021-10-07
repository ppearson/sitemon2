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

#ifndef EXTRACTION_ITEM_H
#define EXTRACTION_ITEM_H

#include <string>

class ExtractionItem
{
public:
	ExtractionItem(const std::string &name, const std::string &startText, const std::string &endText, int itemNum = 0) :
			m_name(name), m_startText(startText), m_endText(endText), m_itemNum(itemNum)
	{

	}

	const std::string& getName() const { return m_name; }
	const std::string& getStartText() const { return m_startText; }
	const std::string& getEndText() const { return m_endText; }
	int				getItemNum() const { return m_itemNum; }

protected:
	std::string		m_name;
	std::string		m_startText;
	std::string		m_endText;
	int				m_itemNum;
};

#endif


