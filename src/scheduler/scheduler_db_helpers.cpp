#include <map>
#include <deque>

#include "scheduler_db_helpers.h"

bool createNeededSchedulerTables(SQLiteDB *pDB)
{
	createScheduledSingleTestsTable(pDB);
	createScheduledSingleTestResultsTable(pDB);
	
	return true;
}

bool createScheduledSingleTestsTable(SQLiteDB *pDB)
{
	std::string sql = "create table if not exists scheduled_single_tests (enabled integer, description string, url string, referer string, expected_phrase string, interval integer, accept_compressed integer)";
	
	if (pDB)
	{
		SQLiteQuery q(*pDB, true);
		
		return q.execute(sql);		
	}	
	
	return false;
}

bool createScheduledSingleTestResultsTable(SQLiteDB *pDB)
{
	std::string sql1 = "create table if not exists scheduled_single_test_results (test_id integer, run_time date, error_code integer, response_code integer, lookup_time double,"
					"connect_time double, data_start_time double, total_time double, redirect_count integer, content_size integer, download_size integer)";
	
	std::string sql2 = "create index if not exists single_test_results on scheduled_single_test_results(test_id)";
	
	if (pDB)
	{
		SQLiteQuery q(*pDB, true);
		
		bool ret1 = q.execute(sql1);
		bool ret2 = q.execute(sql2);
		
		return ret1 && ret2;
	}	
	
	return false;
}

// expects an empty vector
bool getScheduledSingleTestsFromDB(SQLiteDB *pDB, std::vector<ScheduledItem> &items)
{
	if (!pDB)
		return false;
	
	std::string sql = "select rowid, description, url, interval from scheduled_single_tests where enabled = 1";

	SQLiteQuery q(*pDB);

	time_t timeNow;
	time(&timeNow);

	long schedID = 0;
	
	q.getResult(sql);
	while (q.fetchNext())
	{
		long testID = q.getLong();
		std::string description = q.getString();
		std::string url = q.getString();
		long interval = q.getLong();

		if (description.empty())
			description = " ";

		if (url.empty())
			continue;

		ScheduledItem newItem(schedID++, description, interval, timeNow);
		newItem.setTestType(SINGLE_TEST);
		newItem.setTestID(testID);

		items.push_back(newItem);
	}
	
	return true;
}

bool updateScheduledSingleTests(SQLiteDB *pDB, std::vector<ScheduledItem> &items)
{
	if (!pDB)
		return false;

	// basically we want to try to keep tests that aren't being 
	// added or removed and only update the timing interval as this will allow an interval
	// of 5 to be changed to 10 and the second it gets fired off on to still be the same

	std::map<unsigned long, unsigned long> aCurrentPositions; // current positions in the vector
	std::vector<ScheduledItem> aNewTests;

	std::vector<ScheduledItem>::iterator it = items.begin();
	int pos = 0;
	for (; it != items.end(); ++it, pos++)
	{
		ScheduledItem &item = *it;
		aCurrentPositions[item.getTestID()] = pos;
	}

	std::string sql = "select rowid, enabled, description, url, interval from scheduled_single_tests";

	SQLiteQuery q(*pDB);

	time_t timeNow;
	time(&timeNow);
	
	q.getResult(sql);
	while (q.fetchNext())
	{
		long testID = q.getLong();
		long enabled = q.getLong();
		std::string description = q.getString();
		std::string url = q.getString();
		long interval = q.getLong();

		if (description.empty())
			description = " ";

		if (url.empty())
			continue;

		std::map<unsigned long, unsigned long>::iterator itFind = aCurrentPositions.find(testID);

		if (itFind != aCurrentPositions.end())
		{
			unsigned long currentPos = (*itFind).second;

			ScheduledItem &currentItem = items.at(currentPos);

			if (enabled == 0) // this is now disabled, so lets remove it
			{
				continue; // don't remove it from map so we can remove it from vector
			}

			currentItem.setInterval(interval);
			currentItem.setDescription(description);

			aCurrentPositions.erase(itFind);	
		}
		else if (enabled == 1) // new one, so create it if needed
		{
			ScheduledItem newItem(pos++, description, interval, timeNow);
			newItem.setTestType(SINGLE_TEST);
			newItem.setTestID(testID);

			aNewTests.push_back(newItem);
		}
	}

	// now the remainder in the map can be deleted from the vector in reverse order (from the bottom)
	if (!aCurrentPositions.empty())
	{
		std::deque<unsigned long> aTempPos;
		std::map<unsigned long, unsigned long>::iterator itDel = aCurrentPositions.begin();
		for (; itDel != aCurrentPositions.end(); ++itDel)
		{
			aTempPos.push_front((*itDel).second);
		}

		std::deque<unsigned long>::iterator itDel2 = aTempPos.begin();
		for (; itDel2 != aTempPos.end(); ++itDel2)
		{
			unsigned long pos2 = *itDel2;

			items.erase(items.begin() + pos2);
		}
	}

	// add any new items on the end
	if (!aNewTests.empty())
	{
		std::copy(aNewTests.begin(), aNewTests.end(), std::inserter(items, items.end()));
	}

	return true;
}