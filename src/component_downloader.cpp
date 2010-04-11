
#include "component_downloader.h"

ComponentTask::ComponentTask(const std::string &url) : m_url(url)
{

}

ComponentDownloader::ComponentDownloader(CURL *mainCURLHandle, HTTPResponse &response, bool acceptCompressed) : ThreadPool(),
											m_response(response), m_acceptCompressed(acceptCompressed)
{
	// needed to share cookie info between threads - we need to do this because some content management systems with
	// personalisation (I'm looking at you Fatwire) require cookies in order to respond correctly with content to requests.
	// However, only the thread which did the initial request for the HTML got the cookie back, hence the need to share it
	
	m_CURLSharedData = curl_share_init();
	curl_share_setopt(m_CURLSharedData, CURLSHOPT_LOCKFUNC, share_lock);
	curl_share_setopt(m_CURLSharedData, CURLSHOPT_USERDATA, &m_CURLSharedData);
	curl_share_setopt(m_CURLSharedData, CURLSHOPT_UNLOCKFUNC, share_unlock);
	curl_share_setopt(m_CURLSharedData, CURLSHOPT_SHARE, CURL_LOCK_DATA_COOKIE);
	
	// reuse the original curl handle for the first thread, and init second one
	m_aCURLHandles[0] = mainCURLHandle;
	m_aCURLHandles[1] = curl_easy_init();	
	
	for (int i = 0; i < 2; i++)
	{
		curl_easy_setopt(m_aCURLHandles[i], CURLOPT_SHARE, m_CURLSharedData);
	}
}

ComponentDownloader::~ComponentDownloader()
{
	// Main handle will be cleaned up by HTTPEngine
	for (int i = 1; i < 2; i++)
	{
		if (m_aCURLHandles[i])
		{
			curl_easy_cleanup(m_aCURLHandles[i]);
		}
	}
	
	curl_share_cleanup(m_CURLSharedData);
}

void ComponentDownloader::addURL(const std::string &url)
{
	ComponentTask *pNewTask = new ComponentTask(url);
	
	addTask(pNewTask);	
}

bool ComponentDownloader::downloadComponents()
{
	startPoolAndWaitForCompletion();
	
	return true;
}

void ComponentDownloader::doTask(Task *pTask, int threadID)
{
	HTTPComponentResponse newResponse;
	
	ComponentTask *pThisTask = static_cast<ComponentTask*>(pTask);
	CURL *pThisHandle = m_aCURLHandles[threadID];
	
	if (curl_easy_setopt(pThisHandle, CURLOPT_USERAGENT, "Mozilla/5.0") != 0)
		return;
	
	if (curl_easy_setopt(pThisHandle, CURLOPT_URL, pThisTask->getURL().c_str()) != 0)
		return;
	
	if (m_acceptCompressed)
	{
		if (curl_easy_setopt(pThisHandle, CURLOPT_ENCODING, "gzip, deflate") != 0)
			return;
	}
	
	if (curl_easy_setopt(pThisHandle, CURLOPT_WRITEFUNCTION, writeBodyData) != 0)
		return;
	
	if (curl_easy_setopt(pThisHandle, CURLOPT_WRITEDATA, (void *)&newResponse) != 0)
		return;
	
	newResponse.requestedURL = pThisTask->getURL();
	
	curl_easy_setopt(pThisHandle, CURLOPT_NOSIGNAL, 1L);
	
	int res = curl_easy_perform(pThisHandle);
	
	if (res != 0)
	{
		switch (res)
		{
			case CURLE_COULDNT_RESOLVE_HOST:
				newResponse.errorCode = HTTP_COULDNT_RESOLVE_HOST;
				newResponse.errorString = "Couldn't resolve host.";
				break;
			case CURLE_COULDNT_CONNECT:
				newResponse.errorCode = HTTP_COULDNT_CONNECT;
				newResponse.errorString = "Couldn't connect to host.";
				break;
			case CURLE_OPERATION_TIMEOUTED:
				newResponse.errorCode = HTTP_TIMEOUT;
				newResponse.errorString = "Connection timed out.";
				break;
			case CURLE_RECV_ERROR:
				newResponse.errorCode = HTTP_RECV_ERROR;
				newResponse.errorString = "Error receiving data.";
				break;
			default:
				newResponse.errorCode = HTTP_UNKNOWN_ERROR;
				newResponse.errorString = "Unknown error.";
				break;
		}
	}
	else
	{
		extractResponseFromCURLHandle(pThisHandle, newResponse);
	}
	
//	printf("T: %i, Res: %ld for: %s\n", threadID, newResponse.responseCode, pThisTask->getURL().c_str());
	
	m_response.addComponent(newResponse);	
}

bool ComponentDownloader::extractResponseFromCURLHandle(CURL *handle, HTTPComponentResponse &response)
{
	char *actual_url = 0;
	char *content_type = 0;
	double download = 0.0;
	
	curl_easy_getinfo(handle, CURLINFO_RESPONSE_CODE, &response.responseCode);
	
	curl_easy_getinfo(handle, CURLINFO_TOTAL_TIME, &response.totalTime);
	curl_easy_getinfo(handle, CURLINFO_NAMELOOKUP_TIME, &response.lookupTime);
	curl_easy_getinfo(handle, CURLINFO_CONNECT_TIME, &response.connectTime);
	curl_easy_getinfo(handle, CURLINFO_STARTTRANSFER_TIME, &response.dataStartTime);
	
//	curl_easy_getinfo(handle, CURLINFO_REDIRECT_COUNT, &response.redirectCount);
//	curl_easy_getinfo(handle, CURLINFO_REDIRECT_TIME, &response.redirectTime);
	
	curl_easy_getinfo(handle, CURLINFO_SIZE_DOWNLOAD, &download);
	response.downloadSize = (long)download;
	
	curl_easy_getinfo(handle, CURLINFO_CONTENT_TYPE, &content_type);
	response.contentType.assign(content_type);
	
	curl_easy_getinfo(handle, CURLINFO_EFFECTIVE_URL, &actual_url);
	response.finalURL.assign(actual_url);
	
	return true;
}

static size_t writeBodyData(void *buffer, size_t size, size_t nmemb, void *userp)
{
	if (!userp)
		return 0;
	
	size_t full_size = size * nmemb;
	
	HTTPComponentResponse *response = static_cast<HTTPComponentResponse *>(userp);
	
	response->contentSize += full_size;
	
	return full_size;
}

void share_lock(CURL *handle, curl_lock_data data, curl_lock_access locktype, void *userptr)
{
	
}

void share_unlock(CURL *handle, curl_lock_data data, curl_lock_access locktype, void *userptr)
{
	
}
