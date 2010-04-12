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

