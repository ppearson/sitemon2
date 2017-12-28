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

#ifndef HTTP_SERVER_H
#define HTTP_SERVER_H

#include <string>

#include "../utils/sqlite_db.h"
#include "../scheduler/scheduled_results_saver.h"

#include "http_server_common.h"

class HTTPServer
{
public:
	HTTPServer(const std::string& webContentPath, SQLiteDB* pMonitoringDB, SQLiteDB* pLoadTestingDB = NULL, int port = 8080);

	void setScheduledResultSaver(ScheduledResultsSaver* pSaver)
	{
		m_pResultsSaver = pSaver;
	}
	
	void setAuthenticationType(HTTPServerAuthenticationType authenticationType, const std::string& username, const std::string& password)
	{
		m_authenticationType = authenticationType;
		m_authenticationUser = username;
		m_authenticationPassword = password;
	}
	
	HTTPServerAuthenticationType getAuthenticationType() const
	{
		return m_authenticationType;
	}
	
	bool areAuthCredentialsValid(const std::string& authUsername, const std::string& authPassword) const;
	
	const std::string& getWebContentPath() const
	{
		return m_webContentPath;
	}
	
	bool start();
	
protected:
	HTTPServerAuthenticationType	m_authenticationType;
	std::string						m_authenticationUser;
	std::string						m_authenticationPassword;
	
	int						m_port;
	
	std::string				m_webContentPath;
	
	SQLiteDB *m_pMonitoringDB;
	SQLiteDB *m_pLoadTestingDB;

	ScheduledResultsSaver*	m_pResultsSaver;
};

#endif
