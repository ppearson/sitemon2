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

#ifndef PROFILE_TEST_ENGINE_H
#define PROFILE_TEST_ENGINE_H

#include <vector>
#include <set>
#include <bitset>

#include "../utils/thread.h"

#include "../http_request.h"
#include "../http_response.h"
#include "../script.h"

#include "load_test_results_saver.h"

#include "profile_load_request_thread.h"

#ifdef _MSC_VER

static HANDLE *lock_cs1;

static void win32_locking_callback(int mode, int type, char *file, int line);
static void thread_setup();
static void thread_cleanup();
static unsigned long id_function(void);

#endif

enum ProfileTestType
{
	PROFILE_SCRIPT,
	PROFILE_REQUEST
};

enum ProfileTestStyle
{
	PROFILE_STEP,
	PROFILE_SMOOTH
};

class ProfileSegment
{
public:
	ProfileSegment(int concurrentRequests, int duration);
	
	int m_duration;
	int m_concurrentRequests;
};

class ProfileTestEngine
{
public:
	ProfileTestEngine(bool smooth = false);
	ProfileTestEngine(int concurrentRequests, int duration, bool debugging);
	~ProfileTestEngine();
	
	void setResultsSaver(LoadTestResultsSaver *pSaver) { m_pSaver = pSaver; }
	
	bool initialise(Script &script);
	bool initialise(HTTPRequest &request, int sleep = 2);
	void addProfileSegment(int concurrentRequests, int duration);
	
	bool start();	
	
protected:
	void setupTimeframeCounts();	
	
protected:
	ProfileTestType		m_testType;
	ProfileTestStyle	m_testStyle;
	
	int					m_maxNumberOfThreads;
	int					m_runLength;
	
	int					m_numberOfActiveThreads;
	
	std::vector<int>	m_aTimeframeCounts;
	
	std::vector<ProfileLoadRequestThread*> m_aThreads;
	
	std::vector<ProfileSegment>	m_aProfileSegments;
	
	Script *		m_pScript;
	HTTPRequest *	m_pRequest;
	int				m_sleep;
	
	bool			m_isScript;
	bool			m_debugging;
	
	LoadTestResultsSaver *	m_pSaver;
	
};


#endif


