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

#include "../utils/socket.h"
#include "http_server.h"
#include "http_server_request_thread.h"

#include "../utils/sqlite_db.h"

#include "http_server_db_helpers.h"

HTTPServer::HTTPServer(const std::string &webContentPath, SQLiteDB *pDB, int port) : m_port(port), m_webContentPath(webContentPath), m_pMainDB(pDB)
{
	
}

bool HTTPServer::start()
{
	Socket::initWinsocks();
	
	Socket mainSocket;
	mainSocket.create();
	
	if (!mainSocket.bind(m_port))
	{
		printf("Couldn't bind to port: %i\n", m_port);
		return false;
	}
	
	if (!mainSocket.listen(20))
	{
		printf("Couldn't listen for connections.\n");
		return false;
	}
	
	while (true)
	{
		Socket *newSock = new Socket();
		if (mainSocket.accept(newSock))
		{
			HTTPServerRequestThread *newThread = new HTTPServerRequestThread(newSock, m_webContentPath, m_pMainDB);
			
			if (newThread)
			{
				newThread->setAutodestruct(true);
				newThread->start();
			}
			else
			{
				printf("Couldn't create new thread\n");
			}
		}
		else
		{
			printf("Couldn't accept connection\n");
		}
	}
		
	Socket::cleanupWinsocks();
	return true;
}

