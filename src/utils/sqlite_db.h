/*
 *  sqlite_db.h
 *  sqlite
 *
 *  Created by Peter Pearson on 04/04/2010.
 *  Copyright 2010 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef SQLITE_DB_H
#define SQLITE_DB_H

#include <string>
#include <list>

#include "sqlite3.h"

#include "mutex.h"

struct DBConn
{
	DBConn() : m_inUse(false) { }
	
	sqlite3 *	m_pDB;
	bool		m_inUse;
};

class SQLiteDB
{
public:
	SQLiteDB(const std::string &path, bool useMutex = false);
	~SQLiteDB();
	
	DBConn *getDBConnection(bool write = false);
	void releaseDBConnection(DBConn *pConn, bool cache = false);
	
	bool isThreadSafe() { return sqlite3_threadsafe() != 0; }
	
protected:
	SQLiteDB(const SQLiteDB &db) : m_mutex(m_mutex) { }
	
	std::string		m_path;
	bool			m_useMutex;
	Mutex			m_mutex;
	
	std::list<DBConn *> m_aAvailableConnections;
	
};


#endif
