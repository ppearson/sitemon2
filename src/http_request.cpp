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

HTTPRequest::HTTPRequest() : m_requestType(HTTP_GET), m_pauseTime(0), m_acceptCompressed(false), m_storeHeader(true),
								m_storeBody(true), m_downloadContent(false), m_connectTimeout(30)
{

}

HTTPRequest::HTTPRequest(const std::string &url) : m_url(url), m_requestType(HTTP_GET), m_pauseTime(0), m_acceptCompressed(false), m_storeHeader(true), m_storeBody(true),
								m_downloadContent(false), m_connectTimeout(30)
{

}

void HTTPRequest::addParameter(std::string &name, std::string &value)
{
	HTTPParameter param;
	param.name = name;
	param.value = value;

	m_aParameters.push_back(param);
}

void HTTPRequest::addCookie(std::string &name, std::string &value)
{
	HTTPCookie param;
	param.name = name;
	param.value = value;

	m_aCookies.push_back(param);
}