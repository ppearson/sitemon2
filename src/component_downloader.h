#ifndef CONTENT_DOWNLOADER_H
#define CONTENT_DOWNLOADER_H

#include "http_engine.h"
#include "utils/thread_pool.h"

class ComponentTask : public Task
{
public:
	ComponentTask(const std::string &url);
	virtual ~ComponentTask() { }
	
	std::string &	getURL() { return m_url; }
	
protected:
	std::string		m_url;	
};

static size_t writeBodyData(void *buffer, size_t size, size_t nmemb, void *userp);
static void share_lock(CURL *handle, curl_lock_data data, curl_lock_access locktype, void *userptr);
static void share_unlock(CURL *handle, curl_lock_data data, curl_lock_access locktype, void *userptr);

class ComponentDownloader : public ThreadPool
{
public:
	ComponentDownloader(CURL *mainCURLHandle, HTTPResponse &response, bool acceptCompressed = false);
	virtual ~ComponentDownloader();
	
	void addURL(const std::string &url);
	bool downloadComponents();
	
	virtual void doTask(Task *pTask, int threadID);
	
	bool extractResponseFromCURLHandle(CURL *handle, HTTPComponentResponse &response);
	
protected:
	CURLSH *m_CURLSharedData;
	Mutex	m_sharedLock;
	CURL *m_aCURLHandles[2];
	HTTPResponse &	m_response;
	
	bool	m_acceptCompressed;
};

#endif
