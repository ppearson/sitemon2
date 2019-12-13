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

#include "http_response.h"

HTTPComponentResponse::HTTPComponentResponse() : contentSize(0), downloadSize(0), errorCode(HTTP_OK), responseCode(0),
									lookupTime(0.0), connectTime(0.0), dataStartTime(0.0), totalTime(0.0)
{
	
}

HTTPResponse::HTTPResponse(bool storeHeader, bool storeBody) : contentSize(0), downloadSize(0), totalContentSize(0), totalDownloadSize(0), m_storeHeader(storeHeader), m_storeBody(storeBody),
															errorCode(HTTP_OK), responseCode(0), redirectCount(0), redirectTime(0.0), componentProblem(false),
															lookupTime(0.0), connectTime(0.0), dataStartTime(0.0), dataTransferTime(0.0),
															totalTime(0.0), m_thread(0), m_repeat(0), componentContentSize(0),
															componentDownloadSize(0)
{

}

void HTTPResponse::addComponent(HTTPComponentResponse &component)
{
	if (component.errorCode == HTTP_OK)
	{
		if (component.responseCode == 200 || component.responseCode == 301 || component.responseCode == 302)
		{
			componentContentSize += component.contentSize;
			componentDownloadSize += component.downloadSize;
		}
		else
		{
			componentProblem = true;
		}
	}
	else
	{
		fprintf(stderr, "Had component problem with EC: %i\n", component.errorCode);
		if (component.errorCode == HTTP_UNKNOWN_ERROR)
		{
			fprintf(stderr, "Component Curl error: %s\n", component.errorString.c_str());
		}
		componentProblem = true;
	}	
		
	m_aComponents.push_back(component);
}

