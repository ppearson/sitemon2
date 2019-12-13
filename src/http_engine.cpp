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

#include "http_engine.h"

#include <iostream>
#include "utils/time.h"
#include "html_parser.h"
#include "component_downloader.h"

//static const char* kUserAgent = "Sitemon/0.66";
//static const char* kUserAgent = "Mozilla/5.0 (Windows; U; Windows NT 6.0; en-US) AppleWebKit/533.4 (KHTML, like Gecko) Chrome/5.0.375.125 Safari/533.4";
//static const char* kUserAgent = "Mozilla/5.0 (Windows NT 6.0) AppleWebKit/535.2 (KHTML, like Gecko) Chrome/15.0.874.120 Safari/535.2";
static const char* kUserAgent = "Mozilla/5.0 (X11; Linux x86_64) AppleWebKit/537.36 (KHTML, like Gecko) Ubuntu Chromium/63.0.3239.84 Chrome/63.0.3239.84 Safari/537.36";

HTTPEngine::HTTPEngine(bool debug) : m_debug(debug)
{
	m_handle = curl_easy_init();
}

HTTPEngine::~HTTPEngine()
{
	if (m_handle)
		curl_easy_cleanup(m_handle);
}

bool HTTPEngine::setupCURLHandleFromRequest(CURL *handle, const HTTPRequest &request)
{
	if (!handle)
		return false;

	if (curl_easy_setopt(handle, CURLOPT_FOLLOWLOCATION, 1L) != 0)
		return false;

	if (curl_easy_setopt(handle, CURLOPT_MAXREDIRS, 12L) != 0)
		return false;

	if (curl_easy_setopt(handle, CURLOPT_USERAGENT, kUserAgent) != 0)
		return false;

	m_url = request.getUrl();

	// ugly as hell, but...
	HTTPRequest& nonConstRequest = const_cast<HTTPRequest&>(request);
	nonConstRequest.processDynamicParameters(*this);

	if (request.hasParameters())
	{
		m_parametersString = buildParametersString(request);

		if (request.getRequestType() == HTTP_POST)
		{
			if (curl_easy_setopt(handle, CURLOPT_POST, 1L) != 0)
				return false;

			if (curl_easy_setopt(handle, CURLOPT_POSTFIELDS, m_parametersString.c_str()) != 0)
				return false;
		}
		else
		{
			m_url += "?";
			m_url += m_parametersString;
		}
	}

	if (curl_easy_setopt(handle, CURLOPT_URL, m_url.c_str()) != 0)
		return false;

	m_referrer = request.getReferrer();
	if (!m_referrer.empty())
	{
		curl_easy_setopt(handle, CURLOPT_REFERER, m_referrer.c_str());
	}

	curl_easy_setopt(handle, CURLOPT_CONNECTTIMEOUT, request.getConnectTimeout());

	curl_easy_setopt(handle, CURLOPT_TIMEOUT, request.getTotalTimeout());

	// blank to enable CURL's cookie handler
	curl_easy_setopt(handle, CURLOPT_COOKIEFILE, "");

	if (request.hasCookies())
	{
		std::vector<HTTPCookie>::const_iterator it = request.cookies_begin();
		std::vector<HTTPCookie>::const_iterator itEnd = request.cookies_end();

		m_cookies = "";
		for (; it != itEnd; ++it)
		{
			m_cookies += (*it).name;
			m_cookies += "=";
			m_cookies += (*it).value;
			m_cookies += "; ";
		}

		if (curl_easy_setopt(handle, CURLOPT_COOKIE, m_cookies.c_str()) != 0)
			return false;
	}

	if (request.getAcceptCompressed())
	{
		if (curl_easy_setopt(handle, CURLOPT_ENCODING, "gzip, deflate") != 0)
			return false;
	}

	return true;
}

bool HTTPEngine::extractResponseFromCURLHandle(CURL *handle, HTTPResponse &response)
{
	char *actual_url = 0;
	char *content_type = 0;
	double download = 0.0;

	curl_easy_getinfo(handle, CURLINFO_RESPONSE_CODE, &response.responseCode);

	curl_easy_getinfo(handle, CURLINFO_TOTAL_TIME, &response.totalTime);
	curl_easy_getinfo(handle, CURLINFO_NAMELOOKUP_TIME, &response.lookupTime);
	curl_easy_getinfo(handle, CURLINFO_CONNECT_TIME, &response.connectTime);
	curl_easy_getinfo(handle, CURLINFO_STARTTRANSFER_TIME, &response.dataStartTime);

	curl_easy_getinfo(handle, CURLINFO_REDIRECT_COUNT, &response.redirectCount);
	curl_easy_getinfo(handle, CURLINFO_REDIRECT_TIME, &response.redirectTime);

	// try and get the times on a per-item basis, as opposed to the time since the request was started
/*
	response.dataTransferTime = response.totalTime - response.dataStartTime;
	response.dataStartTime -= response.connectTime;
	response.connectTime -= response.lookupTime;

	if (response.redirectCount > 0)
	{
		response.redirectTime -= response.dataStartTime;
		response.dataTransferTime -= response.redirectTime;
	}
*/


	curl_easy_getinfo(handle, CURLINFO_SIZE_DOWNLOAD, &download);
	response.downloadSize = (long)download;

	response.totalContentSize = response.contentSize;
	response.totalDownloadSize = response.downloadSize;

	curl_easy_getinfo(handle, CURLINFO_CONTENT_TYPE, &content_type);
	response.contentType.assign(content_type);

	// Note: CURL 7.62 changed the behaviour of this...
	curl_easy_getinfo(handle, CURLINFO_EFFECTIVE_URL, &actual_url);
	response.finalURL.assign(actual_url);

	return true;
}

bool HTTPEngine::performRequest(const HTTPRequest &request, HTTPResponse &response)
{
	if (!setupCURLHandleFromRequest(m_handle, request))
		return false;

	if (curl_easy_setopt(m_handle, CURLOPT_WRITEFUNCTION, writeBodyData) != 0)
		return false;

	if (curl_easy_setopt(m_handle, CURLOPT_WRITEDATA, (void *)&response) != 0)
		return false;

	if (curl_easy_setopt(m_handle, CURLOPT_HEADERFUNCTION, writeHeaderData) != 0)
		return false;

	if (curl_easy_setopt(m_handle, CURLOPT_HEADERDATA, (void *)&response) != 0)
		return false;

	if (m_debug)
	{
		if (curl_easy_setopt(m_handle, CURLOPT_DEBUGFUNCTION, debugFunction) != 0)
			return false;

		if (curl_easy_setopt(m_handle, CURLOPT_DEBUGDATA, this) != 0)
			return false;

		curl_easy_setopt(m_handle, CURLOPT_VERBOSE, 1L);
	}

	curl_easy_setopt(m_handle, CURLOPT_NOSIGNAL, 1L);

	// disable SSL security check
	curl_easy_setopt(m_handle, CURLOPT_SSL_VERIFYPEER, 0L);

	response.requestedURL = request.getUrl();

	Time timeNow;
	timeNow.now();

	response.timestamp = timeNow;

	int res = curl_easy_perform(m_handle);
	if (res != 0)
	{
		switch (res)
		{
		case CURLE_COULDNT_RESOLVE_HOST:
			response.errorCode = HTTP_COULDNT_RESOLVE_HOST;
			response.errorString = "Couldn't resolve host.";
			break;
		case CURLE_COULDNT_CONNECT:
			response.errorCode = HTTP_COULDNT_CONNECT;
			response.errorString = "Couldn't connect to host.";
			break;
		case CURLE_OPERATION_TIMEOUTED:
			response.errorCode = HTTP_TIMEOUT;
			response.errorString = "Connection timed out.";
			break;
		case CURLE_RECV_ERROR:
			response.errorCode = HTTP_RECV_ERROR;
			response.errorString = "Error receiving data.";
			break;
		default:
			response.errorCode = HTTP_UNKNOWN_ERROR;
			response.errorString = "Unknown error.";
			break;
		}

		return false;
	}

	extractResponseFromCURLHandle(m_handle, response);

	// handle expected phrase
	if (!response.content.empty()) // if we stored the returned content, test for expected phrase
	{
		const std::string &expectedPhrase = request.getExpectedPhrase();

		if (!expectedPhrase.empty())
		{
			if (response.content.find(expectedPhrase) == -1)
			{
				response.errorCode = HTTP_EXPECTED_PHRASE_NOT_FOUND;
				response.errorString = "Expected phrase not found.";
				return false; // for things like scripts, we want to fail here
			}
		}
	}

	processExtractionItems(request, response);

	if (request.getDownloadContent())
	{
		downloadContent(m_handle, response, request.getAcceptCompressed());

		response.totalDownloadSize += response.componentDownloadSize;
		response.totalContentSize += response.componentContentSize;

		if (response.componentProblem)
		{
			response.errorCode = HTTP_OK_MISSING_COMPONENTS;
		}
	}

	return true;
}

void HTTPEngine::downloadContent(CURL *mainHandle, HTTPResponse &response, bool acceptCompressed)
{
	HTMLParser parser(response.content, response.finalURL);
	parser.parse();

	std::set<std::string>& aScripts = parser.getScripts();
	std::set<std::string>& aImages = parser.getImages();
	std::set<std::string>& aCSS = parser.getCSS();

	ComponentDownloader compDownloader(mainHandle, kUserAgent, response, acceptCompressed);
	
	std::set<std::string>::iterator it = aCSS.begin();
	std::set<std::string>::iterator itEnd = aCSS.end();
	for (; it != itEnd; ++it)
	{
		const std::string& url = *it;
		
		compDownloader.addURL(url);
	}

	it = aScripts.begin();
	itEnd = aScripts.end();
	for (; it != itEnd; ++it)
	{
		const std::string& url = *it;

		compDownloader.addURL(url);
	}

	it = aImages.begin();
	itEnd = aImages.end();
	for (; it != itEnd; ++it)
	{
		const std::string& url = *it;

		compDownloader.addURL(url);
	}	

	compDownloader.downloadComponents();
}

void HTTPEngine::processExtractionItems(const HTTPRequest &request, HTTPResponse &response)
{
	std::vector<ExtractionItem>::const_iterator it = request.extractionItems_begin();
	std::vector<ExtractionItem>::const_iterator itEnd = request.extractionItems_end();

	for (; it != itEnd; ++it)
	{
		const ExtractionItem &extrItem = *it;

		const std::string &content = response.content;

		// we assume that the ExtractionItem has valid parameters

		std::string startText = extrItem.getStartText();
		std::string endText = extrItem.getEndText();

		// todo - need to cope with getting the n'th item
		int itemNum = extrItem.getItemNum();

		int nFindStart = content.find(startText);

		if (nFindStart >= 0)
		{
			int nItemStart = nFindStart + startText.size();

			int nFindEnd = content.find(endText, nItemStart);

			if (nFindEnd >= 0)
			{
				std::string extractedContent = content.substr(nItemStart, nFindEnd - nItemStart);

				std::string	itemName = extrItem.getName();

				m_aExtractedItems[itemName] = extractedContent;
			}
		}
	}
}

static size_t writeBodyData(void *buffer, size_t size, size_t nmemb, void *userp)
{
	if (!userp)
		return 0;

	size_t full_size = size * nmemb;

	HTTPResponse *response = static_cast<HTTPResponse *>(userp);

	if (response->m_storeBody)
	{
		response->content.append(reinterpret_cast<char *>(buffer), full_size);
	}
	response->contentSize += full_size;

	return full_size;
}

static size_t writeHeaderData(void *buffer, size_t size, size_t nmemb, void *userp)
{
	if (!userp)
		return 0;

	size_t full_size = size * nmemb;

	HTTPResponse *response = static_cast<HTTPResponse *>(userp);

	std::string headerLine = reinterpret_cast<char *>(buffer);

	int nColon = headerLine.find(":");

	if (nColon > 0)
	{
		std::string fieldName = headerLine.substr(0, nColon);
		std::string value = headerLine.substr(nColon + 2);

		if (fieldName == "Content-Encoding")
		{
			response->contentEncoding = value;
		}
		else if (fieldName == "Server")
		{
			response->server = value;
		}
	}

	if (response->m_storeHeader)
	{
		response->header += headerLine;
	}

	return full_size;
}

static int debugFunction(CURL *handle, curl_infotype type, unsigned char *data, size_t size, void *userp)
{
	HTTPEngine *pEngine = static_cast<HTTPEngine *>(userp);

	switch (type)
	{
		case CURLINFO_HEADER_OUT:
			if (data)
			{
				std::string header((char *)data, size);
				pEngine->setRequestHeader(header);
			}
			break;
	}

	return 0;
}

static std::string buildParametersString(const HTTPRequest &request)
{
	std::string params;

	std::map<std::string, std::string>::const_iterator it = request.params_begin();
	std::map<std::string, std::string>::const_iterator itEnd = request.params_end();

	for (; it != itEnd; ++it)
	{
		const std::string &name = (*it).first;
		const std::string &value = (*it).second;

		params += name;
		params += "=";
		params += value;
		params += "&";
	}

	// strip off the trailing &
	if (params.size() > 3)
	{
		params = params.substr(0, params.size() - 1);
	}

	return params;
}

