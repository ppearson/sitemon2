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

#ifndef CONTENT_DOWNLOADER_H
#define CONTENT_DOWNLOADER_H

#include "http_engine.h"
#include "utils/thread_pool.h"

class ComponentTask : public Task
{
public:
	ComponentTask(const std::string &url, const std::string &referrer = "");
	virtual ~ComponentTask() { }
	
	std::string &	getURL() { return m_url; }
	std::string &	getReferrer() { return m_referrer; }
	
protected:
	std::string		m_url;
	std::string		m_referrer;
};

static size_t writeBodyData(void *buffer, size_t size, size_t nmemb, void *userp);
static void share_lock(CURL *handle, curl_lock_data data, curl_lock_access locktype, void *userptr);
static void share_unlock(CURL *handle, curl_lock_data data, curl_lock_access locktype, void *userptr);
struct cdLocks
{
	Mutex	shareLock;
	Mutex	cookieLock;
};

class ComponentDownloader : public ThreadPool
{
public:
	ComponentDownloader(CURL *mainCURLHandle, const std::string &userAgent, HTTPResponse &response, bool acceptCompressed = false);
	virtual ~ComponentDownloader();
	
	void addURL(const std::string &url);
	bool downloadComponents();
	
	virtual void doTask(Task *pTask, int threadID);
	
	static bool extractResponseFromCURLHandle(CURL *handle, HTTPComponentResponse &response);

protected:
	CURLSH*		m_CURLSharedData;
	cdLocks		m_locks;

	CURL*		m_aCURLHandles[2];


	Mutex		m_componentLock;

	HTTPResponse&	m_response;
	bool		m_acceptCompressed;
	
	std::string	m_userAgent;
};

#endif
