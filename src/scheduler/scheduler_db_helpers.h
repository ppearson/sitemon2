
#ifndef SCHEDULER_DB_HELPERS_H
#define SCHEDULER_DB_HELPERS_H

#include "../utils/sqlite_query.h"
#include "scheduler.h"
#include "http_response.h"

bool createNeededSchedulerTables(SQLiteDB *pDB);
bool createScheduledSingleTestsTable(SQLiteDB *pDB);
bool createScheduledSingleTestResultsTable(SQLiteDB *pDB);

bool getScheduledSingleTestsFromDB(SQLiteDB *pDB, std::vector<ScheduledItem> &items);
bool updateScheduledSingleTests(SQLiteDB *pDB, std::vector<ScheduledItem> &items);

#endif
