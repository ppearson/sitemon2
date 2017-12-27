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

#ifndef HIT_LOAD_REQUEST_THREAD_H
#define HIT_LOAD_REQUEST_THREAD_H

#include "../utils/thread.h"

#include "../script.h"
#include "../http_engine.h"

#include "load_test_results_saver.h"

struct RequestThreadData
{
	RequestThreadData(int thread, Script *pScript, LoadTestResultsSaver *pSaver = NULL, int repeats = 0) : m_thread(thread), m_pScript(pScript), m_repeats(repeats),
						m_pSaver(pSaver), m_debugging(false) { }
	
	bool m_debugging;
	
	Script *m_pScript;
	int m_thread;
	int m_repeats;
	
	LoadTestResultsSaver * m_pSaver;
};

class HitLoadRequestThread : public Thread
{
public:
	HitLoadRequestThread(RequestThreadData *data);
	virtual ~HitLoadRequestThread();

	virtual void run();
	
protected:
	int m_threadID;
	int m_repeats;
	Script m_Script;
	
	bool m_debugging;
	
	LoadTestResultsSaver * m_pSaver;
};

#endif
