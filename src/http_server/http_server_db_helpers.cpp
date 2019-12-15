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

#include "http_server_db_helpers.h"
#include "http_server_html_formatters.h"
#include "http_form_generator.h"

#include "../script.h"

#include "utils/string_helper.h"

bool createNeededHTTPServerTables(SQLiteDB *pDB)
{
	createSingleTestHistoryTable(pDB);

	return true;
}

bool createSingleTestHistoryTable(SQLiteDB *pDB)
{
	std::string sql = "create table if not exists single_test_history (run_time date, requested_url string, final_url string, error_code integer, return_code integer, lookup_time double,"
					   "connect_time double, data_start_time double, total_time double, redirect_count integer, content_size integer, download_size integer, component_content_size integer, component_download_size integer)";

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
	sprintf(szTemp, "'%s', '%s', %i, %ld, %f, %f, %f, %f, %ld, %ld, %ld, %ld, %ld)", response.requestedURL.c_str(), response.finalURL.c_str(), response.errorCode, response.responseCode, response.lookupTime, response.connectTime,
			response.dataStartTime, response.totalTime, response.redirectCount, response.contentSize, response.downloadSize, response.componentContentSize, response.componentDownloadSize);

	sql.append(szTemp);

	SQLiteQuery q(*pDB, true);

	return q.execute(sql);
}

bool getSingleTestHistoryList(SQLiteDB *pDB, std::string &output, unsigned int limit, unsigned int offset)
{
	if (!pDB)
		return false;

	std::string sql = "select rowid, datetime(run_time,'localtime') as rtime, requested_url, error_code, return_code, total_time, download_size,"
							"content_size, component_download_size, component_content_size from single_test_history limit ";
	
	char szLimit[8];
	sprintf(szLimit, "%u", limit);
	sql.append(szLimit);
	
	sql += " offset ";

	char szOffset[8];
	memset(szOffset, 0, 8);
	sprintf(szOffset, "%u", offset);
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
		long componentDownloadSize = q.getLong();
		long componentContentSize = q.getLong();

		memset(szResult, 0, 6);
		if (errorCode == 0)
		{
			strcat(szResult, "OK");
		}
		else
		{
			sprintf(szResult, "%ld", errorCode);
		}

		sprintf(szTemp, "<tr>\n <td><a href=\"/single_details?runid=%ld\">%ld</a></td>\n <td>%s</td>\n <td>%s</td>\n <td>%s</td>\n <td>%ld</td>\n <td>%f</td>"
				"\n <td>%ld</td>\n <td>%ld</td>\n <td>%ld</td>\n <td>%ld</td>\n</tr>\n",
						runID, runID, time.c_str(), url.c_str(), szResult, returnCode, totalTime, downloadSize, contentSize, componentDownloadSize, componentContentSize);

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
						"redirect_count, download_size, content_size, component_download_size, component_content_size from single_test_history where rowid = ";
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
		addLongToDL(format, "Component ownload size");
		addLongToDL(format, "Component content size");
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
		long componentDownloadSize = q.getLong();
		long componentContentSize = q.getLong();

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
				dataStartTime, totalTime, redirectCount, contentSize, downloadSize, componentContentSize, componentDownloadSize);

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

	std::string sql = "select rowid, enabled, description, url, interval, accept_compressed, download_components from scheduled_single_tests limit 40";// offset ";

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
		long downloadComponents = q.getLong();

		if (description.empty())
			description = " ";

		if (url.empty())
			url = " ";

		std::string strEnabled = (enabled == 1) ? "YES" : "NO";
		std::string strAcceptCompressed = (acceptCompressed == 1) ? "YES" : "NO";
		std::string strDownloadComponents = (downloadComponents == 1) ? "YES" : "NO";

		sprintf(szTemp, "<tr>\n <td id=\"l\"><a href=\"/edit_monitor_test?test_id=%ld\" title=\"Edit Test\"><img src=\"images/edit.png\"></a> "
				"<a href=\"JavaScript:deleteSingleTest(%ld)\" title=\"Delete test\"><image src=\"images/delete.png\"></a>"
				"</td>\n <td id=\"l\">%s</td>\n <td id=\"l\">%s</td>\n <td id=\"l\">%s</td>\n"
				" <td id=\"l\">%ld</td>\n <td id=\"l\">%s</td>\n <td id=\"l\">%s</td>\n"
				" <td><a href=\"/view_single_test?testid=%ld\"><img src=\"images/view_details.png\" title=\"View Results\"></a> "
				" <a href=\"JavaScript:runManualSingleTest(%ld)\" title=\"Run Manual Test Now\"><image src=\"images/run.png\"></a></td>\n</tr>\n",
						testID, testID, strEnabled.c_str(), description.c_str(), url.c_str(), interval, strAcceptCompressed.c_str(), strDownloadComponents.c_str(), testID, testID);

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
	std::string expectedPhrase = request.getParam("expected_phrase");
	long compressed = 0;
	if (request.getParam("accept_compressed") == "on")
		compressed = 1;
	long downloadComponents = 0;
	if (request.getParam("download_components") == "on")
		downloadComponents = 1;

	std::string sql = "insert into scheduled_single_tests values (1, ";

	char szTemp[1024];
	memset(szTemp, 0, 1024);
	sprintf(szTemp, "'%s', '%s', '%s', %s, %ld, %ld, datetime('now'))", desc.c_str(), url.c_str(), expectedPhrase.c_str(), interval.c_str(), compressed, downloadComponents);

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
	std::string expectedPhrase = request.getParam("expected_phrase");
	long compressed = 0;
	if (request.getParam("accept_compressed") == "on")
		compressed = 1;
	long downloadComponents = 0;
	if (request.getParam("download_components") == "on")
		downloadComponents = 1;

	std::string sql = "update scheduled_single_tests set ";

	char szTemp[2048];
	memset(szTemp, 0, 2048);
	sprintf(szTemp, "enabled = %ld, description = '%s', url = '%s', expected_phrase = '%s', interval = %s, accept_compressed = %ld,"
			"download_components = %ld, modified_timestamp = datetime('now') where rowid = %ld", enabled, desc.c_str(),
			url.c_str(), expectedPhrase.c_str(), interval.c_str(), compressed, downloadComponents, rowid);

	sql.append(szTemp);

	SQLiteQuery q(*pDB, true);

	return q.execute(sql);
}

bool getSingleScheduledTestResultsList(SQLiteDB *pDB, int testID, std::string &description, std::string &output, unsigned int limit, unsigned int offset)
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

	std::string sql = "select rowid, datetime(run_time,'localtime') as rtime, error_code, response_code, lookup_time, connect_time, data_start_time, total_time,"
						"download_size, content_size, component_download_size, component_content_size from scheduled_single_test_results where test_id = ";
	sql.append(szTestID);
	sql += " order by rowid desc limit ";
	
	sql += std::to_string(limit) + " ";
	
	sql += "offset ";
	sql += std::to_string(offset);

	SQLiteQuery q(*pDB);

	output = "";

	char szTemp[2048];

	q.getResult(sql);
	while (q.fetchNext())
	{
		memset(szTemp, 0, 2048);

		long rowID = q.getLong();
		std::string time = q.getString();
		long errorCode = q.getLong();
		long responseCode = q.getLong();
		float lookupTime = q.getDouble();
		float connectTime = q.getDouble();
		float dataStartTime = q.getDouble();
		float totalTime = q.getDouble();
		
		long downloadSize = q.getLong();
		std::string downloadSizeString = StringHelpers::formatNumberThousandsSeparator(downloadSize);
		long contentSize = q.getLong();
		std::string contentSizeString = StringHelpers::formatNumberThousandsSeparator(contentSize);
		long componentDownloadSize = q.getLong();
		std::string componentDownloadSizeString = StringHelpers::formatNumberThousandsSeparator(componentDownloadSize);
		long componentContentSize = q.getLong();
		std::string componentContentSizeString = StringHelpers::formatNumberThousandsSeparator(componentContentSize);

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

		sprintf(szTemp, "<tr>\n <td id=\"l\">%s</td>\n <td id=\"l\">%s</td>\n <td id=\"l\">%ld</td>\n <td>%f</td>\n <td>%f</td>\n <td>%f</td>\n <td>%f</td>\n <td>%s</td>\n <td>%s</td>\n <td>%s</td>\n"
				"<td>%s</td>\n <td><a href=\"/single_components?test_id=%i&run_id=%ld\">View</a></td>\n</tr>\n", time.c_str(), szResult,
				responseCode, lookupTime, connectTime, dataStartTime, totalTime, downloadSizeString.c_str(), contentSizeString.c_str(),
				componentDownloadSizeString.c_str(), componentContentSizeString.c_str(), testID, rowID);

		output.append(szTemp);
	}

	return true;
}

bool getSingleScheduledTestComponentsList(SQLiteDB *pDB, long testID, long runID, std::string &output)
{
	if (!pDB)
	{
		output = "No DB Connection";
		return false;
	}

	char szTestID[12];
	memset(szTestID, 0, 12);
	sprintf(szTestID, "%ld", testID);

	char szRunID[12];
	memset(szRunID, 0, 12);
	sprintf(szRunID, "%ld", runID);

	std::string sql = "select url, error_code, response_code, lookup_time, connect_time, data_start_time, total_time,"
	"download_size, content_size from scheduled_single_test_component_results where test_id = ";
	sql.append(szTestID);
	sql += " and run_id = ";
	sql.append(szRunID);

	SQLiteQuery q(*pDB);

	output = "";

	char szTemp[2048];

	q.getResult(sql);
	while (q.fetchNext())
	{
		memset(szTemp, 0, 2048);

		std::string url = q.getString();
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

		sprintf(szTemp, "<tr>\n <td id=\"l\">%s</td>\n <td>%s</td>\n <td>%ld</td>\n <td>%f</td>\n <td>%ld</td>\n <td>%ld</td>\n</tr>\n",
				url.c_str(), szResult, responseCode, totalTime, downloadSize, contentSize);

		output.append(szTemp);
	}

	return true;
}

bool generateEditSingleScheduledTestForm(SQLiteDB *pDB, int testID, std::string &output)
{
	if (!pDB)
		return false;

	std::string sql = "select enabled, interval, description, url, expected_phrase, accept_compressed, download_components from scheduled_single_tests where rowid = ";

	char szRowID[16];
	memset(szRowID, 0, 16);
	sprintf(szRowID, "%d", testID);

	sql.append(szRowID);

	SQLiteQuery q(*pDB);

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
	std::string expectedPhrase = q.getString();
	long acceptCompressed = q.getLong();
	long downloadComponents = q.getLong();

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

	HTTPFormTextItem formExpectedPhrase("Expected Phrase", "expected_phrase", 60, expectedPhrase);
	HTTPFormCheckItem formAcceptCompressed("Accept compressed content", "accept_compressed", acceptCompressed == 1);
	HTTPFormCheckItem formDownloadComponents("Download components", "download_components", downloadComponents == 1);

	HTTPFormHiddenItem formTestID("test_id", testID);

	formGen.addItem(formEnabled);
	formGen.addItem(formDescription);
	formGen.addItem(formURL);
	formGen.addItem(formInterval);
	formGen.addItem(formExpectedPhrase);
	formGen.addItem(formAcceptCompressed);
	formGen.addItem(formDownloadComponents);
	formGen.addItem(formTestID);

	output = formGen.getGeneratedCode();

	return true;
}

bool getScriptScheduledTestsList(SQLiteDB *pDB, std::string &output)
{
	if (!pDB)
		return false;

	std::string sql = "select rowid, enabled, description, interval, accept_compressed, download_components from scheduled_script_tests limit 40";

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
		long interval = q.getLong();
		long acceptCompressed = q.getLong();
		long downloadComponents = q.getLong();

		if (description.empty())
			description = " ";

		std::string strEnabled = (enabled == 1) ? "YES" : "NO";
		std::string strAcceptCompressed = (acceptCompressed == 1) ? "YES" : "NO";
		std::string strDownloadComponents = (downloadComponents == 1) ? "YES" : "NO";

		sprintf(szTemp, "<tr>\n <td id=\"l\"><a href=\"/edit_script_test?test_id=%ld\" title=\"Edit Test\"><img src=\"images/edit.png\"></a> "
				"<a href=\"JavaScript:deleteScriptTest(%ld)\" title=\"Delete test\"><image src=\"images/delete.png\"></a>"
				"</td>\n <td id=\"l\">%s</td>\n <td id=\"l\">%s</td>"
				" <td id=\"l\">%ld</td>\n <td id=\"l\">%s</td>\n <td id=\"l\">%s</td>\n"
				" <td><a href=\"/view_script_test?testid=%ld\"><img src=\"images/view_details.png\" title=\"View Results\"></a>"
				" <a href=\"JavaScript:runManualScriptTest(%ld)\" title=\"Run Manual Test Now\"><image src=\"images/run.png\"></a></td>\n</tr>\n",
				testID, testID, strEnabled.c_str(), description.c_str(), interval, strAcceptCompressed.c_str(), strDownloadComponents.c_str(), testID, testID);

		output.append(szTemp);
	}

	return true;
}

bool addScriptScheduledTest(SQLiteDB *pDB, HTTPServerRequest &request, std::string &output)
{
	if (!pDB)
	{
		output = "No DB Connection";
		return false;
	}

	std::string desc = request.getParam("description");
	std::string interval = request.getParam("interval");
	long compressed = 0;
	if (request.getParam("accept_compressed") == "on")
		compressed = 1;
	long downloadComponents = 0;
	if (request.getParam("download_components") == "on")
		downloadComponents = 1;

	std::string sql = "insert into scheduled_script_tests values (1, ";

	char szTemp[1024];
	memset(szTemp, 0, 1024);
	sprintf(szTemp, "'%s', '%s', %ld, %ld, datetime('now'))", desc.c_str(), interval.c_str(), compressed, downloadComponents);

	sql.append(szTemp);

	SQLiteQuery q(*pDB, true);

	if (!q.execute("BEGIN IMMEDIATE"))
	{
		printf("problem starting adding script transaction\n");
		return false;
	}

	bool ret = q.execute(sql);

	char szDesc[64];
	char szURL[64];

	// now create basic steps for each of the pages specified
	if (ret)
	{
		for (int i = 1; i < 10; i++)
		{
			unsigned long scriptID = q.getInsertRowID();

			memset(szDesc, 0, 64);
			memset(szURL, 0, 64);

			sprintf(szDesc, "p%i_desc", i);
			sprintf(szURL, "p%i_url", i);

			long pageNum = i;

			if (request.hasParam(szDesc) && request.hasParam(szURL))
			{
				memset(szTemp, 0, 1024);

				std::string pageDesc = request.getParam(szDesc);
				std::string pageURL = request.getParam(szURL);

				sprintf(szTemp, "insert into scheduled_script_test_pages values (%ld, %ld, '%s', '%s', %i, '', 5)", scriptID, pageNum,
									pageDesc.c_str(), pageURL.c_str(), 0);

				std::string sql2 = szTemp;

				if (!q.execute(sql2))
				{
					printf("Couldn't add script page item...\n");
				}
			}
			else
			{
				break;
			}
		}
	}

	if (!q.execute("COMMIT"))
	{
		printf("problem committing add script transaction\n");
		return false;
	}

	return true;
}

bool generateEditScriptScheduledTestForm(SQLiteDB *pDB, int testID, std::string &scriptSettings, std::string &pages, std::string &addNewPageLink)
{
	if (!pDB)
		return false;

	std::string sql = "select enabled, description, interval, accept_compressed, download_components from scheduled_script_tests where rowid = ";

	char szRowID[16];
	memset(szRowID, 0, 16);
	sprintf(szRowID, "%d", testID);

	sql.append(szRowID);

	SQLiteQuery q(*pDB);

	q.getResult(sql);
	if (!q.fetchNext())
	{
		scriptSettings = "Couldn't find requested script test in db.\n";
		return false;
	}

	long enabled = q.getLong();
	std::string description = q.getString();
	long interval = q.getLong();
	long acceptCompressed = q.getLong();
	long downloadComponents = q.getLong();

	HTTPFormGenerator formGen("edit_script_test", "Update", true);

	HTTPFormCheckItem formEnabled("Enabled", "enabled", enabled == 1);

	HTTPFormTextItem formDescription("Description", "description", 40, description);

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

	HTTPFormCheckInlineItem formAcceptCompressed("Accept compressed content", "accept_compressed", acceptCompressed == 1);
	HTTPFormCheckInlineItem formDownloadComponents("Download components", "download_components", downloadComponents == 1);

	HTTPFormHiddenItem formTestID("test_id", testID);

	formGen.addItem(formEnabled);
	formGen.addItem(formDescription);
	formGen.addItem(formInterval);
	formGen.addItem(formAcceptCompressed);
	formGen.addItem(formDownloadComponents);
	formGen.addItem(formTestID);

	scriptSettings = formGen.getGeneratedCode();

	// now generate the list of pages

	q.freeResults();

	std::string sql2 = "select rowid, description, url, request_type, expected_phrase from scheduled_script_test_pages where script_id = ";
	sql2.append(szRowID);

	sql2 += " order by page_num";

	char szTemp[4096];

	q.getResult(sql2);
	int pageNum = 1;
	while (q.fetchNext())
	{
		memset(szTemp, 0, 4096);

		long pageID = q.getLong();
		description = q.getString();
		std::string url = q.getString();
		long requestType = q.getLong();
		std::string expectedPhrase = q.getString();

		if (description.empty())
			description = " ";

		std::string strRequestType = (requestType == 1) ? "POST" : "GET";

		sprintf(szTemp, "<tr>\n <td id=\"l\">%i</td>\n<td id=\"l\"><a href=\"/edit_script_page?page_id=%ld\" title=\"Edit Step\"><img src=\"images/edit.png\"></a> "
						"<a href=\"JavaScript:deleteScriptStep(%i, %ld)\" title=\"Delete Step\"><image src=\"images/delete.png\"></a>"
						"</td>\n <td id=\"l\">%s</td>\n <td id=\"l\">%s</td>"
				" <td id=\"l\">%s</td>\n <td id=\"l\">%s</td>\n", pageNum, pageID, testID, pageID, description.c_str(), url.c_str(), strRequestType.c_str(),
				expectedPhrase.c_str());

		pages.append(szTemp);

		pageNum ++;
	}

	addNewPageLink.assign(szRowID);

	return true;
}

bool generateEditScriptScheduledTestPageForm(SQLiteDB *pDB, int pageID, std::string &output)
{
	if (!pDB)
		return false;

	SQLiteQuery q(*pDB);

	std::string sql = "select script_id, page_num, description, url, request_type, expected_phrase, pause_time from scheduled_script_test_pages where rowid = ";

	char szPageID[16];
	memset(szPageID, 0, 16);
	sprintf(szPageID, "%d", pageID);

	sql.append(szPageID);
	sql += " order by page_num";

	q.getResult(sql);
	if (!q.fetchNext())
	{
		output = "Couldn't find requested script page in db.\n";
		return false;
	}

	long scriptID = q.getLong();
	std::string pageNumber = q.getString();
	std::string description = q.getString();
	std::string url = q.getString();
	long requestType = q.getLong();
	std::string expectedPhrase = q.getString();
	long pauseTime = q.getLong();

	HTTPFormGenerator formGen("edit_script_page", "Update", true);

	HTTPFormTextItem formNumber("Number", "number", 10, pageNumber);

	HTTPFormTextItem formDescription("Description", "description", 40, description);
	HTTPFormTextItem formURL("URL", "url", 60, url);

	HTTPFormSelectItem formRequestType("Request type", "request_type", requestType);
	formRequestType.addOption("GET");
	formRequestType.addOption("POST");

	HTTPFormTextItem formExpectedPhrase("Expected Phrase", "expected_phrase", 50, expectedPhrase);

	HTTPFormTextItem formPauseTime("Pause time", "pause_time", 20, pauseTime);

	HTTPFormHiddenItem formPageID("page_id", pageID);
	HTTPFormHiddenItem formScriptID("script_id", scriptID);

	formGen.addItem(formNumber);
	formGen.addItem(formDescription);
	formGen.addItem(formURL);
	formGen.addItem(formRequestType);
	formGen.addItem(formExpectedPhrase);
	formGen.addItem(formPauseTime);
	formGen.addItem(formPageID);
	formGen.addItem(formScriptID);

	// now list the page parameters, plus some blank ones

	q.freeResults();

	std::string sql2 = "select name, value from scheduled_script_test_page_params where page_id = ";
	sql2.append(szPageID);

	char szLabel[16];

	char szEditName[16];
	char szEditValue[16];


	int paramNum = 1;

	if (q.getResult(sql2))
	{
		formGen.addSeparator();

		while (q.fetchNext())
		{
			std::string paramName = q.getString();
			std::string paramValue = q.getString();

			memset(szLabel, 0, 16);
			memset(szEditName, 0, 16);
			memset(szEditValue, 0, 16);

			sprintf(szLabel, "Param %i", paramNum);
			sprintf(szEditName, "n%i", paramNum);
			sprintf(szEditValue, "v%i", paramNum);

			formGen.addParameterPair(szLabel, szEditName, paramName, szEditValue, paramValue);

			paramNum ++;
		}
	}

	formGen.addSeparator();

	// and some blank ones

	for (int i = 0; i < 5; i++)
	{
		memset(szLabel, 0, 16);
		memset(szEditName, 0, 16);
		memset(szEditValue, 0, 16);

		sprintf(szLabel, "Param %i", paramNum);
		sprintf(szEditName, "n%i", paramNum);
		sprintf(szEditValue, "v%i", paramNum);

		formGen.addParameterPair(szLabel, szEditName, "", szEditValue, "");

		paramNum ++;
	}

	output = formGen.getGeneratedCode();

	return true;
}

bool generateAddScriptScheduledTestPageForm(SQLiteDB *pDB, int scriptID, std::string &output)
{
	if (!pDB)
		return false;

	SQLiteQuery q(*pDB);

	std::string sql = "select max(page_num) from scheduled_script_test_pages where script_id = ";

	char szScriptID[16];
	memset(szScriptID, 0, 16);
	sprintf(szScriptID, "%d", scriptID);

	sql.append(szScriptID);

	q.getResult(sql);
	if (!q.fetchNext())
	{
		output = "Couldn't find requested script in db.\n";
		return false;
	}

	long numberOfPages = q.getLong();

	char szPageNumber[4];
	sprintf(szPageNumber, "%ld", numberOfPages + 1);
	std::string pageNumber(szPageNumber);

	HTTPFormGenerator formGen("add_script_page", "Add", true);

	HTTPFormTextItem formNumber("Number", "number", 10, pageNumber);

	HTTPFormTextItem formDescription("Description", "description", 40, "");
	HTTPFormTextItem formURL("URL", "url", 60, "");

	HTTPFormSelectItem formRequestType("Request type", "request_type", 0);
	formRequestType.addOption("GET");
	formRequestType.addOption("POST");

	HTTPFormTextItem formExpectedPhrase("Expected Phrase", "expected_phrase", 50, "");

	HTTPFormTextItem formPauseTime("Pause time", "pause_time", 20, "5");

	HTTPFormHiddenItem formPageID("script_id", scriptID);

	formGen.addItem(formNumber);
	formGen.addItem(formDescription);
	formGen.addItem(formURL);
	formGen.addItem(formRequestType);
	formGen.addItem(formExpectedPhrase);
	formGen.addItem(formPauseTime);
	formGen.addItem(formPageID);

	formGen.addSeparator();

	char szLabel[16];
	char szEditName[16];
	char szEditValue[16];

	int paramNum = 1;

	// and some blank ones

	for (int i = 0; i < 8; i++)
	{
		memset(szLabel, 0, 16);
		memset(szEditName, 0, 16);
		memset(szEditValue, 0, 16);

		sprintf(szLabel, "Param %i", paramNum);
		sprintf(szEditName, "n%i", paramNum);
		sprintf(szEditValue, "v%i", paramNum);

		formGen.addParameterPair(szLabel, szEditName, "", szEditValue, "");

		paramNum ++;
	}

	output = formGen.getGeneratedCode();

	return true;
}

bool editScriptScheduledTest(SQLiteDB *pDB, HTTPServerRequest &request, std::string &output)
{
	if (!pDB)
	{
		output = "No DB Connection";
		return false;
	}

	long enabled = 0;
	if (request.getParam("enabled") == "on")
		enabled = 1;

	std::string strTestID = request.getParam("test_id");
	long testID = 0;
	if (!strTestID.empty())
	{
		testID = atol(strTestID.c_str());
	}

	std::string desc = request.getParam("description");
	std::string interval = request.getParam("interval");
	long compressed = 0;
	if (request.getParam("accept_compressed") == "on")
		compressed = 1;
	long downloadComponents = 0;
	if (request.getParam("download_components") == "on")
		downloadComponents = 1;

	std::string sql = "update scheduled_script_tests set ";

	char szTemp[1024];
	memset(szTemp, 0, 1024);
	sprintf(szTemp, "enabled = %ld, description = '%s', interval = '%s', accept_compressed = %ld, download_components = %ld, modified_timestamp = datetime('now') where rowid = %ld",
					enabled, desc.c_str(), interval.c_str(), compressed, downloadComponents, testID);

	sql.append(szTemp);

	SQLiteQuery q(*pDB, true);

	bool ret = q.execute(sql);

	return ret;
}

bool editScriptScheduledTestPage(SQLiteDB *pDB, HTTPServerRequest &request, std::string &output)
{
	if (!pDB)
	{
		output = "No DB Connection";
		return false;
	}

	std::string strPageID = request.getParam("page_id");
	long pageID = 0;
	if (!strPageID.empty())
	{
		pageID = atol(strPageID.c_str());
	}

	std::string strScriptID = request.getParam("script_id");

	std::string number = request.getParam("number");

	std::string desc = request.getParam("description");
	std::string url = request.getParam("url");

	long requestType = 0;
	std::string strRequestType = request.getParam("request_type");
	if (strRequestType == "POST")
		requestType = 1;

	std::string expectedPhrase = request.getParam("expected_phrase");

	long pauseTime = 0;
	std::string strPauseTime = request.getParam("pause_time");
	if (!strPauseTime.empty())
		pauseTime = atol(strPauseTime.c_str());

	std::string sql = "update scheduled_script_test_pages set ";

	char szTemp[1024];
	memset(szTemp, 0, 1024);
	sprintf(szTemp, "page_num = '%s', description = '%s', url = '%s', request_type = %ld, expected_phrase = '%s', pause_time = %ld where rowid = %ld",
					number.c_str(), desc.c_str(), url.c_str(), requestType, expectedPhrase.c_str(), pauseTime, pageID);

	sql.append(szTemp);

	SQLiteQuery q(*pDB, true);

	// update script modified timestamp first
	std::string scriptSQL = "update scheduled_script_tests set modified_timestamp = datetime('now') where rowid = " + strScriptID;

	bool ret = q.execute(scriptSQL);

	ret = q.execute(sql);

	if (ret)
	{
		// now delete all the params for this page then re-add them

		std::string sql2 = "delete from scheduled_script_test_page_params where page_id = ";
		sql2 += strPageID;

		if (q.execute(sql2))
		{
			char szParamName[16];
			char szParamValue[16];

			for (int i = 1; i < 50; i++)
			{
				memset(szParamName, 0, 16);
				memset(szParamValue, 0, 16);

				sprintf(szParamName, "n%i", i);
				sprintf(szParamValue, "v%i", i);

				if (request.hasParam(szParamName) && request.hasParam(szParamValue))
				{
					memset(szTemp, 0, 1024);

					std::string paramName = request.getParam(szParamName);
					std::string paramValue = request.getParam(szParamValue);

					sprintf(szTemp, "insert into scheduled_script_test_page_params values (%ld, '%s', '%s')", pageID,
										paramName.c_str(), paramValue.c_str());

					std::string sql3 = szTemp;

					if (!q.execute(sql3))
					{
						printf("Couldn't add script page param item...\n");
					}
				}
				else
				{
					break;
				}
			}
		}
		else
		{
			return false;
		}
	}
	else
	{
		return false;
	}

	return true;
}

bool addScriptScheduledTestPage(SQLiteDB *pDB, HTTPServerRequest &request, std::string &output)
{
	if (!pDB)
	{
		output = "No DB Connection";
		return false;
	}

	std::string strScriptID = request.getParam("script_id");
	long scriptID = 0;
	if (!strScriptID.empty())
	{
		scriptID = atol(strScriptID.c_str());
	}

	long pageNumber = 0;
	if (request.hasParam("number"))
	{
		pageNumber = atol(request.getParam("number").c_str());
	}

	std::string desc = request.getParam("description");
	std::string url = request.getParam("url");

	long requestType = 0;
	std::string strRequestType = request.getParam("request_type");
	if (strRequestType == "POST")
		requestType = 1;

	std::string expectedPhrase = request.getParam("expected_phrase");

	long pauseTime = 0;
	std::string strPauseTime = request.getParam("pause_time");
	if (!strPauseTime.empty())
		pauseTime = atol(strPauseTime.c_str());

	std::string sql = "insert into scheduled_script_test_pages values(";

	char szTemp[1024];
	memset(szTemp, 0, 1024);
	sprintf(szTemp, "%ld, %ld, '%s', '%s', %ld, '%s', %ld)",
			scriptID, pageNumber, desc.c_str(), url.c_str(), requestType, expectedPhrase.c_str(), pauseTime);

	sql.append(szTemp);

	SQLiteQuery q(*pDB, true);

	bool ret = q.execute(sql);

	if (ret)
	{
		// update script modified timestamp first
		std::string scriptSQL = "update scheduled_script_tests set modified_timestamp = datetime('now') where rowid = " + strScriptID;

		ret = q.execute(scriptSQL);

		unsigned long pageID = q.getInsertRowID();

		char szParamName[16];
		char szParamValue[16];

		for (int i = 1; i < 50; i++)
		{
			memset(szParamName, 0, 16);
			memset(szParamValue, 0, 16);

			sprintf(szParamName, "n%i", i);
			sprintf(szParamValue, "v%i", i);

			if (request.hasParam(szParamName) && request.hasParam(szParamValue))
			{
				memset(szTemp, 0, 1024);

				std::string paramName = request.getParam(szParamName);
				std::string paramValue = request.getParam(szParamValue);

				sprintf(szTemp, "insert into scheduled_script_test_page_params values (%ld, '%s', '%s')", pageID,
						paramName.c_str(), paramValue.c_str());

				std::string sql3 = szTemp;

				if (!q.execute(sql3))
				{
					printf("Couldn't add script page param item...\n");
				}
			}
			else
			{
				break;
			}
		}
	}
	else
	{
		output = "Couldn't add new script page to db.";
		return false;
	}

	return true;
}

bool getScriptScheduledTestResultsList(SQLiteDB *pDB, int testID, std::string &description, std::string &output)
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
		std::string sql = "select description from scheduled_script_tests where rowid = ";
		sql.append(szTestID);

		SQLiteQuery q(*pDB);

		q.getResult(sql);
		if (q.fetchNext())
		{
			std::string desc = q.getString();

			description = desc;
		}
	}

	std::string sql = "select rowid, datetime(run_time,'localtime') as rtime, overall_error_code, last_response_code, steps from scheduled_script_test_results where test_id = ";
	sql.append(szTestID);
	sql += " order by rowid desc limit 40";

	SQLiteQuery q(*pDB);

	output = "";

	char szTemp[2048];

	q.getResult(sql);
	while (q.fetchNext())
	{
		memset(szTemp, 0, 2048);

		long runID = q.getLong();
		std::string time = q.getString();
		long overallErrorCode = q.getLong();
		long lastResponseCode = q.getLong();
		long steps = q.getLong();

		char szResult[6];
		memset(szResult, 0, 6);
		if (overallErrorCode == 0)
		{
			strcat(szResult, "OK");
		}
		else
		{
			sprintf(szResult, "%ld", overallErrorCode);
		}

		sprintf(szTemp, "<tr>\n <td id=\"l\">%s</td>\n <td id=\"l\">%s</td>\n <td id=\"l\">%ld</td>\n <td>%ld</td>\n <td><a href=\"/script_details?test_id=%i&run_id=%ld\">View Details</a></td>\n</tr>\n",
				time.c_str(), szResult,	lastResponseCode, steps, testID, runID);

		output.append(szTemp);
	}

	return true;
}

bool getScriptScheduledTestResultsDetails(SQLiteDB *pDB, int testID, long runID, std::string &output)
{
	if (!pDB)
	{
		output = "No DB Connection";
		return false;
	}

	char szTestID[12];
	memset(szTestID, 0, 12);
	sprintf(szTestID, "%i", testID);

	char szRunID[12];
	memset(szRunID, 0, 12);
	sprintf(szRunID, "%ld", runID);

	std::string sql = "select datetime(run_time,'localtime') as rtime, requested_url, error_code, response_code, lookup_time, connect_time, data_start_time, total_time,"
						"redirect_count, download_size, content_size, component_download_size, component_content_size from scheduled_script_test_page_results where test_id = ";
	sql.append(szTestID);
	sql += " and run_id = ";
	sql.append(szRunID);
	sql += " order by page_num";

	SQLiteQuery q(*pDB);

	output = "";

	char szTemp[2048];

	int page = 1;

	q.getResult(sql);
	while (q.fetchNext())
	{
		memset(szTemp, 0, 2048);

		std::string time = q.getString();
		std::string url = q.getString();
		long errorCode = q.getLong();
		long responseCode = q.getLong();
		float lookupTime = q.getDouble();
		float connectTime = q.getDouble();
		float dataStartTime = q.getDouble();
		float totalTime = q.getDouble();
		long redirectCount = q.getLong();
		long downloadSize = q.getLong();
		long contentSize = q.getLong();
		long componentDownloadSize = q.getLong();
		long componentContentSize = q.getLong();

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

		sprintf(szTemp, "<tr>\n <td id=\"l\">%i</td>\n <td id=\"l\">%s</td>\n <td id=\"l\">%s</td>\n <td>%s</td>\n <td>%ld</td>\n <td>%f</td>\n <td>%f</td>\n <td>%f</td>\n <td>%f</td>\n <td>%ld</td>\n <td>%ld</td>\n <td>%ld</td>\n</tr>\n",
				page, time.c_str(), url.c_str(), szResult, responseCode, lookupTime, connectTime, dataStartTime, totalTime, redirectCount, downloadSize, contentSize);

		output.append(szTemp);

		page ++;
	}

	return true;
}

bool deleteSingleTestFromDB(SQLiteDB *pDB, unsigned long testID, std::string &output)
{
	if (!pDB)
	{
		output = "No DB Connection";
		return false;
	}

	char szTestID[16];
	memset(szTestID, 0, 16);
	sprintf(szTestID, "%ld", testID);

	std::string sql1 = "delete from scheduled_single_tests where rowid = ";
	sql1.append(szTestID);

	std::string sql2 = "delete from scheduled_single_test_results where test_id = ";
	sql2.append(szTestID);

	std::string sql3 = "delete from scheduled_single_test_component_results where test_id = ";
	sql3.append(szTestID);

	SQLiteQuery q(*pDB, true);

	bool ret1 = q.execute(sql1);
	bool ret2 = q.execute(sql2);
	bool ret3 = q.execute(sql3);

	return ret1 && ret2 && ret3;
}

bool deleteScriptTestFromDB(SQLiteDB *pDB, unsigned long testID, std::string &output)
{
	if (!pDB)
	{
		output = "No DB Connection";
		return false;
	}

	char szTestID[16];
	memset(szTestID, 0, 16);
	sprintf(szTestID, "%ld", testID);

	std::string sql1 = "delete from scheduled_script_tests where rowid = ";
	sql1.append(szTestID);

	// build up a list of pageIDs
	std::vector<unsigned long> aPageIDs;

	std::string sql2 = "select rowid from scheduled_script_test_pages where script_id = ";
	sql2.append(szTestID);

	SQLiteQuery q(*pDB, true);

	q.getResult(sql2);
	while (q.fetchNext())
	{
		unsigned long pageID = q.getULong();

		aPageIDs.push_back(pageID);
	}

	std::string sql3 = "delete from scheduled_script_test_pages where script_id = ";
	sql3.append(szTestID);

	char szTemp[256];

	std::vector<unsigned long>::iterator itPI = aPageIDs.begin();
	std::vector<unsigned long>::iterator itPIEnd = aPageIDs.end();
	for (; itPI != itPIEnd; ++itPI)
	{
		memset(szTemp, 0, 256);
		sprintf(szTemp, "delete from scheduled_script_test_page_params where page_id = %ld", *itPI);

		std::string sql4(szTemp);

		if (!q.execute(sql4))
		{
			printf("Couldn't delete params for script page...\n");
		}
	}

	// now delete results

	std::string sql5 = "delete from scheduled_script_test_results where test_id = ";
	sql5.append(szTestID);

	std::string sql6 = "delete from scheduled_script_test_page_results where test_id = ";
	sql6.append(szTestID);

	bool ret1 = q.execute(sql1);
	bool ret2 = q.execute(sql3);
	bool ret3 = q.execute(sql5);
	bool ret4 = q.execute(sql6);

	return ret1 && ret2 && ret3 && ret4;
}

bool deleteScriptStepFromDB(SQLiteDB *pDB, unsigned long testID, unsigned long pageID, std::string &output)
{
	if (!pDB)
	{
		output = "No DB Connection";
		return false;
	}

	char szTestID[16];
	memset(szTestID, 0, 16);
	sprintf(szTestID, "%ld", testID);

	char szPageID[16];
	memset(szPageID, 0, 16);
	sprintf(szPageID, "%ld", pageID);

	std::string sql1 = "delete from scheduled_script_test_pages where rowid = ";
	sql1.append(szPageID);

	std::string sql2 = "delete from scheduled_script_test_page_params where page_id = ";
	sql2.append(szPageID);

	std::string sql3 = "update scheduled_script_tests set modified_timestamp = datetime('now') where rowid = ";
	sql3.append(szTestID);

	SQLiteQuery q(*pDB, true);

	bool ret1 = q.execute(sql1);
	bool ret2 = q.execute(sql2);
	bool ret3 = q.execute(sql3);

	return ret1 && ret2 && ret3;
}

bool runManualSingleTest(SQLiteDB *pDB, ScheduledResultsSaver *pSaver, unsigned long testID)
{
	if (!pDB || !pSaver)
	{
		return false;
	}

	std::string sql = "select url, expected_phrase, accept_compressed, download_components from scheduled_single_tests where rowid = ";
	char szTestID[12];
	memset(szTestID, 0, 12);
	sprintf(szTestID, "%ld", testID);
	sql.append(szTestID);

	SQLiteQuery q(*pDB);

	long schedID = 0;

	q.getResult(sql);
	if (q.fetchNext())
	{
		std::string url = q.getString();
		std::string expectedPhrase = q.getString();
		long acceptCompressed = q.getLong();
		long downloadComponents = q.getLong();

		if (url.empty())
			return false;

		HTTPRequest newRequest(url);
		newRequest.setExpectedPhrase(expectedPhrase);
		newRequest.setAcceptCompressed(acceptCompressed == 1);
		newRequest.setDownloadContent(downloadComponents == 1);

		HTTPEngine engine;
		HTTPResponse response;

		engine.performRequest(newRequest, response);

		pSaver->addResult(response, testID);

		return true;
	}

	return false;
}

bool runManualScriptTest(SQLiteDB *pDB, ScheduledResultsSaver *pSaver, unsigned long testID)
{
	if (!pDB || !pSaver)
	{
		return false;
	}

	std::string sql = "select accept_compressed, download_components from scheduled_script_tests where rowid = ";
	char szTestID[12];
	memset(szTestID, 0, 12);
	sprintf(szTestID, "%ld", testID);
	sql.append(szTestID);

	SQLiteQuery q(*pDB);

	q.getResult(sql);
	if (q.fetchNext())
	{
		long acceptCompressed = q.getLong();
		long downloadComponents = q.getLong();

		SQLiteQuery qPages(*pDB);

		std::string pageSQL = "select rowid, description, url, request_type, expected_phrase, pause_time from scheduled_script_test_pages"
								" where script_id = ";
		pageSQL += szTestID;
		pageSQL += " order by page_num asc";

		if (qPages.getResult(pageSQL))
		{
			Script newScript;

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

			HTTPEngine engine;
			ScriptResult result;

			for (std::vector<HTTPRequest>::const_iterator it = newScript.begin(); it != newScript.end(); ++it)
			{
				const HTTPRequest &request = *it;

				HTTPResponse response;

				if (engine.performRequest(request, response))
				{
					result.addResponse(response);
				}
				else
				{
					result.addResponse(response);

					break; // break out and end, as there's been an issue
				}

				if (request.getPauseTime() > 0)
				{
					Thread::sleep(request.getPauseTime());
				}
			}

			pSaver->addResult(result, testID);
		}
	}

	return false;
}
