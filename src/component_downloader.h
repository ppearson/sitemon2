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

struct cdLocks
{
	Mutex	shareLock;
	Mutex	cookieLock;
};

class ComponentDownloader : public ThreadPool
{
public:
	ComponentDownloader(CURL *mainCURLHandle, HTTPResponse &response, bool acceptCompressed = false);
	virtual ~ComponentDownloader();
	
	void addURL(const std::string &url);
	bool downloadComponents();
	
	virtual void doTask(Task *pTask, int threadID);
	
	bool extractResponseFromCURLHandle(CURL *handle, HTTPComponentResponse &response);

	cdLocks m_locks;

protected:
	static void share_lock(CURL *handle, curl_lock_data data, curl_lock_access locktype, void *userptr);
	static void share_unlock(CURL *handle, curl_lock_data data, curl_lock_access locktype, void *userptr);
	
protected:
	CURLSH *m_CURLSharedData;
	cdLocks	m_lock;
	CURL *m_aCURLHandles[2];
	HTTPResponse &	m_response;
	
	bool	m_acceptCompressed;
};

#endif
