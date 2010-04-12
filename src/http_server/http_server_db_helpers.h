
#ifndef HTTP_SERVER_DB_HELPERS_H
#define HTTP_SERVER_DB_HELPERS_H

#include "http_engine.h"
#include "../utils/sqlite_query.h"
#include "http_server_request.h"

bool createNeededTables(SQLiteDB *pDB);
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

#endif
