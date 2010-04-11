
#ifndef HTTP_SERVER_REQUEST_THREAD_H
#define HTTP_SERVER_REQUEST_THREAD_H

#include "../utils/Socket.h"
#include "../utils/thread.h"
#include "../utils/sqlite_query.h"

class HTTPServerRequestThread: public Thread
{
public:
	HTTPServerRequestThread(Socket *socket, const std::string &webContentPath = "", SQLiteDB *pMainDB = NULL);
	virtual ~HTTPServerRequestThread();
	
	virtual void run();
	
protected:
	Socket *	m_pSocket;
	std::string	m_webContentPath;
	SQLiteDB *	m_pMainDB;	
};
	
#endif
