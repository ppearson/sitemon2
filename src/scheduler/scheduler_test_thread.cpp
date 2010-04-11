#include "scheduler_test_thread.h"
#include "../utils/sqlite_query.h"
#include "scheduler_db_helpers.h"

SchedulerTestThread::SchedulerTestThread(ScheduledResultSaver *pSaver, SQLiteDB *pDB, TestType testType, unsigned long testID)
				: m_pSaver(pSaver), m_pMainDB(pDB), m_testType(testType), m_testID(testID)
{
	m_autoDestruct = true;
}

void SchedulerTestThread::run()
{
	if (!m_pMainDB)
		return;

	if (m_testType == SCRIPT_TEST)
		return;
	
	std::string sql = "select enabled, description, url, referer, expected_phrase, accept_compressed from scheduled_single_tests where rowid = ";

	char szRowID[16];
	memset(szRowID, 0, 16);
	sprintf(szRowID, "%ld", m_testID);

	sql.append(szRowID);

	SQLiteQuery q(*m_pMainDB, true);

	q.getResult(sql);
	if (q.fetchNext())
	{
		long enabled = q.getLong();
		std::string description = q.getString();
		std::string url = q.getString();
		std::string referrer = q.getString();
		std::string expectedPhrase = q.getString();
		long acceptCompressed = q.getLong();

		if (enabled == 0)
		{
			return;
		}

		HTTPEngine engine;

		HTTPRequest request(url);
		if (acceptCompressed == 1)
			request.setAcceptCompressed(true);

		request.setExpectedPhrase(expectedPhrase);
		request.setReferrer(referrer);

		HTTPResponse response;

		engine.performRequest(request, response);
		
		if (m_pSaver)
		{
			m_pSaver->addResult(response, m_testType, m_testID);
		}		
	}
}


void SchedulerTestThread::runTest()
{


}