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
	
	std::string sql = "select enabled, description, url, expected_phrase, accept_compressed, download_components from scheduled_single_tests where rowid = ";

	char szRowID[16];
	memset(szRowID, 0, 16);
	sprintf(szRowID, "%ld", m_testID);

	sql.append(szRowID);
	
	long enabled = 0;
	std::string description;
	std::string url;
	std::string expectedPhrase;
	long acceptCompressed = 0;
	long downloadComponents = 0;

	// scope the SQLiteQuery object so it goes out of scope (and therefore releases the DB connection) ASAP
	// otherwise, it hangs around blocking db access until all the test threads have finished downloading and
	// parsing HTML...
	{
		SQLiteQuery q(*m_pMainDB);

		if (!q.getResult(sql))
		{
			printf("Problem getting params needed from db for scheduled test...\n");
			return;
		}

		if (!q.fetchNext())
		{
			return;
		}
		
		enabled = q.getLong();
		description = q.getString();
		url = q.getString();
		expectedPhrase = q.getString();
		acceptCompressed = q.getLong();
		downloadComponents = q.getLong();
	}

	if (enabled == 0)
	{
		return;
	}

	HTTPEngine engine;

	HTTPRequest request(url);
	if (acceptCompressed == 1)
		request.setAcceptCompressed(true);
	if (downloadComponents == 1)
		request.setDownloadContent(true);

	request.setExpectedPhrase(expectedPhrase);

	HTTPResponse response;

	engine.performRequest(request, response);
	
	if (m_pSaver)
	{
		m_pSaver->addResult(response, m_testType, m_testID);
	}
}
