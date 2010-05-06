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

#ifndef LOAD_TEST_RESULTS_SAVER_H
#define LOAD_TEST_RESULTS_SAVER_H

#include <string>
#include <vector>

#include "../script_result.h"

#include "../script.h"

#include "../utils/thread.h"
#include "../utils/sqlite_query.h"

class StepResults
{
public:
	StepResults(int step, const std::string &description);
	
	void addResult(const HTTPResponse &response) { m_aResults.push_back(response); }
	
	inline std::vector<HTTPResponse>::iterator begin() { return m_aResults.begin(); }
	inline std::vector<HTTPResponse>::iterator end() { return m_aResults.end(); }
	
	bool hasResults() { return !m_aResults.empty(); }
	
	std::string			m_description;
	int					m_step;
	std::vector<HTTPResponse> m_aResults;
};

class LoadTestResultsSaver : public Thread
{
public:
	LoadTestResultsSaver(bool continualSaving, const std::string &filePath);
	LoadTestResultsSaver(bool continualSaving, SQLiteDB *pDB);
	virtual ~LoadTestResultsSaver() { }
	
	bool initStorage();
	bool initDatabase();
	
	void copyScript(const Script &script) { m_script = script; m_haveScript = true; }
	
	virtual void run();
	void stop();
	
	void addResult(const ScriptResult &result);
	
	void storeResults();
	
protected:
	void databaseStore();
	void fileStore();

protected:
	Script							m_script;
	bool							m_haveScript;
	std::vector<ScriptResult>		m_aResults;
	Mutex							m_lock;

	bool							m_continualSaving;

	bool							m_database;

	std::string						m_filePath;	

	SQLiteDB *						m_pMainDB;

	// used to store the result ID row of the DB table, when storing continually
	unsigned long					m_loadTestRunID;
};

#endif
