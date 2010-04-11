/*
 *  http_response.h
 *  sitemon
 *
 *  Created by Peter Pearson on 27/02/2010.
 *  Copyright 2010 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef HTTP_RESPONSE_H
#define HTTP_RESPONSE_H

#include <string>
#include <vector>

enum HTTPResponseError
{
	HTTP_OK = 0,
	HTTP_COULDNT_RESOLVE_HOST = -1,
	HTTP_COULDNT_CONNECT = -2,
	HTTP_TIMEOUT = -3,
	HTTP_RECV_ERROR = -4,
	HTTP_EXPECTED_PHRASE_NOT_FOUND = -5,
	HTTP_UNKNOWN_ERROR = -20
};

class HTTPComponentResponse
{
public:
	HTTPComponentResponse();
	
	time_t		timestamp;
	
	HTTPResponseError errorCode;
	long		responseCode;
	
	std::string errorString;
	
	double		lookupTime;
	double		connectTime;
	double		dataStartTime;
	double		totalTime;
	
//	double		redirectTime;
//	long		redirectCount;
	
	std::string		requestedURL;
	std::string		finalURL;
	
	long		contentSize;
	long		downloadSize;
	
	std::string		contentType;
	std::string		contentEncoding;
};	

class HTTPResponse
{
public:
	HTTPResponse(bool storeHeader = true, bool storeBody = true);
	
	time_t		timestamp;

	HTTPResponseError errorCode;
	long		responseCode;

	std::string errorString;
	
	double		lookupTime;
	double		connectTime;
	double		dataStartTime;
	double		totalTime;

	double		redirectTime;
	long		redirectCount;
	
	std::string		requestedURL;
	std::string		finalURL;

	std::string		content;
	std::string		header;

	long		contentSize;
	long		downloadSize;
	
	long		totalContentSize;
	long		totalDownloadSize;

	std::string		contentType;
	std::string		contentEncoding;
	std::string		server;
	
	int		m_thread;
	bool	m_storeHeader;
	bool	m_storeBody;
	
	bool	componentProblem;
	
	void addComponent(HTTPComponentResponse &component);
	std::vector<HTTPComponentResponse> &getComponents() { return m_aComponents; }
	
protected:
	std::vector<HTTPComponentResponse> m_aComponents;
};

#endif