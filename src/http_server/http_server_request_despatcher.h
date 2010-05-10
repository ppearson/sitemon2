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

#ifndef HTTP_SERVER_REQUEST_DESPATCHER_H
#define HTTP_SERVER_REQUEST_DESPATCHER_H

#include <string>
#include <map>

#include "http_server_request.h"
#include "http_server_responses.h"

#include "../utils/sqlite_query.h"

class HTTPServerRequestDespatcher
{
public:
	HTTPServerRequestDespatcher(const std::string &webContentPath = "", SQLiteDB *pMainDB = NULL);
	
	void registerMappings();
	
	void handleRequest(HTTPServerRequest &request, std::string &response);
	
	typedef void (HTTPServerRequestDespatcher::*MFP)(HTTPServerRequest &, std::string &);
	
	
	//////// individual  handlers....
	
	void inlineSimple(HTTPServerRequest &request, std::string &response);
	void history(HTTPServerRequest &request, std::string &response);
	void monitoring(HTTPServerRequest &request, std::string &response);
	void addSingleTest(HTTPServerRequest &request, std::string &response);
	void addScriptTest(HTTPServerRequest &request, std::string &response);
	void editMonitorTest(HTTPServerRequest &request, std::string &response);
	void editScriptTest(HTTPServerRequest &request, std::string &response);
	void addScriptPage(HTTPServerRequest &request, std::string &response);
	void editScriptPage(HTTPServerRequest &request, std::string &response);
	void viewSingleTest(HTTPServerRequest &request, std::string &response);
	void singleDetails(HTTPServerRequest &request, std::string &response);
	void singleComponents(HTTPServerRequest &request, std::string &response);
	void viewScriptTest(HTTPServerRequest &request, std::string &response);
	void scriptDetails(HTTPServerRequest &request, std::string &response);
	
	void deleteSingleTest(HTTPServerRequest &request, std::string &response);
	void deleteScriptTest(HTTPServerRequest &request, std::string &response);
	void deleteScriptStep(HTTPServerRequest &request, std::string &response);
	
protected:
	std::map<std::string, MFP>	m_requestMappings;	
	
	std::string	m_webContentPath;
	SQLiteDB *	m_pMainDB;
};

#endif
