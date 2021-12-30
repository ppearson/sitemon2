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

#ifndef HTTP_RESPONSE_H
#define HTTP_RESPONSE_H

#include <string>
#include <vector>

#include "utils/time.h"

enum HTTPResponseError
{
	HTTP_OK = 0,
	HTTP_OK_MISSING_COMPONENTS = 1,
	HTTP_COULDNT_RESOLVE_HOST = -1,
	HTTP_COULDNT_CONNECT = -2,
	HTTP_TIMEOUT = -3,
	HTTP_RECV_ERROR = -4,
	HTTP_EXPECTED_PHRASE_NOT_FOUND = -5,
	HTTP_UNKNOWN_ERROR = -20
};

enum class HTTPResponseVersion
{
	eUnknown,
	e1_0,
	e1_1,
	e2_0,
	e3_0
};

class HTTPComponentResponse
{
public:
	HTTPComponentResponse();
	
	std::string errorString;
	
	std::string	requestedURL;
	std::string	finalURL;
	
	std::string	contentType;
	std::string	contentEncoding;

	Time		timestamp;
	
	double		lookupTime;
	double		connectTime;
	double		dataStartTime;
	double		totalTime;
	
	HTTPResponseError errorCode;
	long		responseCode;
	
	long		contentSize;
	long		downloadSize;
};	

class HTTPResponse
{
public:
	HTTPResponse(bool storeHeader = true, bool storeBody = true);
	
	std::string errorString;
	
	std::string	requestedURL;
	std::string	finalURL;

	std::string	content;
	std::string	header;
	
	std::string	contentType;
	std::string	contentEncoding;
	std::string	server;
	
	Time		timestamp;
	
	double		lookupTime;
	double		connectTime;
	double		sslHandshakeTime;
	double		dataStartTime;
	double		dataTransferTime;
	double		totalTime;

	double		redirectTime;

	HTTPResponseError errorCode;
	long		responseCode;
	
	HTTPResponseVersion	httpVersion;
	
	long		redirectCount;
	
	long		contentSize;
	long		downloadSize;
	
	long		componentContentSize;
	long		componentDownloadSize;
	
	long		totalContentSize;
	long		totalDownloadSize;
	
	int		m_thread;
	int		m_repeat;
	bool	m_storeHeader;
	bool	m_storeBody;
	
	bool	componentProblem;
	
	void addComponent(const HTTPComponentResponse &component);
	std::vector<HTTPComponentResponse>& getComponents() { return m_aComponents; }
	const std::vector<HTTPComponentResponse>& getComponents() const { return m_aComponents; }
	
protected:
	std::vector<HTTPComponentResponse> m_aComponents;
};

#endif
