
#include "scheduled_results_saver.h"

ScheduledResult::ScheduledResult(HTTPResponse response, TestType type, unsigned long testID) : m_response(response), m_testType(type), m_testID(testID)
{
	
}

ScheduledResultSaver::ScheduledResultSaver(SQLiteDB *pDB) : m_pMainDB(pDB)
{
	
}

void ScheduledResultSaver::run()
{
	while (m_isRunning)
	{
		sleep(30); // sleep for 20 secs
		
		storeResults();
	}
}

void ScheduledResultSaver::addResult(HTTPResponse &response, TestType type, unsigned long testID)
{
	m_mutex.lock();
	
	ScheduledResult newResult(response, type, testID);
	
	m_aResults.push_back(newResult);
	
	m_mutex.unlock();	
}

void ScheduledResultSaver::storeResults()
{
	m_mutex.lock();
	
	SQLiteQuery q(*m_pMainDB, true);

	std::vector<ScheduledResult>::iterator it = m_aResults.begin();
	
	char szTemp[1024];
	for (; it != m_aResults.end();)
	{
		ScheduledResult &result = *it;
		
		std::string sql = "insert into scheduled_single_test_results values (";
		
		memset(szTemp, 0, 1024);
		sprintf(szTemp, "%ld, datetime(%ld, 'unixepoch'), %ld, %ld, %f, %f, %f, %f, %ld, %ld, %ld, %ld, %ld)", result.m_testID, result.m_response.timestamp, result.m_response.errorCode,
				result.m_response.responseCode, result.m_response.lookupTime, result.m_response.connectTime, result.m_response.dataStartTime,result.m_response.totalTime,
				result.m_response.redirectCount, result.m_response.contentSize, result.m_response.downloadSize, result.m_response.componentContentSize, result.m_response.componentDownloadSize);
		
		sql.append(szTemp);
		
		if (q.execute(sql, true))
		{
			long runID = q.getInsertRowID();
			
			// save component results if needed
			
			const std::vector<HTTPComponentResponse> &components = result.m_response.getComponents();
			
			std::vector<HTTPComponentResponse>::const_iterator itComponent = components.begin();
			for (; itComponent != components.end(); ++itComponent)
			{
				const HTTPComponentResponse &compResult = *itComponent;
				std::string sqlComponentResults = "insert into scheduled_single_test_component_results values(";
				memset(szTemp, 0, 1024);
				sprintf(szTemp, "%ld, %ld, %ld, %ld, '%s', %f, %f, %f, %f, %ld, %ld)", result.m_testID, runID, compResult.errorCode, compResult.responseCode, compResult.requestedURL.c_str(),
						compResult.lookupTime, compResult.connectTime, compResult.dataStartTime, compResult.totalTime, compResult.contentSize, compResult.downloadSize);
				sqlComponentResults.append(szTemp);
				
				q.execute(sqlComponentResults, true);			
			}			
		
			// we can delete it now
						
			it = m_aResults.erase(it);			
		}
		else
		{
			// todo: need to see if the error wasn't busy and if so probably delete it, but for now just get the next one

			++it;
		}
	}
	
	m_mutex.unlock();
}
