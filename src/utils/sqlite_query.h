
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
	
	bool execute(const std::string &sql, bool retry = false);
	static int busyCallback(void *pArg, int busy);
	
	sqlite3_stmt *getResult(const std::string &sql);
	void freeResults();
	bool fetchNext();
	
	sqlite_int64 getInsertRowID();
	
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
