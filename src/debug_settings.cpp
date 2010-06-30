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

#include "debug_settings.h"

DebugSettings::DebugSettings() : m_outputBodyResponse(false), m_veryVerboseOutput(false), m_outputHeaderRequest(false)
{
	
}

DebugSettings::DebugSettings(const std::string &debugOutput) : m_outputBodyResponse(true), m_veryVerboseOutput(false),
																m_outputPath(debugOutput), m_outputHeaderRequest(false)
{
	
}

void DebugSettings::loadDebugElement(TiXmlElement *pElement)
{
	std::string enabled = pElement->Attribute("enabled");
	if (enabled.empty() || enabled == "yes" || enabled == "true")
	{
		for (TiXmlElement *pItem = pElement->FirstChildElement(); pItem; pItem = pItem->NextSiblingElement())
		{
			const std::string &elementName = pItem->ValueStr();
			
			std::string content;
			if (pItem->GetText())
				content = pItem->GetText();
			
			if (elementName == "output_location")
			{
				m_outputPath = content;
			}
			else if (elementName == "output_body_response")
			{
				if (content == "yes" || content == "true")
				{
					m_outputBodyResponse = true;
				}
			}
			else if (elementName == "output_header_request")
			{
				if (content == "yes" || content == "true")
				{
					m_outputHeaderRequest = true;
				}
			}
		}			
	}	
}