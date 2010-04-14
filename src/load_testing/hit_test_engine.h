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

#ifndef HIT_TEST_ENGINE_H
#define HIT_TEST_ENGINE_H

#include <openssl/ssl.h>
#include <openssl/crypto.h>

#include <vector>
#include <set>

#include "../http_request.h"
#include "../http_response.h"
#include "../script.h"
#include "results_storage.h"

#ifdef _MSC_VER

static HANDLE *lock_cs;

void win32_locking_callback(int mode, int type, char *file, int line);
void thread_setup();
void thread_cleanup();
static unsigned long id_function(void);

#endif

enum HitTestType
{
	HIT_SCRIPT,
	HIT_REQUEST
};

class HitTestEngine
{
public:
	HitTestEngine();
	
	bool initialise(Script &script, int numberOfThreads, int repeats = 0);
	bool initialise(HTTPRequest &request, int numberOfThreads, int repeats = 0);
	
	bool start();
	
	ConcurrentHitResults &getResults() { return m_results; }
	
protected:
	HitTestType		m_hitTestType;
	Script *		m_pScript;
	HTTPRequest *	m_pRequest;
	
	int		m_numberOfThreads;
	int		m_repeats;
	
	ConcurrentHitResults	m_results;
};

#endif
