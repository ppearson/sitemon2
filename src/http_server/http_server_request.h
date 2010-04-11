
#ifndef HTTP_SERVER_REQUEST_H
#define HTTP_SERVER_REQUEST_H

#ifdef _MSC_VER
#pragma warning (disable : 4786)
#endif

#include <string>
#include <map>
#include <vector>

class HTTPServerRequest
{
public:
	HTTPServerRequest(const std::string &request);
	
	bool parse();
	
	const std::string &getPath() const { return m_path; }
	const std::map<std::string, std::string> &getParams() const { return m_aParams; }
	
	bool isPost() const { return  m_post; }
	bool hasParams() const { return !m_aParams.empty(); }
	std::string getParam(const std::string &name) { return m_aParams[name]; }
	
protected:
	void addParams(const std::string &params);
	
protected:
	std::string		m_request;
	
	std::string		m_path;
	bool			m_post;
	std::map<std::string, std::string> m_aParams;	
	
};

#endif
