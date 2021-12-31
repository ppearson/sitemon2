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

#include "http_request.h"

HTTPRequest::HTTPRequest() : m_requestType(HTTP_GET), m_connectTimeout(30), m_totalTimeout(60), m_pauseTime(0),
            m_acceptCompressed(false), m_downloadContent(false), m_storeHeader(true), m_storeBody(true) 
{

}

HTTPRequest::HTTPRequest(const HTTPRequest &request)
{
	m_requestType = request.m_requestType;

	m_connectTimeout = request.m_connectTimeout;
	m_totalTimeout = request.m_totalTimeout;

	m_description = request.m_description;

	m_url = request.m_url;
	m_host = request.m_host;
	m_referrer = request.m_referrer;

	m_acceptMain = request.m_acceptMain;
	m_acceptLanguage = request.m_acceptLanguage;
	m_acceptEncoding = request.m_acceptEncoding;
	m_acceptCharset = request.m_acceptCharset;

	m_contentType = request.m_contentType;

	m_acceptCompressed = request.m_acceptCompressed;
	m_downloadContent = request.m_downloadContent;

	m_expectedPhrase = request.m_expectedPhrase;

	m_storeHeader = request.m_storeHeader;
	m_storeBody = request.m_storeBody;

	m_pauseTime = request.m_pauseTime;

	m_aParameters = request.m_aParameters;
	m_aCookies = request.m_aCookies;

	std::vector<DynamicParameter *>::const_iterator it = request.m_aDynamicParameters.begin();
	std::vector<DynamicParameter *>::const_iterator itEnd = request.m_aDynamicParameters.end();
	for (; it != itEnd; ++it)
	{
		DynamicParameter *pCopy = (*it)->clone();

		m_aDynamicParameters.push_back(pCopy);
	}
	
	m_aExtractionItems = request.m_aExtractionItems;
}

HTTPRequest::HTTPRequest(const std::string &url) : m_url(url), m_requestType(HTTP_GET), m_connectTimeout(30), m_totalTimeout(240), m_pauseTime(0),
				m_acceptCompressed(false), m_downloadContent(false),
				m_storeHeader(true), m_storeBody(true)

{

}

HTTPRequest::HTTPRequest(const std::string &url, const std::string &description, bool post) : m_url(url), m_description(description), m_connectTimeout(30), m_totalTimeout(240),
					m_pauseTime(0), m_acceptCompressed(false), m_downloadContent(false), m_storeHeader(true), m_storeBody(true)
{
	if (post)
	{
		m_requestType = HTTP_POST;
	}
	else
	{
		m_requestType = HTTP_GET;
	}
}

HTTPRequest::~HTTPRequest()
{
	
}

HTTPRequest& HTTPRequest::operator=(const HTTPRequest& rhs)
{
	m_requestType = rhs.m_requestType;
	
	m_connectTimeout = rhs.m_connectTimeout;
	m_totalTimeout = rhs.m_totalTimeout;
	
	m_description = rhs.m_description;
	
	m_url = rhs.m_url;
	m_host = rhs.m_host;
	m_referrer = rhs.m_referrer;
	
	m_acceptMain = rhs.m_acceptMain;
	m_acceptLanguage = rhs.m_acceptLanguage;
	m_acceptEncoding = rhs.m_acceptEncoding;
	m_acceptCharset = rhs.m_acceptCharset;
	
	m_contentType = rhs.m_contentType;
	
	m_acceptCompressed = rhs.m_acceptCompressed;
	m_downloadContent = rhs.m_downloadContent;
	
	m_expectedPhrase = rhs.m_expectedPhrase;
	
	m_storeHeader = rhs.m_storeHeader;
	m_storeBody = rhs.m_storeBody;
	
	m_pauseTime = rhs.m_pauseTime;
	
	m_aParameters = rhs.m_aParameters;
	m_aCookies = rhs.m_aCookies;
	
	std::vector<DynamicParameter *>::const_iterator it = rhs.m_aDynamicParameters.begin();
	std::vector<DynamicParameter *>::const_iterator itEnd = rhs.m_aDynamicParameters.end();
	
	m_aDynamicParameters.clear();
	
	for (; it != itEnd; ++it)
	{
		DynamicParameter *pCopy = (*it)->clone();
		
		m_aDynamicParameters.push_back(pCopy);
	}
	
	m_aExtractionItems = rhs.m_aExtractionItems;
	
	return *this;
}

void HTTPRequest::addParameter(const std::string &name, const std::string &value)
{
	m_aParameters[name] = value;
}

void HTTPRequest::addCookie(const std::string &name, const std::string &value)
{
	HTTPCookie param;
	param.name = name;
	param.value = value;

	m_aCookies.push_back(param);
}

void HTTPRequest::addDynamicParameter(DynamicParameter *pDynamicParameter)
{
	m_aDynamicParameters.push_back(pDynamicParameter);
}

void HTTPRequest::processDynamicParameters(HTTPEngine &engine)
{
	std::vector<DynamicParameter *>::iterator it = m_aDynamicParameters.begin();
	std::vector<DynamicParameter *>::iterator itEnd = m_aDynamicParameters.end();
	for (; it != itEnd; ++it)
	{
		DynamicParameter *pDynParam = *it;
		if (pDynParam)
		{
			std::string name = pDynParam->getName();
			std::string value = pDynParam->getValue(engine);
			
			addParameter(name, value);			
		}		
	}
}

void HTTPRequest::cleanupDynamicParameters()
{
	// clear up and delete any dynamic parameters
	
	std::vector<DynamicParameter *>::iterator it = m_aDynamicParameters.begin();
	std::vector<DynamicParameter *>::iterator itEnd = m_aDynamicParameters.end();
	for (; it != itEnd; ++it)
	{
		DynamicParameter *pDynParam = *it;
		if (pDynParam)
		{
			delete pDynParam;
			pDynParam = NULL;
		}		
	}
	
	m_aDynamicParameters.clear();
}

void HTTPRequest::addExtractionItem(const ExtractionItem &item)
{
	m_aExtractionItems.push_back(item);
}
