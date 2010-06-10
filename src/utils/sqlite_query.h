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

#ifndef SQLITE_QUERY_H
#define SQLITE_QUERY_H

#ifdef _MSC_VER
#pragma warning (disable : 4786)
#endif

#include <map>

#include "sqlite_db.h"

class SQLiteQuery
{
public:
	SQLiteQuery(SQLiteDB &db, bool write = false);
	SQLiteQuery(SQLiteDB &db, const std::string &sql, bool write = false);
	~SQLiteQuery();
	
	bool execute(const std::string &sql);
	static int busyCallback(void *pArg, int busy);

	void sleep(long ms);
	
	sqlite3_stmt *getResult(const std::string &sql);
	void freeResults();
	bool fetchNext();
	
	unsigned long getInsertRowID();
	
	long rowCount();
	int colCount();
	
	std::string getLastError();
	
	const char *executeGetString(const std::string &sql);
	long executeGetLong(const std::string &sql);
	double executeGetDouble(const std::string &sql);
	
	const char *getString(const std::string &name);
	const char *getString(int col);
	const char *getString();
	
	long getLong(const std::string &name);
	long getLong(int col);
	long getLong();
	
	unsigned long getULong(const std::string &name);
	unsigned long getULong(int col);
	unsigned long getULong();
	
	double getDouble(const std::string &name);
	double getDouble(int col);
	double getDouble();

private:
	SQLiteQuery(const SQLiteQuery &q) : m_db(m_db) { }
	
	SQLiteDB &		m_db;
	DBConn *		m_pConn;
	sqlite3_stmt *	m_result;
	bool			m_hasRow;
	int				m_thisColumn;
	
	std::string		m_lastQuery;
	std::string		m_tempString;
	
	int				m_cachedRC;
	bool			m_validCachedRC;
	int				m_rowCount;
	int				m_colCount;
	
	std::map<std::string, int> m_aCols;	
};

#endif
