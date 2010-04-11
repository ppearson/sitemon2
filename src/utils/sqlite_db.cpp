/*
 *  sqlite_db.cpp
 *  sqlite
 *
 *  Created by Peter Pearson on 04/04/2010.
 *  Copyright 2010 __MyCompanyName__. All rights reserved.
 *
 */

#include "sqlite_db.h"

SQLiteDB::SQLiteDB(const std::string &path) : m_path(path), m_useMutex(false), m_mutex(m_mutex)
{
	
}

SQLiteDB::SQLiteDB(Mutex &mutex, const std::string &path) : m_mutex(mutex), m_useMutex(true), m_path(path)
{
	
}

SQLiteDB::~SQLiteDB()
{
	std::list<DBConn *>::iterator it = m_aAvailableConnections.begin();
	for (; it != m_aAvailableConnections.end(); ++it)
	{
		DBConn *conn = *it;
		
		sqlite3_close(conn->m_pDB);
		
		delete conn;
		
		// need to check return code from close was SQLITE_OK and empty the list
	}
}

DBConn *SQLiteDB::getDBConnection(bool write)
{
	if (m_useMutex)
	{
		m_mutex.lock();
	}
	
	DBConn * pConn = NULL;
	
/*	std::list<DBConn *>::iterator it = m_aAvailableConnections.begin();
	for (; it != m_aAvailableConnections.end(); ++it)
	{
		if (!(*it)->m_inUse)
		{
			pConn = *it;
			break;
		}
	}
*/
	
	// need to do write flag in above

	if (pConn)
	{
		pConn->m_inUse = true;
	}
	else
	{
		pConn = new DBConn;
		
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
		pConn->m_inUse = true;
		
//		m_aAvailableConnections.push_back(pConn);
	}
	
	if (m_useMutex)
	{
		m_mutex.unlock();
	}
	
	return pConn;
}

void SQLiteDB::releaseDBConnection(DBConn *pConn, bool cache)
{
	if (m_useMutex)
	{
		m_mutex.lock();
	}

	if (!cache)
	{
		sqlite3_close(pConn->m_pDB);
		delete pConn;

		// need to remove the item from the list
	}
	else
	{
		if (pConn)
		{
			pConn->m_inUse = false;
		}
	}

	if (m_useMutex)
	{
		m_mutex.unlock();
	}
}