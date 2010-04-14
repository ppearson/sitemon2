#include "http_engine.h"
#include "sitemon.h"
#include "load_testing/hit_test_engine.h"
#include "load_testing/results_storage.h"
#include "html_parser.h"

bool performSingleRequest(HTTPRequest &request, bool outputHeader)
{
	HTTPEngine engine;
	HTTPResponse response;

	if (engine.performRequest(request, response))
	{
		if (outputHeader)
		{
			std::cout << response.header << "\n";
		}

		outputResponse(request, response);		
	}
	else
	{
		std::cout << response.errorString << "\n";

		return false;
	}

	return true;
}

bool performScriptRequest(Script &script)
{
	HTTPEngine engine;

	for (std::vector<HTTPRequest>::iterator it = script.begin(); it != script.end(); ++it)
	{
		HTTPRequest &request = *it;
		HTTPResponse response;

		std::cout << "Testing... " << request.getDescription() << "\n";

		if (engine.performRequest(request, response))
		{
			outputResponse(request, response);
		}
		else
		{
			std::cout << response.errorString << "\n";
		}
	}

	return true;
}

bool performConcurrentScriptRequest(Script &script, int threads, const std::string &outputPath)
{
	HitTestEngine engine;
	engine.initialise(script, threads);
	
	if (engine.start())
	{
		if (!outputPath.empty())
		{
			ConcurrentHitResults &results = engine.getResults();
			
			results.outputResultsToCSV(outputPath);
		}
	}
	
	return true;
}

void outputResponse(HTTPRequest &request, HTTPResponse &response)
{
	std::cout << "Final URL:\t\t" << response.finalURL << "\n";
	std::cout << "Respone code:\t\t" << response.responseCode << "\n\n";

	std::cout << "DNS Lookup:\t\t" << response.lookupTime << " seconds.\n";
	std::cout << "Connection:\t\t" << response.connectTime << " seconds.\n";
	std::cout << "Data start:\t\t" << response.dataStartTime << " seconds.\n";

	if (response.redirectCount)
	{
		std::cout << "Redirect count:\t\t" << response.redirectCount << ".\n";
		std::cout << "Redirect time:\t\t" << response.redirectTime << " seconds.\n";
	}

	std::cout << "Total time:\t\t" << response.totalTime << " seconds.\n\n";
	
	std::cout << "HTML Content size:\t" << response.contentSize << "\n";
	std::cout << "HTML Download size:\t" << response.downloadSize << "\n";

	if (response.contentSize > response.downloadSize)
	{
		int compression = 100 - (int)(((double)response.downloadSize / response.contentSize) * 100.0);

		std::cout << "Compression Savings:\t" << compression << "%\n";
		std::cout << "Content Encoding:\t" << response.contentEncoding << "\n";
	}
	
	if (request.getDownloadContent())
	{	
		std::cout << "Total Content size:\t" << response.totalContentSize << "\n";
		std::cout << "Total Download size:\t" << response.totalDownloadSize << "\n";
		
		if (response.componentProblem)
		{
			std::cout << "Issues downloading one or more components:\n";
			
			const std::vector<HTTPComponentResponse> &components = response.getComponents();
			
			std::vector<HTTPComponentResponse>::const_iterator it = components.begin();
			for (; it != components.end(); ++it)
			{
				const HTTPComponentResponse &compResponse = *it;
				
				if (compResponse.errorCode != HTTP_OK || compResponse.responseCode != 200)
				{
					printf("%i\t%ld\t%s\n", compResponse.errorCode, compResponse.responseCode, compResponse.requestedURL.c_str());					
				}				
			}
			
			
		}
	}
}

#ifdef _MSC_VER

void win32_locking_callback(int mode, int type, char *file, int line)
{
	if (mode & CRYPTO_LOCK)
	{
		WaitForSingleObject(lock_cs[type], INFINITE);
	}
	else
	{
		ReleaseMutex(lock_cs[type]);
	}
}

void thread_setup()
{
	int i;

	lock_cs = (void**)OPENSSL_malloc(CRYPTO_num_locks() * sizeof(HANDLE));
	for (i = 0; i < CRYPTO_num_locks(); i++)
	{
		lock_cs[i] = CreateMutex(NULL, FALSE, NULL);
	}

	CRYPTO_set_locking_callback((void (*)(int, int, const char *, int))win32_locking_callback);
}

void thread_cleanup()
{
	int i;

	CRYPTO_set_locking_callback(NULL);
	for (i = 0; i < CRYPTO_num_locks(); i++)
		CloseHandle(lock_cs[i]);

	OPENSSL_free(lock_cs);
}

static unsigned long id_function(void)
{
  return ((unsigned long)GetCurrentThreadId());
}

#endif