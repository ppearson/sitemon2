/*
 *  http_server.h
 *  sitemon
 *
 *  Created by Peter Pearson on 02/04/2010.
 *  Copyright 2010 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef HTTP_SERVER_H
#define HTTP_SERVER_H

#include <string>
#include "../utils/sqlite_db.h"

class HTTPServer
{
public:
	HTTPServer(const std::string &webContentPath, SQLiteDB *pDB, int port = 8080);
	
	bool start();
	
protected:
	int m_port;
	std::string m_webContentPath;
	SQLiteDB *m_pMainDB;

};

#endif
