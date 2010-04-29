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

#include <map>
#include <deque>

#include "scheduler_db_helpers.h"

bool createNeededSchedulerTables(SQLiteDB *pDB)
{
	createScheduledSingleTestsTable(pDB);
	createScheduledSingleTestResultsTable(pDB);

	createScheduledScriptTestsTables(pDB);
	
	return true;
}

bool createScheduledSingleTestsTable(SQLiteDB *pDB)
{
	std::string sql = "create table if not exists scheduled_single_tests (enabled integer, description string, url string, expected_phrase string, interval integer, accept_compressed integer, download_components integer)";
	
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
					"connect_time double, data_start_time double, total_time double, redirect_count integer, content_size integer, download_size integer, component_content_size integer, component_download_size integer)";
	
	std::string sql2 = "create index if not exists single_test_results on scheduled_single_test_results(test_id)";
	
	std::string sqlComponentResults = "create table if not exists scheduled_single_test_component_results (test_id integer, run_id integer, error_code integer, response_code integer,"
										"url string, lookup_time double, connect_time double, data_start_time double, total_time double, content_size integer, download_size integer)";
	
	std::string sqlComponentResultsIndex = "create index if not exists single_test_component_results on scheduled_single_test_component_results(test_id, run_id)";
	
	if (pDB)
	{
		SQLiteQuery q(*pDB, true);
		
		bool ret1 = q.execute(sql1);
		bool ret2 = q.execute(sql2);
		bool ret3 = q.execute(sqlComponentResults);
		bool ret4 = q.execute(sqlComponentResultsIndex);
		
		return ret1 && ret2 && ret3 && ret4;
	}	
	
	return false;
}

bool createScheduledScriptTestsTables(SQLiteDB *pDB)
{
	std::string sql1 = "create table if not exists scheduled_script_tests (enabled integer, description string, interval integer, accept_compressed integer, download_components integer)";
	
	std::string sql2 = "create table if not exists scheduled_script_test_pages (script_id integer, page_num integer, description string, url string, request_type integer, expected_phrase string)";
	std::string sql3 = "create index if not exists script_test_pages on scheduled_script_test_pages(script_id)";
	
	std::string sql4 = "create table if not exists scheduled_script_test_page_params (page_id integer, name string, value string)";
	std::string sql5 = "create index if not exists script_test_page_params on scheduled_script_test_page_params(page_id)";
	
	if (pDB)
	{
		SQLiteQuery q(*pDB, true);
		
		bool ret1 = q.execute(sql1);
		bool ret2 = q.execute(sql2);
		bool ret3 = q.execute(sql3);
		bool ret4 = q.execute(sql4);
		bool ret5 = q.execute(sql5);
		
		return ret1 && ret2 && ret3 && ret4 && ret5;
	}	
	
	return false;
}

bool createScheduledSingleTestResultsTables(SQLiteDB *pDB)
{
	return true;
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

	time_t timeNow;
	time(&timeNow);
	
	{
		SQLiteQuery q(*pDB);
		
		// sometimes if db is locked, even a read can't be done as sqlite can't read the db schema
		if (!q.getResult(sql))
			return false;

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