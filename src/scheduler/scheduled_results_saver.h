#ifndef SCHEDULED_RESULTS_SAVER_H
#define SCHEDULED_RESULTS_SAVER_H

#include <string>
#include <vector>

#include "../http_engine.h"

#include "../utils/thread.h"

#include "../utils/sqlite_query.h"

enum TestType
{
	SINGLE_TEST,
	SCRIPT_TEST	
};

class ScheduledResult
{
public:
	ScheduledResult(HTTPResponse response, TestType type, unsigned long testID);
	
	TestType	m_testType;
	unsigned long	m_testID;
	
	HTTPResponse	m_response;	
};


class ScheduledResultSaver : public Thread
{
public:
	ScheduledResultSaver(SQLiteDB *pDB);
	virtual ~ScheduledResultSaver() { }
	
	virtual void run();
	
	void addResult(HTTPResponse &response, TestType type, unsigned long testID);
	
	void storeResults();
	
protected:
	
	std::vector<ScheduledResult>	m_aResults;
	Mutex							m_mutex;
	
	SQLiteDB *						m_pMainDB;	
	
};



#endif
