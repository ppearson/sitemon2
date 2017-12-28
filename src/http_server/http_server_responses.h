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

#ifndef HTTP_SERVER_RESPONSES_H
#define HTTP_SERVER_RESPONSES_H

#include <string>
#include <vector>
#include <iostream>
#include <sstream>

// TODO: use inheritance here?

class HTTPServerResponse
{
public:
	HTTPServerResponse(int returnCode, const std::string &text = "");
	
	std::string responseString();
	
protected:
	int m_returnCode;
	std::string m_text;
};

class HTTPServerRedirectResponse
{
public:
	HTTPServerRedirectResponse(const std::string &url = "");
	
	std::string responseString();
	
protected:
	std::string m_url;
};

class HTTPServerAuthenticationResponse
{
public:
	HTTPServerAuthenticationResponse();
	
	std::string responseString();
	
protected:

};

class HTTPServerFileResponse
{
public:
	HTTPServerFileResponse(const std::string &path, bool binary = false);
	
	std::string responseString();
	
protected:
	std::string m_path;
	bool		m_binary;
};

class HTTPServerTemplateFileResponse
{
public:
	HTTPServerTemplateFileResponse(const std::string &path, std::string &content);
	HTTPServerTemplateFileResponse(const std::string &path, std::string &content1, std::string &content2);
	HTTPServerTemplateFileResponse(const std::string &path, std::string &content1, std::string &content2, std::string &content3);
	
	std::string responseString();
	
protected:
	int			m_templateArgs;
	std::string m_path;
	std::vector<std::string *> m_aContent;
};

#endif
