/*
 *  http_server_db_helpers.cpp
 *  sitemon
 *
 *  Created by Peter Pearson on 04/04/2010.
 *  Copyright 2010 __MyCompanyName__. All rights reserved.
 *
 */

#include "http_server_db_helpers.h"
#include "http_server_html_formatters.h"
#include "http_form_generator.h"

bool createNeededTables(SQLiteDB *pDB)
{
	createSingleTestHistoryTable(pDB);
	
	return true;
}

bool createSingleTestHistoryTable(SQLiteDB *pDB)
{
	std::string sql = "create table if not exists single_test_history (run_time date, requested_url string, final_url string, error_code integer, return_code integer, lookup_time double,"
					   "connect_time double, data_start_time double, total_time double, redirect_count integer, content_size integer, download_size integer)";
	
	if (pDB)
	{
		SQLiteQuery q(*pDB, true);
		
		return q.execute(sql);		
	}
	
	return false;
}

bool addResponseToSingleTestHistoryTable(SQLiteDB *pDB, HTTPResponse &response)
{
	if (!pDB)
		return false;
	
	std::string sql = "insert into single_test_history values (datetime('now'), ";
	
	char szTemp[1024];
	memset(szTemp, 0, 1024);
	sprintf(szTemp, "'%s', '%s', %ld, %ld, %f, %f, %f, %f, %ld, %ld, %ld)", response.requestedURL.c_str(), response.finalURL.c_str(), response.errorCode, response.responseCode, response.lookupTime, response.connectTime,
			response.dataStartTime, response.totalTime, response.redirectCount, response.contentSize, response.downloadSize);
	
	sql.append(szTemp);
	
	SQLiteQuery q(*pDB, true);
	
	return q.execute(sql);
}

bool getSingleTestHistoryList(SQLiteDB *pDB, std::string &output, int offset)
{
	if (!pDB)
		return false;
	
	std::string sql = "select rowid, datetime(run_time,'localtime') as rtime, requested_url, error_code, return_code, total_time, download_size,"
							"content_size from single_test_history limit 20 offset ";
	
	char szOffset[8];
	memset(szOffset, 0, 8);
	sprintf(szOffset, "%i", offset);
	sql.append(szOffset);
	
	SQLiteQuery q(*pDB);
	
	output = "";
	
	char szTemp[2048];
	char szResult[6];
	
	q.getResult(sql);
	while (q.fetchNext())
	{
		memset(szTemp, 0, 2048);
		
		long runID = q.getLong();
		std::string time = q.getString();
		std::string url = q.getString();
		long errorCode = q.getLong();
		long returnCode = q.getLong();
		float totalTime = q.getDouble();
		long downloadSize = q.getLong();
		long contentSize = q.getLong();

		memset(szResult, 0, 6);
		if (errorCode == 0)
		{
			strcat(szResult, "OK");
		}
		else
		{
			sprintf(szResult, "%ld", errorCode);
		}

		sprintf(szTemp, "<tr>\n <td><a href=\"/single_details?runid=%ld\">%ld</a></td>\n <td>%s</td>\n <td>%s</td>\n <td>%s</td>\n <td>%ld</td>\n <td>%f</td>\n <td>%ld</td>\n <td>%ld</td></tr>\n",
						runID, runID, time.c_str(), url.c_str(), szResult, returnCode, totalTime, downloadSize, contentSize);
		
		output.append(szTemp);		
	}
	
	return true;
}

bool formatDBSingleTestResponseToHTMLDL(SQLiteDB *pDB, long rowID, std::string &output)
{
	char szRowID[12];
	memset(szRowID, 0, 12);
	
	sprintf(szRowID, "%ld", rowID);
	
	std::string sql = "select datetime(run_time,'localtime') as rtime, requested_url, final_url, error_code, return_code, lookup_time, connect_time, data_start_time, total_time,"
						"redirect_count, download_size, content_size from single_test_history where rowid = ";
	sql.append(szRowID);
	
	SQLiteQuery q(*pDB);
	
	output = "";
	
	q.getResult(sql);
	if (q.fetchNext())
	{
		char szTemp[2048];
		memset(szTemp, 0, 2048);

		std::string format = "<dl>\n";
		addStringToDL(format, "Time");
		addStringToDL(format, "Requested URL");
		addStringToDL(format, "Final URL");
		addStringToDL(format, "Result");
		addLongToDL(format, "Response code");
		addFloatToDL(format, "Lookup time");
		addFloatToDL(format, "Connect time");
		addFloatToDL(format, "Data start time");
		addFloatToDL(format, "Total time");
		addLongToDL(format, "Redirect count");
		addLongToDL(format, "Download size");
		addLongToDL(format, "Content size");
		format += "</dl>\n";
		
		std::string time = q.getString();
		std::string requested_url = q.getString();
		std::string final_url = q.getString();
		long errorCode = q.getLong();
		long responseCode = q.getLong();
		float lookupTime = q.getDouble();
		float connectTime = q.getDouble();
		float dataStartTime = q.getDouble();
		float totalTime = q.getDouble();
		long redirectCount = q.getLong();
		long downloadSize = q.getLong();
		long contentSize = q.getLong();

		char szResult[6];
		memset(szResult, 0, 6);
		if (errorCode == 0)
		{
			strcat(szResult, "OK");
		}
		else
		{
			sprintf(szResult, "%ld", errorCode);
		}
		
		sprintf(szTemp, format.c_str(), time.c_str(), requested_url.c_str(), final_url.c_str(), szResult, responseCode, lookupTime, connectTime,
				dataStartTime, totalTime, redirectCount, contentSize, downloadSize);
		
		output.assign(szTemp);
		
		return true;
	}
	else
	{
		output = "Couldn't find requested runid in database.\n";
	}
	
	return false;
}

bool getSingleScheduledTestsList(SQLiteDB *pDB, std::string &output)
{
	if (!pDB)
		return false;
	
	std::string sql = "select rowid, enabled, description, url, interval, accept_compressed from scheduled_single_tests limit 40";// offset ";
	
/*	char szOffset[8];
	memset(szOffset, 0, 8);
	sprintf(szOffset, "%i", offset);
	sql.append(szOffset);
*/	
	SQLiteQuery q(*pDB);
	
	output = "";
	
	char szTemp[2048];
	
	q.getResult(sql);
	while (q.fetchNext())
	{
		memset(szTemp, 0, 2048);
		
		long testID = q.getLong();
		long enabled = q.getLong();
		std::string description = q.getString();
		std::string url = q.getString();
		long interval = q.getLong();
		long acceptCompressed = q.getLong();

		if (description.empty())
			description = " ";

		if (url.empty())
			url = " ";
		
		std::string strEnabled = (enabled == 1) ? "YES" : "NO";

		std::string strAcceptCompressed = (acceptCompressed == 1) ? "YES" : "NO";

		sprintf(szTemp, "<tr>\n <td><a href=\"/edit_monitor_test?test_id=%ld\">Edit</a></td>\n <td>%s</td>\n <td>%s</td>\n <td>%s</td>\n <td>%ld</td>\n <td>%s</td>\n <td><a href=\"/view_monitortest?testid=%ld\">Results</a></td>\n</tr>\n",
						testID, strEnabled.c_str(), description.c_str(), url.c_str(), interval, strAcceptCompressed.c_str(), testID);
		
		output.append(szTemp);		
	}
	
	return true;
}

bool addSingleScheduledTest(SQLiteDB *pDB, HTTPServerRequest &request, std::string &output)
{
	if (!pDB)
	{
		output = "No DB Connection";
		return false;
	}

	std::string desc = request.getParam("description");
	std::string url = request.getParam("url");
	std::string interval = request.getParam("interval");
	std::string referrer = request.getParam("referrer");
	std::string expectedPhrase = request.getParam("expected_phrase");
	long compressed = 0;
	if (request.getParam("accept_compressed") == "on")
		compressed = 1;

	std::string sql = "insert into scheduled_single_tests values (1, ";
	
	char szTemp[1024];
	memset(szTemp, 0, 1024);
	sprintf(szTemp, "'%s', '%s', '%s', '%s', %s, %ld)", desc.c_str(), url.c_str(), referrer.c_str(), expectedPhrase.c_str(), interval.c_str(), compressed);
	
	sql.append(szTemp);
	
	SQLiteQuery q(*pDB, true);
	
	return q.execute(sql);
}

bool editSingleScheduledTest(SQLiteDB *pDB, HTTPServerRequest &request, std::string &output)
{
	if (!pDB)
	{
		output = "No DB Connection";
		return false;
	}
	
	long rowid = atoi(request.getParam("test_id").c_str());
	long enabled = 0;
	if (request.getParam("enabled") == "on")
		enabled = 1;
	std::string desc = request.getParam("description");
	std::string url = request.getParam("url");
	std::string interval = request.getParam("interval");
	std::string referrer = request.getParam("referrer");
	std::string expectedPhrase = request.getParam("expected_phrase");
	long compressed = 0;
	if (request.getParam("accept_compressed") == "on")
		compressed = 1;
	
	std::string sql = "update scheduled_single_tests set ";
	
	char szTemp[2048];
	memset(szTemp, 0, 2048);
	sprintf(szTemp, "enabled = %ld, description = '%s', url = '%s', referer = '%s', expected_phrase = '%s', interval = %s, accept_compressed = %ld where rowid = %ld", enabled, desc.c_str(),
			url.c_str(), referrer.c_str(), expectedPhrase.c_str(), interval.c_str(), compressed, rowid);
	
	sql.append(szTemp);
	
	SQLiteQuery q(*pDB, true);
	
	return q.execute(sql);
}

bool getSingleScheduledTestResultsList(SQLiteDB *pDB, int testID, std::string &description, std::string &output)
{
	if (!pDB)
	{
		output = "No DB Connection";
		return false;
	}	
	
	char szTestID[12];
	memset(szTestID, 0, 12);
	sprintf(szTestID, "%d", testID);
	
	{
		std::string sql = "select description, url, interval from scheduled_single_tests where rowid = ";
		sql.append(szTestID);
		
		SQLiteQuery q(*pDB);
		
		q.getResult(sql);
		if (q.fetchNext())
		{
			std::string desc = q.getString();
			std::string url = q.getString();
			
			description = desc + " : " + url;			
		}		
	}
	
	std::string sql = "select datetime(run_time,'localtime') as rtime, error_code, response_code, lookup_time, connect_time, data_start_time, total_time, download_size, content_size from scheduled_single_test_results where test_id = ";
	sql.append(szTestID);
	sql += " order by rowid desc limit 40";
	
	SQLiteQuery q(*pDB);

	output = "";
	
	char szTemp[2048];
	
	q.getResult(sql);
	while (q.fetchNext())
	{
		memset(szTemp, 0, 2048);

		std::string time = q.getString();
		long errorCode = q.getLong();
		long responseCode = q.getLong();
		float lookupTime = q.getDouble();
		float connectTime = q.getDouble();
		float dataStartTime = q.getDouble();
		float totalTime = q.getDouble();
		long downloadSize = q.getLong();
		long contentSize = q.getLong();
		
		char szResult[6];
		memset(szResult, 0, 6);
		if (errorCode == 0)
		{
			strcat(szResult, "OK");
		}
		else
		{
			sprintf(szResult, "%ld", errorCode);
		}

		sprintf(szTemp, "<tr>\n <td>%s</td>\n <td>%s</td>\n <td>%ld</td>\n <td>%f</td>\n <td>%f</td>\n <td>%f</td>\n <td>%f</td>\n <td>%ld</td>\n <td>%ld</td>\n</tr>\n",
						time.c_str(), szResult, responseCode, lookupTime, connectTime, dataStartTime, totalTime, downloadSize, contentSize);
		
		output.append(szTemp);		
	}

	return true;
}

bool generateEditSingleScheduledTestForm(SQLiteDB *pDB, int testID, std::string &output)
{
	if (!pDB)
		return false;
	
	std::string sql = "select enabled, interval, description, url, referer, expected_phrase, accept_compressed from scheduled_single_tests where rowid = ";
	
	char szRowID[16];
	memset(szRowID, 0, 16);
	sprintf(szRowID, "%d", testID);
	
	sql.append(szRowID);
	
	SQLiteQuery q(*pDB, true);
	
	q.getResult(sql);
	if (!q.fetchNext())
	{
		output = "Couldn't find requested test to update in db.\n";
		return false;
	}
	
	long enabled = q.getLong();
	long interval = q.getLong();
	std::string description = q.getString();
	std::string url = q.getString();
	std::string referrer = q.getString();
	std::string expectedPhrase = q.getString();
	long acceptCompressed = q.getLong();
		
	HTTPFormGenerator formGen("edit_monitor_test", "Update", true);
	
	HTTPFormCheckItem formEnabled("Enable", "enabled", enabled == 1);
	
	HTTPFormTextItem formDescription("Description", "description", 40, description);
	HTTPFormTextItem formURL("URL", "url", 50, url);
	
	int selInt = 0;
	switch (interval)
	{
		case 5:
			selInt = 1;
			break;
		case 10:
			selInt = 2;
			break;
		case 15:
			selInt = 3;
			break;
		case 20:
			selInt = 4;
			break;
		case 30:
			selInt = 5;
			break;
		case 60:
			selInt = 6;
			break;
		default:
			selInt = 0;
	}
	
	HTTPFormSelectItem formInterval("Interval", "interval", selInt);
	formInterval.addOption("1");
	formInterval.addOption("5");
	formInterval.addOption("10");
	formInterval.addOption("15");
	formInterval.addOption("20");
	formInterval.addOption("30");
	formInterval.addOption("60");
	
	HTTPFormTextItem formReferrer("Referrer", "referrer", 50, referrer);
	HTTPFormTextItem formExpectedPhrase("Expected Phrase", "expected_phrase", 60, expectedPhrase);
	HTTPFormCheckItem formAcceptCompressed("Accept compressed content", "accept_compressed", acceptCompressed == 1);
	HTTPFormHiddenItem formTestID("test_id", testID);
	
	formGen.addItem(formEnabled);
	formGen.addItem(formDescription);
	formGen.addItem(formURL);
	formGen.addItem(formInterval);
	formGen.addItem(formReferrer);
	formGen.addItem(formExpectedPhrase);
	formGen.addItem(formAcceptCompressed);
	formGen.addItem(formTestID);
	
	output = formGen.getGeneratedCode();	
	
	return true;
}