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

#include "load_test_settings.h"

LoadTestSettings::LoadTestSettings()
{
	
}

void LoadTestSettings::addProfile(LoadTestProfileSeg &seg)
{
	m_aProfileSegments.push_back(seg);
}

bool LoadTestSettings::loadLoadTestElement(TiXmlElement *pElement)
{
	for (TiXmlElement *pItem = pElement->FirstChildElement(); pItem; pItem = pItem->NextSiblingElement())
	{
		const std::string elementName = pItem->ValueStr();
		
		std::string content;
		if (pItem->GetText())
			content = pItem->GetText();
		
		if (elementName == "type")
		{
			if (content == "profile")
			{
				m_type = LOAD_PROFILE_TEST;
			}
			else if (content == "hit")
			{
				m_type = LOAD_HIT_TEST;
			}
		}
		else if (elementName == "segments")
		{
			loadSegmentsElement(pItem);
		}
		else if (elementName == "threads")
		{
			if (!content.empty())
			{
				m_threads = atoi(content.c_str());
			}
		}
		else if (elementName == "repeats")
		{
			if (!content.empty())
			{
				m_repeats = atoi(content.c_str());
			}
		}
	}
	
	return true;
}

void LoadTestSettings::loadSegmentsElement(TiXmlElement *pElement)
{
	for (TiXmlElement *pItem = pElement->FirstChildElement(); pItem; pItem = pItem->NextSiblingElement())
	{
		if (pItem->ValueStr() == "seg")
		{
			std::string content;
			if (pItem->GetText())
				content = pItem->GetText();
			
			std::string threadsAttr = pItem->Attribute("threads");
			
			if (!content.empty() && !threadsAttr.empty())
			{
				int threads = atoi(threadsAttr.c_str());
				int duration = atoi(content.c_str());
				
				LoadTestProfileSeg newSeg(threads, duration);
				
				addProfile(newSeg);
			}
		}
	}	
}
