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

#ifndef HTTP_REQUEST_H
#define HTTP_REQUEST_H

#include <string>
#include <vector>
#include <map>

#include "dynamic_parameters.h"
#include "extraction_item.h"

class HTTPEngine;

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

struct HTTPCookie
{
	std::string name;
	std::string value;
};

class HTTPRequest
{
public:
	HTTPRequest();
	HTTPRequest(const HTTPRequest &request);
	HTTPRequest(const std::string &url);
	HTTPRequest(const std::string &url, const std::string &description, bool post = false);
	~HTTPRequest();

	HTTPRequest& operator=(const HTTPRequest& rhs);

	RequestType getRequestType() const { return m_requestType; }
	std::string getDescription() const { return m_description; }
	std::string getUrl() const { return m_url; }
	std::string getHost() const { return m_host; }
	std::string getReferrer() const { return m_referrer; }
	std::string getAcceptMain() const { return m_acceptMain; }
	std::string getAcceptLanguage() const { return m_acceptLanguage; }
	std::string getAcceptEncoding() const { return m_acceptEncoding; }
	std::string getAcceptCharset() const { return m_acceptCharset; }
	std::string getContentType() const { return m_contentType; }
	bool getAcceptCompressed() const { return m_acceptCompressed; }
	int getPauseTime() const { return m_pauseTime; }
	bool getStoreHeader() const { return m_storeHeader; }
	bool getStoreBody() const { return m_storeBody; }
	std::string getExpectedPhrase() const { return m_expectedPhrase; }
	bool getDownloadContent() const { return m_downloadContent; }

	long getConnectTimeout() const { return m_connectTimeout; }
	long getTotalTimeout() const { return m_totalTimeout; }

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

	void setConnectTimeout(long timeout) { m_connectTimeout = timeout; }
	void setTotalTimeout(long timeout) { m_totalTimeout = timeout; }

	void addParameter(std::string &name, std::string &value);
	void addCookie(std::string &name, std::string &value);

	bool hasParameters() const { return !m_aParameters.empty(); }
	bool hasCookies() const { return !m_aCookies.empty(); }

	void addDynamicParameter(DynamicParameter *pDynamicParameter);
	void processDynamicParameters(HTTPEngine &engine);

	void clearParameters() { m_aParameters.clear(); }
	void cleanupDynamicParameters();

	void addExtractionItem(ExtractionItem &item);

	inline std::map<std::string, std::string>::iterator params_begin() { return m_aParameters.begin(); }
	inline std::map<std::string, std::string>::iterator params_end() { return m_aParameters.end(); }
	inline std::map<std::string, std::string>::const_iterator params_begin() const { return m_aParameters.begin(); }
	inline std::map<std::string, std::string>::const_iterator params_end() const { return m_aParameters.end(); }

	inline std::vector<HTTPCookie>::iterator cookies_begin() { return m_aCookies.begin(); }
	inline std::vector<HTTPCookie>::iterator cookies_end() { return m_aCookies.end(); }
	inline std::vector<HTTPCookie>::const_iterator cookies_begin() const { return m_aCookies.begin(); }
	inline std::vector<HTTPCookie>::const_iterator cookies_end() const { return m_aCookies.end(); }

	inline std::vector<ExtractionItem>::iterator extractionItems_begin() { return m_aExtractionItems.begin(); }
	inline std::vector<ExtractionItem>::iterator extractionItems_end() { return m_aExtractionItems.end(); }
	inline std::vector<ExtractionItem>::const_iterator extractionItems_begin() const { return m_aExtractionItems.begin(); }
	inline std::vector<ExtractionItem>::const_iterator extractionItems_end() const { return m_aExtractionItems.end(); }

protected:
	RequestType		m_requestType;

	long			m_connectTimeout;
	long			m_totalTimeout;

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

	std::map<std::string, std::string> m_aParameters;
	std::vector<HTTPCookie> m_aCookies;

	std::vector<DynamicParameter *>	m_aDynamicParameters;
	std::vector<ExtractionItem> m_aExtractionItems;
};

#endif
