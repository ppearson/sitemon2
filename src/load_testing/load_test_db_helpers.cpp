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

#include "load_test_db_helpers.h"

bool createNeededLoadTestTables(SQLiteDB *pDB)
{
	std::string sql1 = "create table if not exists load_test_runs (run_time date, description string)";
	
	std::string sql2 = "create table if not exists load_test_overall_results (run_id integer, run_time date, overall_error_code integer, last_response_code integer, steps integer)";
	
	std::string sql3 = "create table if not exists load_test_page_results (run_id integer, overall_result_id integer, page_num integer, run_time date, requested_url string, error_code integer,"
	" response_code integer, lookup_time double, connect_time double, data_start_time double, total_time double, redirect_count integer, content_size integer, download_size integer,"
	" component_content_size integer, component_download_size integer)";
	
	
	
	if (pDB)
	{
		SQLiteQuery q(*pDB, true);
		
		return q.execute(sql1);		
	}
	
	return false;
}