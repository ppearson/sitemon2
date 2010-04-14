#ifndef RESULTS_STORAGE_H
#define RESULTS_STORAGE_H

#include <vector>
#include <map>

#include "../http_response.h"

class ConcurrentHitResults
{
public:
	ConcurrentHitResults() { }
	
	void addResult(int step, HTTPResponse &response);
	void addResults(std::vector<HTTPResponse> &results);
	
	bool outputResultsToCSV(std::string path);
	
protected:
	std::map<int, std::vector<HTTPResponse> > m_aResults;	
};

#endif