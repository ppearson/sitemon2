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

#ifndef HTTP_ENGINE_H
#define HTTP_ENGINE_H

#include <curl/curl.h>

#include "http_request.h"
#include "http_response.h"

static size_t writeBodyData(void *buffer, size_t size, size_t nmemb, void *userp);
static size_t writeHeaderData(void *buffer, size_t size, size_t nmemb, void *userp);
static int debugFunction(CURL *handle, curl_infotype type, unsigned char *data, size_t size, void *userp);

static std::string buildParametersString(const HTTPRequest &request);

class HTTPEngine
{
public:
	HTTPEngine(bool debug = false);
	~HTTPEngine();

	bool performRequest(const HTTPRequest &request, HTTPResponse &response);

	std::string extractedItem(const std::string &name) { return m_aExtractedItems[name]; }

	void setRequestHeader(const std::string &value) { m_debugRequestHeader = value; }

	std::string getRequestHeader() { return m_debugRequestHeader; }

protected:
	bool setupCURLHandleFromRequest(CURL *handle, const HTTPRequest &request);
	bool extractResponseFromCURLHandle(CURL *handle, HTTPResponse &response);

	void downloadContent(CURL *mainHandle, HTTPResponse &response, bool acceptCompressed);

	void processExtractionItems(const HTTPRequest &request, HTTPResponse &response);

protected:
	CURL *	m_handle;

	bool	m_debug;

	// these are member variables because the parameters and cookies strings don't seem
	// to be copied by CURL, so they have to stay in scope otherwise things crash in multithreaded
	// scenarios
	std::string		m_parametersString;
	std::string		m_url;
	std::string		m_referrer;
	std::string		m_cookies;

	std::map<std::string, std::string>	m_aExtractedItems;


	std::string		m_debugRequestHeader;
	std::string		m_debugResponseHeader;

};

#endif
