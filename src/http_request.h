/*
 *  http_request.h
 *  sitemon
 *
 *  Created by Peter Pearson on 27/02/2010.
 *  Copyright 2010 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef HTTP_REQUEST_H
#define HTTP_REQUEST_H

#include <string>
#include <vector>

enum RequestType
{
	HTTP_GET,
	HTTP_POST
};

enum HTTPAuthType
{
	AUTH_NONE,
	AUTH_BASIC,
	AUTH_DIGEST,
	AUTH_NTLM
};

struct HTTPParameter
{
	std::string name;
	std::string value;
};

struct HTTPCookie
{
	std::string name;
	std::string value;
};

class HTTPRequest
{
public:
	HTTPRequest();
	HTTPRequest(const std::string &url);
	
	const RequestType getRequestType() { return m_requestType; }
	const std::string getDescription() { return m_description; }
	const std::string getUrl() { return m_url; }
	const std::string getHost() { return m_host; }
	const std::string getReferrer() { return m_referrer; }
	const std::string getAcceptMain() { return m_acceptMain; }
	const std::string getAcceptLanguage() { return m_acceptLanguage; }
	const std::string getAcceptEncoding() { return m_acceptEncoding; }
	const std::string getAcceptCharset() { return m_acceptCharset; }
	const std::string getContentType() { return m_contentType; }
	bool getAcceptCompressed() { return m_acceptCompressed; }
	int getPauseTime() { return m_pauseTime; }
	bool getStoreHeader() { return m_storeHeader; }
	bool getStoreBody() { return m_storeBody; }
	const std::string getExpectedPhrase() { return m_expectedPhrase; }
	bool getDownloadContent() { return m_downloadContent; }
	
	void setRequestType(RequestType requestType) { m_requestType = requestType; }
	void setDescription(std::string &description) { m_description = description; }
	void setUrl(std::string &url) { m_url = url; }
	void setHost(std::string &host) { m_host = host; }
	void setReferrer(std::string &referrer) { m_referrer = referrer; }
	void setAcceptMain(std::string &acceptMain) { m_acceptMain = acceptMain; }
	void setAcceptLanguage(std::string &acceptLanguage) { m_acceptLanguage = acceptLanguage; }
	void setAcceptEncoding(std::string &acceptEncoding) { m_acceptEncoding = acceptEncoding; }
	void setAcceptCharset(std::string &acceptCharset) { m_acceptCharset = acceptCharset; }
	void setContentType(std::string &contentType) { m_contentType = contentType; }
	void setAcceptCompressed(bool acceptCompressed) { m_acceptCompressed = acceptCompressed; }
	void setPauseTime(int pauseTime) { m_pauseTime = pauseTime; }
	void setStoreHeader(bool storeHeader) { m_storeHeader = storeHeader; }
	void setStoreBody(bool storeBody) { m_storeBody = storeBody; }
	void setExpectedPhrase(std::string &expectedPhrase) { m_expectedPhrase = expectedPhrase; }
	void setDownloadContent(bool downloadContent) { m_downloadContent = downloadContent; }

	void addParameter(std::string &name, std::string &value);
	void addCookie(std::string &name, std::string &value);

	bool hasParameters() const { return !m_aParameters.empty(); }
	bool hasCookies() const { return !m_aCookies.empty(); }

	inline std::vector<HTTPParameter>::iterator params_begin() { return m_aParameters.begin(); }
	inline std::vector<HTTPParameter>::iterator params_end() { return m_aParameters.end(); }

	inline std::vector<HTTPCookie>::iterator cookies_begin() { return m_aCookies.begin(); }
	inline std::vector<HTTPCookie>::iterator cookies_end() { return m_aCookies.end(); }
	
protected:
	RequestType		m_requestType;

	std::string		m_description;
	
	std::string		m_url;
	std::string		m_host;
	std::string		m_referrer;
	
	std::string		m_acceptMain;
	std::string		m_acceptLanguage;
	std::string		m_acceptEncoding;
	std::string		m_acceptCharset;
	
	std::string		m_contentType;
	
	bool			m_acceptCompressed;
	bool			m_downloadContent;

	std::string		m_expectedPhrase;

	bool			m_storeHeader;
	bool			m_storeBody;

	int				m_pauseTime;
	
	std::vector<HTTPParameter> m_aParameters;
	std::vector<HTTPCookie> m_aCookies;
};

#endif