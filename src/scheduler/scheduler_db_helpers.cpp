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
	createScheduledScriptTestResultsTables(pDB);
	
	return true;
}

bool createScheduledSingleTestsTable(SQLiteDB *pDB)
{
	std::string sql = "create table if not exists scheduled_single_tests (enabled integer, description string, url string, expected_phrase string, interval integer, accept_compressed integer,"
					"download_components integer, modified_timestamp date)";
	
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
	std::string sql1 = "create table if not exists scheduled_script_tests (enabled integer, description string, interval integer, accept_compressed integer, download_components integer, modified_timestamp date)";
	
	std::string sql2 = "create table if not exists scheduled_script_test_pages (script_id integer, page_num integer, description string, url string, request_type integer, expected_phrase string,"
						" pause_time integer)";
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

bool createScheduledScriptTestResultsTables(SQLiteDB *pDB)
{
	std::string sql1 = "create table if not exists scheduled_script_test_results (test_id integer, run_time date, overall_error_code integer, last_response_code integer, steps integer)";
	std::string sql2 = "create index if not exists script_test_results on scheduled_script_test_results(test_id)";
	
	std::string sql3 = "create table if not exists scheduled_script_test_page_results (test_id integer, run_id integer, page_num integer, run_time date, requested_url string, error_code integer,"
						" response_code integer, lookup_time double, connect_time double, data_start_time double, total_time double, redirect_count integer, content_size integer, download_size integer,"
						" component_content_size integer, component_download_size integer)";
	
	std::string sql4 = "create index if not exists script_test_page_results on scheduled_script_test_page_results(test_id, run_id)";
	
	if (pDB)
	{
		SQLiteQuery q(*pDB, true);
		
		bool ret1 = q.execute(sql1);
		bool ret2 = q.execute(sql2);
		bool ret3 = q.execute(sql3);
		bool ret4 = q.execute(sql4);
		
		return ret1 && ret2 && ret3 && ret4;
	}
	
	return false;
}

// expects an empty vector
bool getScheduledSingleTestsFromDB(SQLiteDB *pDB, std::vector<ScheduledItem> &items)
{
	if (!pDB)
		return false;
	
	std::string sql = "select rowid, description, url, interval, expected_phrase, accept_compressed, download_components, strftime('%s', modified_timestamp) as m_timestamp"
						" from scheduled_single_tests where enabled = 1";

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
		std::string expectedPhrase = q.getString();
		long acceptCompressed = q.getLong();
		long downloadComponents = q.getLong();
		unsigned long modifiedTimestamp = q.getLong();

		if (description.empty())
			description = " ";

		if (url.empty())
			continue;

		ScheduledItem newItem(true, schedID++, description, interval, timeNow);
		newItem.setTestID(testID);
		newItem.setModifiedTimestamp(modifiedTimestamp);

		HTTPRequest newRequest(url);
		newRequest.setExpectedPhrase(expectedPhrase);
		newRequest.setAcceptCompressed(acceptCompressed == 1);
		newRequest.setDownloadContent(downloadComponents == 1);

		newItem.setRequest(newRequest);

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

	std::string sql = "select rowid, enabled, description, url, expected_phrase, accept_compressed, download_components, interval, strftime('%s', modified_timestamp) as m_timestamp from scheduled_single_tests";

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
			std::string expectedPhrase = q.getString();
			long acceptCompressed = q.getLong();
			long downloadComponents = q.getLong();
			long interval = q.getLong();
			unsigned long modifiedTimestamp = q.getLong();

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
				
				// see if it's been modified
				if (modifiedTimestamp > currentItem.getModifiedTimestamp())
				{
					currentItem.setInterval(interval); // TODO: if interval's less than original, nextTime is still going to be previous
														// interval's time away, so won't get updated immediately
					currentItem.setDescription(description);
					currentItem.setModifiedTimestamp(modifiedTimestamp);
					
					HTTPRequest &request = currentItem.getRequest();
					request.setUrl(url);
					request.setExpectedPhrase(expectedPhrase);
					request.setAcceptCompressed(acceptCompressed == 1);
					request.setDownloadContent(downloadComponents == 1);
				}				

				aCurrentPositions.erase(itFind);	
			}
			else if (enabled == 1) // new one, so create it if needed
			{
				ScheduledItem newItem(true, pos++, description, interval, timeNow);
				newItem.setTestID(testID);
				
				HTTPRequest newRequest(url);
				newRequest.setExpectedPhrase(expectedPhrase);
				newRequest.setAcceptCompressed(acceptCompressed == 1);
				newRequest.setDownloadContent(downloadComponents == 1);
				newItem.setRequest(newRequest);

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

bool getScheduledScriptTestsFromDB(SQLiteDB *pDB, std::vector<ScheduledItem> &items)
{
	if (!pDB)
		return false;
	
	// TODO: we should be able to do this as a single join statement with a group-by, at least for the script and its pages
	
	std::string sql = "select rowid, description, interval, accept_compressed, download_components, strftime('%s', modified_timestamp) as m_timestamp"
	" from scheduled_script_tests where enabled = 1";
	
	SQLiteQuery q(*pDB);
	
	time_t timeNow;
	time(&timeNow);
	
	long schedID = 0;
	
	q.getResult(sql);
	while (q.fetchNext())
	{
		long testID = q.getLong();
		std::string description = q.getString();
		long interval = q.getLong();
		long acceptCompressed = q.getLong();
		long downloadComponents = q.getLong();
		unsigned long modifiedTimestamp = q.getULong();
		
		if (description.empty())
			description = " ";
		
		ScheduledItem newItem(false, schedID++, description, interval, timeNow);
		newItem.setTestID(testID);
		newItem.setModifiedTimestamp(modifiedTimestamp);
		
		SQLiteQuery qPages(*pDB);
		
		char szScriptID[12];
		memset(szScriptID, 0, 12);
		sprintf(szScriptID, "%ld", testID);
		
		std::string pageSQL = "select rowid, description, url, request_type, expected_phrase, pause_time from scheduled_script_test_pages"
								" where script_id = ";
		pageSQL += szScriptID;
		pageSQL += " order by page_num asc";
		
		if (qPages.getResult(pageSQL))
		{
			Script & newScript = newItem.getScript();
			
			while (qPages.fetchNext())
			{
				long pageID = qPages.getLong();
				std::string desc = qPages.getString();
				std::string url = qPages.getString();
				long requestType = qPages.getLong();
				std::string expectedPhrase = qPages.getString();
				long pauseTime = qPages.getLong();
				
				HTTPRequest newPage(url, desc, requestType == 1);
				
				newPage.setExpectedPhrase(expectedPhrase);
				newPage.setPauseTime(pauseTime);
				
				// now get any parameters
				
				char szPageID[12];
				memset(szPageID, 0, 12);
				sprintf(szPageID, "%ld", pageID);
				
				SQLiteQuery qParams(*pDB);
				
				std::string paramSQL = "select name, value from scheduled_script_test_page_params where page_id = ";
				paramSQL.append(szPageID);
				
				if (qParams.getResult(paramSQL))
				{
					while (qParams.fetchNext())
					{
						std::string name = qParams.getString();
						std::string value = qParams.getString();
						
						if (!name.empty())
							newPage.addParameter(name, value);
					}
				}				
				
				newScript.addStep(newPage);				
			}
			
			newScript.setAcceptCompressed(acceptCompressed == 1);
			newScript.setDownloadContent(downloadComponents == 1);
			
			items.push_back(newItem);
		}
	}
	
	return true;
}

bool updateScheduledScriptTests(SQLiteDB *pDB, std::vector<ScheduledItem> &items)
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
	
	std::string sql = "select rowid, enabled, description, accept_compressed, download_components, interval, strftime('%s', modified_timestamp) as m_timestamp from scheduled_script_tests";
	
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
			long acceptCompressed = q.getLong();
			long downloadComponents = q.getLong();
			long interval = q.getLong();
			unsigned long modifiedTimestamp = q.getLong();
			
			if (description.empty())
				description = " ";
			
			std::map<unsigned long, unsigned long>::iterator itFind = aCurrentPositions.find(testID);
			
			if (itFind != aCurrentPositions.end())
			{
				unsigned long currentPos = (*itFind).second;
				
				ScheduledItem &currentItem = items.at(currentPos);
				
				if (enabled == 0) // this is now disabled, so lets remove it
				{
					continue; // don't remove it from map so we can remove it from vector
				}
				
				// see if it's been modified
				if (modifiedTimestamp > currentItem.getModifiedTimestamp())
				{
					currentItem.setInterval(interval); // TODO: if interval's less than original, nextTime is still going to be previous
					// interval's time away, so won't get updated immediately
					currentItem.setDescription(description);
					
					currentItem.setModifiedTimestamp(modifiedTimestamp);
					
					SQLiteQuery qPages(*pDB);
					
					char szScriptID[12];
					memset(szScriptID, 0, 12);
					sprintf(szScriptID, "%ld", testID);
					
					std::string pageSQL = "select rowid, description, url, request_type, expected_phrase, pause_time from scheduled_script_test_pages"
					" where script_id = ";
					pageSQL += szScriptID;
					pageSQL += " order by page_num asc";
					
					if (qPages.getResult(pageSQL))
					{
						Script & currentScript = currentItem.getScript();
						
						// we might as well remove them all and re-create them, as we need to cope
						// with removing steps (and therefore shifting succeeding steps down),
						// adding new steps at the end AND in between other steps, and modifications
						// to the parameters of each step even if they haven't been removed/added or
						// had their positions changed
						
						currentScript.clearSteps();
						
						while (qPages.fetchNext())
						{
							long pageID = qPages.getLong();
							std::string desc = qPages.getString();
							std::string url = qPages.getString();
							long requestType = qPages.getLong();
							std::string expectedPhrase = qPages.getString();
							long pauseTime = qPages.getLong();
							
							HTTPRequest newPage(url, desc, requestType == 1);
							
							newPage.setExpectedPhrase(expectedPhrase);
							newPage.setPauseTime(pauseTime);
						
							// now get any parameters
							
							char szPageID[12];
							memset(szPageID, 0, 12);
							sprintf(szPageID, "%ld", pageID);
							
							SQLiteQuery qParams(*pDB);
							
							std::string paramSQL = "select name, value from scheduled_script_test_page_params where page_id = ";
							paramSQL.append(szPageID);
							
							if (qParams.getResult(paramSQL))
							{
								while (qParams.fetchNext())
								{
									std::string name = qParams.getString();
									std::string value = qParams.getString();
									
									if (!name.empty())
										newPage.addParameter(name, value);
								}
							}
							
							currentScript.addStep(newPage);
						}
						
						currentScript.setAcceptCompressed(acceptCompressed == 1);
						currentScript.setDownloadContent(downloadComponents == 1);
					}
				}				
				
				aCurrentPositions.erase(itFind);	
			}
			else if (enabled == 1) // new one, so create it if needed
			{
				ScheduledItem newItem(false, pos++, description, interval, timeNow);
				newItem.setTestID(testID);
				newItem.setModifiedTimestamp(modifiedTimestamp);
				
				SQLiteQuery qPages(*pDB);
				
				char szScriptID[12];
				memset(szScriptID, 0, 12);
				sprintf(szScriptID, "%ld", testID);
				
				std::string pageSQL = "select rowid, description, url, request_type, expected_phrase, pause_time from scheduled_script_test_pages"
										" where script_id = ";
				pageSQL += szScriptID;
				pageSQL += " order by page_num asc";
				
				if (qPages.getResult(pageSQL))
				{
					Script & newScript = newItem.getScript();
					
					while (qPages.fetchNext())
					{
						long pageID = qPages.getLong();
						std::string desc = qPages.getString();
						std::string url = qPages.getString();
						long requestType = qPages.getLong();
						std::string expectedPhrase = qPages.getString();
						long pauseTime = qPages.getLong();
						
						HTTPRequest newPage(url, desc, requestType == 1);
						
						newPage.setExpectedPhrase(expectedPhrase);
						newPage.setPauseTime(pauseTime);
						
						// now get any parameters
						
						char szPageID[12];
						memset(szPageID, 0, 12);
						sprintf(szPageID, "%ld", pageID);
						
						SQLiteQuery qParams(*pDB);
						
						std::string paramSQL = "select name, value from scheduled_script_test_page_params where page_id = ";
						paramSQL.append(szPageID);
						
						if (qParams.getResult(paramSQL))
						{
							while (qParams.fetchNext())
							{
								std::string name = qParams.getString();
								std::string value = qParams.getString();
								
								if (!name.empty())
									newPage.addParameter(name, value);
							}
						}				
						
						newScript.addStep(newPage);			
					}
					
					newScript.setAcceptCompressed(acceptCompressed == 1);
					newScript.setDownloadContent(downloadComponents == 1);
				}
				
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