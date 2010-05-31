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

#include "http_server_load_testing_helpers.h"

bool getLoadTestRunsList(SQLiteDB *pDB, std::string &output)
{
	if (!pDB)
	{
		output = "No DB Connection";
		return false;
	}	
	
	std::string sql = "select rowid, datetime(run_time,'localtime') as rtime, description from load_test_runs";
	sql += " order by rowid desc limit 40";
	
	SQLiteQuery q(*pDB);
	
	output = "";
	
	char szTemp[2048];
	
	q.getResult(sql);
	while (q.fetchNext())
	{
		memset(szTemp, 0, 2048);
		
		long rowID = q.getLong();
		std::string time = q.getString();
		std::string description = q.getString();
		
		sprintf(szTemp, "<tr>\n <td id=\"l\">%s</td>\n <td id=\"l\">%s</td>\n <td id=\"l\"><a href=\"/load_test_results?run_id=%ld\">View Results</a></td>\n</tr>\n", time.c_str(), description.c_str(), rowID);
		
		output.append(szTemp);		
	}
	
	return true;
}

bool getLoadTestRunResults(SQLiteDB *pDB, HTTPServerRequest &request, std::string &output)
{
	if (!pDB)
	{
		output = "No DB Connection";
		return false;
	}	
	
	std::string sql = "select rowid, datetime(test_time,'localtime') as rtime, overall_error_code, last_response_code, steps from load_test_overall_results";
//	sql += " order by rowid desc limit 40";
	sql += " where run_id = ";
	
	std::string runID = request.getParam("run_id");
	
	sql += runID;
	
	SQLiteQuery q(*pDB);
	
	output = "";
	
	char szTemp[2048];
	
	int count = 1;
	
	q.getResult(sql);
	while (q.fetchNext())
	{
		memset(szTemp, 0, 2048);
		
		long rowID = q.getLong();
		std::string time = q.getString();
		long overallErrorCode = q.getLong();
		long lastResponseCode = q.getLong();
		long steps = q.getLong();
		
		sprintf(szTemp, "<tr>\n <td id=\"l\">%i</td>\n <td id=\"l\">%s</td>\n <td id=\"l\">%ld</td>\n <td id=\"l\">%ld</td>\n <td id=\"l\">%ld</td>\n"
						" <td id=\"l\"><a href=\"/load_test_page_results?overall_result_id=%ld\">View Page Result</a></td>\n</tr>\n", count, time.c_str(), overallErrorCode,
						lastResponseCode, steps, rowID);
		
		output.append(szTemp);
		
		count ++;
	}
	
	return true;
}
