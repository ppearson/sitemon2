/*
 *  sqlite_query.cpp
 *  sqlite
 *
 *  Created by Peter Pearson on 04/04/2010.
 *  Copyright 2010 __MyCompanyName__. All rights reserved.
 *
 */

#include "sqlite_query.h"

SQLiteQuery::SQLiteQuery(SQLiteDB &db, bool write) : m_db(db), m_pConn(db.getDBConnection(write)), m_result(NULL), m_hasRow(false),
									m_thisColumn(0), m_cachedRC(0), m_validCachedRC(false), m_rowCount(0), m_colCount(0)
{
	
}

SQLiteQuery::SQLiteQuery(SQLiteDB &db, const std::string &sql, bool write) : m_db(db), m_pConn(db.getDBConnection(write)), m_result(NULL), m_hasRow(false),
									m_thisColumn(0), m_cachedRC(0), m_validCachedRC(false), m_rowCount(0), m_colCount(0)
{
	execute(sql);
}

SQLiteQuery::~SQLiteQuery()
{
	sqlite3_finalize(m_result);
	
	if (m_pConn)
	{
		m_db.releaseDBConnection(m_pConn);
	}
}

bool SQLiteQuery::execute(const std::string &sql, bool retry)
{
	m_lastQuery = sql;
	
	if (m_pConn && m_result)
	{
		sqlite3_finalize(m_result);
		m_result = NULL;
	}

	const char *s = NULL;
	int rc = sqlite3_prepare(m_pConn->m_pDB, sql.c_str(), sql.size(), &m_result, &s);
	if (rc != SQLITE_OK)
	{
		printf("SQLiteQuery::execute prepare query failed.\n");
		return false;
	}
	
	if (!m_result)
	{
		printf("SQLiteQuery::execute query failed.\n");
		return false;
	}
	
	rc = sqlite3_busy_handler(m_pConn->m_pDB, busyCallback, 0);
	
	rc = sqlite3_step(m_result);
	sqlite3_finalize(m_result);
	m_result = NULL;
	
	switch (rc)
	{
		case SQLITE_BUSY:
			printf("SQLiteQuery::execute db busy.\n");
			return false;
		case SQLITE_DONE:
		case SQLITE_ROW:
			return true;
		case SQLITE_ERROR:
			printf("SQLiteQuery::execute error: %s.\n", sqlite3_errmsg(m_pConn->m_pDB));
			return false;
		case SQLITE_MISUSE:
			printf("SQLiteQuery::execute db misuse.\n");
			return false;
	}	
			
	return false;
}

int SQLiteQuery::busyCallback(void *pArg, int busy)
{
	sqlite3_sleep(50);
	printf("SQLiteQuery::busyCallback...\n");
	return 1;
}

sqlite3_stmt *SQLiteQuery::getResult(const std::string &sql)
{
	m_lastQuery = sql;
	
	if (m_pConn && m_result)
	{
		// already got an active result
	}
	else
	{
		const char *s = NULL;
		int rc = sqlite3_prepare(m_pConn->m_pDB, sql.c_str(), sql.size(), &m_result, &s);
		if (rc != SQLITE_OK)
		{
			printf("SQLiteQuery::execute prepare query failed.\n");
			return NULL;
		}
		
		if (!m_result)
		{
			printf("SQLiteQuery::execute query failed.\n");
			return NULL;
		}
		
		int i = 0;
		while (true)
		{
			const char *name = sqlite3_column_name(m_result, i);
			if (!name)
				break;
			m_aCols[name] = i++;
		}
		m_colCount = i;
		
		m_cachedRC = sqlite3_step(m_result);
		m_validCachedRC = true;
		m_rowCount = (m_cachedRC == SQLITE_ROW) ? 1 : 0;
	}
	
	return m_result;
}

void SQLiteQuery::freeResults()
{
	if (m_pConn && m_result)
	{
		sqlite3_finalize(m_result);
		m_result = NULL;
		m_hasRow = false;
		m_validCachedRC = false;
	}
	
	m_aCols.clear();
}

bool SQLiteQuery::fetchNext()
{
	m_thisColumn = 0;
	m_hasRow = false;
	
	if (m_pConn && m_result)
	{
		int rc = m_validCachedRC ? m_cachedRC : sqlite3_step(m_result);
		m_validCachedRC = false;
		
		switch (rc)
		{
			case SQLITE_BUSY:
				printf("SQLiteQuery::execute db busy.\n");
				return false;
			case SQLITE_DONE:
				return false;
			case SQLITE_ROW:
				m_hasRow = true;
				return true;
			case SQLITE_ERROR:
				printf("SQLiteQuery::execute error: %s.\n", sqlite3_errmsg(m_pConn->m_pDB));
				return false;
			case SQLITE_MISUSE:
				printf("SQLiteQuery::execute db misuse.\n");
				return false;
		}		
	}
	
	return false;
}

sqlite_int64 SQLiteQuery::getInsertRowID()
{
	if (m_pConn)
	{
		return sqlite3_last_insert_rowid(m_pConn->m_pDB);
	}
	else
	{
		return 0;
	}
}

long SQLiteQuery::rowCount()
{
	return m_pConn && m_result ? m_rowCount : 0;
}

int SQLiteQuery::colCount()
{
	return m_colCount;
}

std::string SQLiteQuery::getLastError()
{
	if (m_pConn)
	{
		return sqlite3_errmsg(m_pConn->m_pDB);
	}
	return "";
}

const char *SQLiteQuery::executeGetString(const std::string &sql)
{
	m_tempString = "";
	
	if (getResult(sql))
	{
		if (fetchNext())
		{
			m_tempString = getString();
		}
		freeResults();
	}
	
	return m_tempString.c_str();
}

long SQLiteQuery::executeGetLong(const std::string &sql)
{
	long val = 0;
	if (getResult(sql))
	{
		if (fetchNext())
		{
			val = getLong();
		}
		freeResults();
	}
	
	return val;
}

double SQLiteQuery::executeGetDouble(const std::string &sql)
{
	double val = 0;
	if (getResult(sql))
	{
		if (fetchNext())
		{
			val = getDouble();
		}
		freeResults();
	}
	
	return val;
}

const char *SQLiteQuery::getString(const std::string &name)
{
	int col = m_aCols[name];
	if (col >= 0)
		return getString(col);
	
	return "";
}

const char *SQLiteQuery::getString(int col)
{
	if (m_pConn && m_result && m_hasRow && col < sqlite3_column_count(m_result))
	{
		const unsigned char *text = sqlite3_column_text(m_result, col);
		return text ? (const char *)text : "";
	}
	return "";
}

const char *SQLiteQuery::getString()
{
	return getString(m_thisColumn++);
}

long SQLiteQuery::getLong(const std::string &name)
{
	int col = m_aCols[name];
	if (col >= 0)
		return getLong(col);

	return 0;
}

long SQLiteQuery::getLong(int col)
{
	if (m_pConn && m_result && m_hasRow && col < sqlite3_column_count(m_result))
	{
		return sqlite3_column_int(m_result, col);
	}
	
	return 0;
}

long SQLiteQuery::getLong()
{
	return getLong(m_thisColumn++);
}

unsigned long SQLiteQuery::getULong(const std::string &name)
{
	int col = m_aCols[name];
	if (col >= 0)
		return getULong(col);
	
	return 0;
}

unsigned long SQLiteQuery::getULong(int col)
{
	if (m_pConn && m_result && m_hasRow && col < sqlite3_column_count(m_result))
	{
		return sqlite3_column_int(m_result, col);
	}
	
	return 0;
}

unsigned long SQLiteQuery::getULong()
{
	return getULong(m_thisColumn++);
}

double SQLiteQuery::getDouble(const std::string &name)
{
	int col = m_aCols[name];
	if (col >= 0)
		return getDouble(col);
	
	return 0.0;
}

double SQLiteQuery::getDouble(int col)
{
	if (m_pConn && m_result && m_hasRow && col < sqlite3_column_count(m_result))
	{
		return sqlite3_column_double(m_result, col);
	}
	
	return 0.0;
}

double SQLiteQuery::getDouble()
{
	return getDouble(m_thisColumn++);
}

