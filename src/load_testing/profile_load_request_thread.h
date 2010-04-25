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

#ifndef PROFILE_LOAD_REQUEST_THREAD_H
#define PROFILE_LOAD_REQUEST_THREAD_H

#include "../utils/thread.h"

#include "../script.h"
#include "../http_engine.h"

#include "../script_result.h"

#include "load_test_results_saver.h"

struct ProfileThreadData
{
	ProfileThreadData(int threadID, Script *pScript, LoadTestResultsSaver *pSaver = NULL, bool debugging = false) : m_thread(threadID),
						m_pScript(pScript), m_pSaver(pSaver), m_debugging(debugging) { }
	
	bool m_debugging;
	
	Script *m_pScript;
	int m_thread;
	
	LoadTestResultsSaver * m_pSaver;
};

// needs to have a callback to the ProfileTestEngine to hand results back

class ProfileLoadRequestThread : public Thread
{
public:
	ProfileLoadRequestThread(ProfileThreadData *data);
	virtual ~ProfileLoadRequestThread();
	
	virtual void run();
	
	void shutdownThreadAndKill();
	
	void tellThreadToStop();
	
	
protected:
	int m_threadID;
	Script m_Script;
	bool m_debugging;
	
	bool m_active;
	
	ScriptResult	m_pScriptResult;
	
	LoadTestResultsSaver * m_pSaver;
};



#endif
