/*
 *  sqlite_db.cpp
 *  sqlite
 *
 *  Created by Peter Pearson on 04/04/2010.
 *  Copyright 2010 __MyCompanyName__. All rights reserved.
 *
 */

#include "sqlite_db.h"

SQLiteDB::SQLiteDB(const std::string &path, bool useMutex) : m_path(path), m_useMutex(useMutex)
{
	
}

SQLiteDB::~SQLiteDB()
{
	
}

DBConn *SQLiteDB::getDBConnection(bool write)
{
	if (m_useMutex)
	{
		m_mutex.lock();
	}
	
	DBConn * pConn = new DBConn;
		
	if (!pConn)
	{
		printf("Couldn't alloc memory for struct...\n");
		if (m_useMutex)
		{
			m_mutex.unlock();
		}
		return NULL;
	}

	int flags = SQLITE_OPEN_READONLY | SQLITE_OPEN_CREATE;

	if (write)
	{
		flags = SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE;
	}
	
	int rc = sqlite3_open_v2(m_path.c_str(), &pConn->m_pDB, flags, NULL);
	if (rc)
	{
		printf("Can't open specified database file: %s\n", sqlite3_errmsg(pConn->m_pDB));
		sqlite3_close(pConn->m_pDB);
		delete pConn;
		if (m_useMutex)
		{
			m_mutex.unlock();
		}
		return NULL;
	}
	
/*	if (m_useMutex)
	{
		m_mutex.unlock();
	}
*/	
	return pConn;
}

void SQLiteDB::releaseDBConnection(DBConn *pConn)
{
/*	if (m_useMutex)
	{
		m_mutex.lock();
	}
*/
	sqlite3_close(pConn->m_pDB);
	delete pConn;

	if (m_useMutex)
	{
		m_mutex.unlock();
	}
}