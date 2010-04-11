/*
 *  http_request.cpp
 *  sitemon
 *
 *  Created by Peter Pearson on 27/02/2010.
 *  Copyright 2010 __MyCompanyName__. All rights reserved.
 *
 */

#include "http_request.h"

HTTPRequest::HTTPRequest() : m_requestType(HTTP_GET), m_pauseTime(0), m_acceptCompressed(false), m_storeHeader(true), m_storeBody(true),
								m_downloadContent(false)
{

}

HTTPRequest::HTTPRequest(const std::string &url) : m_url(url), m_requestType(HTTP_GET), m_pauseTime(0), m_acceptCompressed(false), m_storeHeader(true), m_storeBody(true),
								m_downloadContent(false)
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