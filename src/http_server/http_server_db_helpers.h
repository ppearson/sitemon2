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

#ifndef HTTP_SERVER_DB_HELPERS_H
#define HTTP_SERVER_DB_HELPERS_H

#include "../http_engine.h"
#include "../utils/sqlite_query.h"
#include "http_server_request.h"

bool createNeededHTTPServerTables(SQLiteDB *pDB);
bool createSingleTestHistoryTable(SQLiteDB *pDB);
bool addResponseToSingleTestHistoryTable(SQLiteDB *pDB, HTTPResponse &response);

bool getSingleTestHistoryList(SQLiteDB *pDB, std::string &output, int offset = 0);
bool formatDBSingleTestResponseToHTMLDL(SQLiteDB *pDB, long rowID, std::string &output);

bool getSingleScheduledTestsList(SQLiteDB *pDB, std::string &output);
bool addSingleScheduledTest(SQLiteDB *pDB, HTTPServerRequest &request, std::string &output);
bool editSingleScheduledTest(SQLiteDB *pDB, HTTPServerRequest &request, std::string &output);

bool getSingleScheduledTestResultsList(SQLiteDB *pDB, int testID, std::string &description, std::string &output);
bool getSingleScheduledTestComponentsList(SQLiteDB *pDB, long testID, long runID, std::string &output);
bool generateEditSingleScheduledTestForm(SQLiteDB *pDB, int testID, std::string &output);

bool getScriptScheduledTestsList(SQLiteDB *pDB, std::string &output);
bool addScriptScheduledTest(SQLiteDB *pDB, HTTPServerRequest &request, std::string &output);

bool generateEditScriptScheduledTestForm(SQLiteDB *pDB, int testID, std::string &scriptSettings, std::string &pages, std::string &addNewPageLink);
bool generateEditScriptScheduledTestPageForm(SQLiteDB *pDB, int pageID, std::string &output);
bool generateAddScriptScheduledTestPageForm(SQLiteDB *pDB, int scriptID, std::string &output);

bool editScriptScheduledTest(SQLiteDB *pDB, HTTPServerRequest &request, std::string &output);
bool editScriptScheduledTestPage(SQLiteDB *pDB, HTTPServerRequest &request, std::string &output);
bool addScriptScheduledTestPage(SQLiteDB *pDB, HTTPServerRequest &request, std::string &output);

#endif
