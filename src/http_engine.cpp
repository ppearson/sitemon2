/*
 *  http_engine.cpp
 *  sitemon
 *
 *  Created by Peter Pearson on 27/02/2010.
 *  Copyright 2010 __MyCompanyName__. All rights reserved.
 *
 */

#include <ctime>
#include "http_engine.h"
#include <iostream>

#include "html_parser.h"
#include "component_downloader.h"

// when creating threads, we want the CURL handle within the thread to create the handle
// so init it later on
HTTPEngine::HTTPEngine(bool threaded)
{
	if (!threaded)
	{
		m_handle = curl_easy_init();
	}
}

void HTTPEngine::initCURLHandle()
{
	m_handle = curl_easy_init();
}

bool HTTPEngine::setupCURLHandleFromRequest(CURL *handle, HTTPRequest &request)
{
	if (!handle)
		return false;
	
	if (curl_easy_setopt(handle, CURLOPT_FOLLOWLOCATION, 1L) != 0)
		return false;
	
	if (curl_easy_setopt(handle, CURLOPT_MAXREDIRS, 12L) != 0)
		return false;
	
	if (curl_easy_setopt(handle, CURLOPT_USERAGENT, "Mozilla/5.0") != 0)//"SiteMon" "/" "0.5") != 0)
		return false;

	std::string url = request.getUrl();	

	if (request.hasParameters())
	{
		std::string params = buildParametersString(request);

		if (request.getRequestType() == HTTP_POST)
		{
			if (curl_easy_setopt(handle, CURLOPT_POST, 1L) != 0)
				return false;

			if (curl_easy_setopt(handle, CURLOPT_POSTFIELDS, params.c_str()) != 0)
				return false;
		}
		else
		{
			url += "?";
			url += params;
		}
	}
	
	if (curl_easy_setopt(handle, CURLOPT_URL, url.c_str()) != 0)
		return false;
	
	// blank to enable CURL's cookie handler
	curl_easy_setopt(handle, CURLOPT_COOKIEFILE, "");

	if (request.hasCookies())
	{
		std::string cookies;
		std::vector<HTTPCookie>::iterator it = request.cookies_begin();

		for (; it != request.cookies_end(); ++it)
		{
			cookies += (*it).name;
			cookies += "=";
			cookies += (*it).value;
			cookies += "; ";
		}

		if (curl_easy_setopt(handle, CURLOPT_COOKIE, cookies.c_str()) != 0)
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
	
	curl_easy_getinfo(handle, CURLINFO_SIZE_DOWNLOAD, &download);
	response.downloadSize = (long)download;
	
	response.totalContentSize = response.contentSize;
	response.totalDownloadSize = response.downloadSize;
	
	curl_easy_getinfo(handle, CURLINFO_CONTENT_TYPE, &content_type);
	response.contentType.assign(content_type);
	
	curl_easy_getinfo(handle, CURLINFO_EFFECTIVE_URL, &actual_url);
	response.finalURL.assign(actual_url);
	
	return true;
}

bool HTTPEngine::performRequest(HTTPRequest &request, HTTPResponse &response)
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
	
	curl_easy_setopt(m_handle, CURLOPT_NOSIGNAL, 1L);

	response.requestedURL = request.getUrl();
	
	time_t timeNow;
	time(&timeNow);
	
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

		curl_easy_cleanup(m_handle);

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
				return false; // for things like scripts, we want to fail here
			}
		}
	}
	
	if (request.getDownloadContent())
	{
		downloadContent(m_handle, response, request.getAcceptCompressed());
	}
	
	curl_easy_cleanup(m_handle);

	return true;
}

void HTTPEngine::downloadContent(CURL *mainHandle, HTTPResponse &response, bool acceptCompressed)
{
	HTMLParser parser(response.content, response.finalURL);
	parser.parse();
	
	std::set<std::string> &aScripts = parser.getScripts();
	std::set<std::string> &aImages = parser.getImages();
	
	ComponentDownloader compDownloader(mainHandle, response, acceptCompressed);
	
	std::set<std::string>::iterator it = aScripts.begin();
	for (; it != aScripts.end(); ++it)
	{
		const std::string &url = *it;
		
		compDownloader.addURL(url);
	}
	
	it = aImages.begin();
	for (; it != aImages.end(); ++it)
	{
		const std::string &url = *it;
		
		compDownloader.addURL(url);
	}
	
	compDownloader.downloadComponents();
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

static std::string buildParametersString(HTTPRequest &request)
{
	std::string params;

	std::vector<HTTPParameter>::iterator it = request.params_begin();

	for (; it != request.params_end(); ++it)
	{
		params += (*it).name;
		params += "=";
		params += (*it).value;
		params += "&";
	}

	// strip off the trailing &
	if (params.size() > 3)
	{
		params = params.substr(0, params.size() - 1);
	}

	return params;
}