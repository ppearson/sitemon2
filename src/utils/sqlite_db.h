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

#ifndef SQLITE_DB_H
#define SQLITE_DB_H

#include <string>

#include "sqlite3.h"
#include "mutex.h"

struct DBConn
{
	DBConn() : m_pDB(NULL)
	{

	}

	sqlite3 *	m_pDB;
};

class SQLiteDB
{
public:
	SQLiteDB(const std::string &path, bool useMutex = false);
	~SQLiteDB();

	DBConn *getDBConnection(bool write = false);
	void releaseDBConnection(DBConn *pConn);

	bool isThreadSafe() { return sqlite3_threadsafe() != 0; }

protected:
	SQLiteDB(const SQLiteDB &db);

	std::string		m_path;
	bool			m_useMutex;
	Mutex			m_mutex;
};


#endif
