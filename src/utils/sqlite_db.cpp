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