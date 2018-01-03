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

#ifndef HTTP_SERVER_REQUEST_H
#define HTTP_SERVER_REQUEST_H

#include <string>
#include <map>
#include <vector>

class HTTPServerRequest
{
public:
	HTTPServerRequest(const std::string &request);
	
	enum AuthenticationType
	{
		eAuthNone,
		eAuthMalformed,
		eAuthUnknown,
		eAuthBasic,
		eAuthDigest
	};
	
	bool parse();
	
	const std::string &getPath() const { return m_path; }
	
	bool isPost() const
	{
		return  m_post;
	}
	
	bool hasAuthenticationHeader() const
	{
		return m_authenticationHeaderType != eAuthNone;
	}
	
	bool isAcceptedAuthenticationHeader() const
	{
		return m_authenticationHeaderType == eAuthBasic;
	}
	
	const std::string& getAuthUsername() const
	{
		return m_authUsername;
	}
	
	const std::string& getAuthPassword() const
	{
		return m_authPassword;
	}
	
	bool hasParams() const { return !m_aParams.empty(); }
	bool hasParam(const std::string& name);
	std::string getParam(const std::string& name) { return m_aParams[name]; }
	unsigned long getParamAsLong(const std::string& name);
	
	bool hasCookies() const { return !m_aCookies.empty(); }
	bool hasCookie(const std::string& name);
	std::string getCookie(const std::string& name) { return m_aCookies[name]; }
	int getParamAsInt(const std::string& name);
	
protected:
	void addParams(const std::string &params);
	
protected:
	std::string			m_request;
	
	std::string			m_path;
	bool				m_post;
	
	AuthenticationType	m_authenticationHeaderType; // note: this doesn't mean it's valid...
	std::string			m_authUsername;
	std::string			m_authPassword;
	
	std::map<std::string, std::string>	m_aParams;
	std::map<std::string, std::string>	m_aCookies;
};

#endif
