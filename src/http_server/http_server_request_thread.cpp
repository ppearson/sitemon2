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

#include "http_server_request_thread.h"

#include "http_server_request.h"
#include "http_server_responses.h"

#include "../http_engine.h"

#include "http_server_db_helpers.h"
#include "http_server_html_formatters.h"

HTTPServerRequestThread::HTTPServerRequestThread(Socket *socket, const std::string &webContentPath, SQLiteDB *pMainDB)
	: m_pSocket(socket), m_webContentPath(webContentPath), m_pMainDB(pMainDB)
{

}

HTTPServerRequestThread::~HTTPServerRequestThread()
{
}

void HTTPServerRequestThread::run()
{
	if (!m_pSocket)
		return;
	
	std::string clientAddress = m_pSocket->getClientAddress();
	
	std::string recvData;
	m_pSocket->recv(recvData);
	
	HTTPServerRequest request(recvData);
	std::string response;
	
	if (request.parse())
	{
		if (request.getPath() == "/inline_simple" && request.hasParams())
		{
			std::string url = request.getParam("url");
			std::string acceptCompressed = request.getParam("accept_compressed");
			
			std::string content;
			
			HTTPEngine engine;
			HTTPRequest httpTestRequest(url);
			if (acceptCompressed == "1")
			{
				httpTestRequest.setAcceptCompressed(true);
			}
			HTTPResponse httpTestResponse;

			if (engine.performRequest(httpTestRequest, httpTestResponse))
			{
				formatResponseToHTMLDL(httpTestResponse, content);
			}
			else
			{
				content += "Couldn't perform test: " + httpTestResponse.errorString + "<br>\n";				
			}

			addResponseToSingleTestHistoryTable(m_pMainDB, httpTestResponse);
			
			HTTPServerResponse resp(200, content);
			response = resp.responseString();
		}
		else if (request.getPath() == "/history")
		{
			int offset = 0;
			if (request.hasParams())
			{
				offset = atoi(request.getParam("start").c_str());
			}
			
			std::string filePath = m_webContentPath + "history.tplt";
			
			std::string dataContent;
			getSingleTestHistoryList(m_pMainDB, dataContent, offset);
			
			HTTPServerTemplateFileResponse resp(filePath, dataContent);
			response = resp.responseString();
		}
		else if (request.getPath() == "/monitoring")
		{
			std::string filePath = m_webContentPath + "monitoring.tplt";
			
			std::string singleTests;
			getSingleScheduledTestsList(m_pMainDB, singleTests);
			
			std::string scriptTests;
			getScriptScheduledTestsList(m_pMainDB, scriptTests);
			
			HTTPServerTemplateFileResponse resp(filePath, singleTests, scriptTests);
			response = resp.responseString();
		}
		else if (request.getPath() == "/add_single_test")
		{
			if (request.isPost()) // actual submission
			{
				std::string thisResponse;
				if (addSingleScheduledTest(m_pMainDB, request, thisResponse))
				{
					// okay

					HTTPServerRedirectResponse resp("/monitoring");
					response = resp.responseString();
				}
				else
				{
					HTTPServerResponse resp(500, thisResponse);
					response = resp.responseString();
				}
			}
			else // otherwise, serve up the entry form
			{
				std::string templatePath = m_webContentPath + "form.tplt";
				std::string title = "Add Single Test";
				std::string formContent;
				generateAddSingleScheduledTestForm(formContent);
				
				HTTPServerTemplateFileResponse resp(templatePath, title, formContent);
				response = resp.responseString();
			}
		}
		else if (request.getPath() == "/add_script_test")
		{
			if (request.isPost()) // actual submission
			{
				std::string thisResponse;
				if (addScriptScheduledTest(m_pMainDB, request, thisResponse))
				{
					// okay
					
					HTTPServerRedirectResponse resp("/monitoring");
					response = resp.responseString();
				}
				else
				{
					HTTPServerResponse resp(500, thisResponse);
					response = resp.responseString();
				}
			}
			else // otherwise, serve up the entry form
			{
				std::string templatePath = m_webContentPath + "form.tplt";
				std::string title = "Add Script Test";
				std::string formContent;
				generateAddScriptScheduledTestForm(formContent);
				
				HTTPServerTemplateFileResponse resp(templatePath, title, formContent);
				response = resp.responseString();
			}
		}
		else if (request.getPath() == "/edit_monitor_test")
		{
			if (request.isPost()) // actual submission
			{
				std::string thisResponse;
				if (editSingleScheduledTest(m_pMainDB, request, thisResponse))
				{
					// okay
					
					HTTPServerRedirectResponse resp("/monitoring");
					response = resp.responseString();
				}
				else
				{
					HTTPServerResponse resp(500, thisResponse);
					response = resp.responseString();
				}
			}
			else // otherwise, serve up the edit form
			{
				std::string templatePath = m_webContentPath + "form.tplt";
				std::string title = "Edit Single Test";
				std::string formContent;
				
				long testID = atoi(request.getParam("test_id").c_str());
				
				generateEditSingleScheduledTestForm(m_pMainDB, testID, formContent);
				
				HTTPServerTemplateFileResponse resp(templatePath, title, formContent);
				response = resp.responseString();
			}
		}
		else if (request.getPath() == "/edit_script_test")
		{
			if (request.isPost()) // actual submission
			{
				std::string thisResponse;
				if (editScriptScheduledTest(m_pMainDB, request, thisResponse))
				{
					// okay
					
					HTTPServerRedirectResponse resp("/monitoring");
					response = resp.responseString();
				}
				else
				{
					HTTPServerResponse resp(500, thisResponse);
					response = resp.responseString();
				}
			}
			else // otherwise, serve up the edit form
			{
				std::string templatePath = m_webContentPath + "edit_script_test.tplt";
				std::string title = "Edit Script Test";
				std::string formContent;
				std::string tableContent;
				std::string addNewPageLink;
				
				long testID = atoi(request.getParam("test_id").c_str());
				
				generateEditScriptScheduledTestForm(m_pMainDB, testID, formContent, tableContent, addNewPageLink);
				
				HTTPServerTemplateFileResponse resp(templatePath, formContent, tableContent, addNewPageLink);
				response = resp.responseString();
			}
		}
		else if (request.getPath() == "/edit_script_page")
		{
			if (request.isPost()) // actual submission
			{
				std::string thisResponse;
				if (editScriptScheduledTestPage(m_pMainDB, request, thisResponse))
				{
					// okay
					
					HTTPServerRedirectResponse resp("/monitoring");
					response = resp.responseString();
				}
				else
				{
					HTTPServerResponse resp(500, thisResponse);
					response = resp.responseString();
				}
			}
			else // otherwise, serve up the edit form
			{
				std::string templatePath = m_webContentPath + "form.tplt";
				std::string title = "Edit Script Page";
				std::string formContent;
							
				long testID = atoi(request.getParam("page_id").c_str());
				
				generateEditScriptScheduledTestPageForm(m_pMainDB, testID, formContent);
				
				HTTPServerTemplateFileResponse resp(templatePath, title, formContent);
				response = resp.responseString();
			}
		}
		else if (request.getPath() == "/add_script_page")
		{
			if (request.isPost()) // actual submission
			{
				std::string thisResponse;
				if (addScriptScheduledTestPage(m_pMainDB, request, thisResponse))
				{
					// okay
					
					HTTPServerRedirectResponse resp("/monitoring");
					response = resp.responseString();
				}
				else
				{
					HTTPServerResponse resp(500, thisResponse);
					response = resp.responseString();
				}
			}
			else // otherwise, serve up the edit form
			{
				std::string templatePath = m_webContentPath + "form.tplt";
				std::string title = "Add Script Page";
				std::string formContent;
				
				long scriptID = atoi(request.getParam("script_id").c_str());
				
				generateAddScriptScheduledTestPageForm(m_pMainDB, scriptID, formContent);
				
				HTTPServerTemplateFileResponse resp(templatePath, title, formContent);
				response = resp.responseString();
			}
		}
		else if (request.getPath() == "/view_monitortest")
		{
			std::string filePath = m_webContentPath + "view_monitortest.tplt";

			long testID = 0;
			std::string strTestID = request.getParam("testid");
			if (!strTestID.empty())
				testID = atoi(strTestID.c_str());
			
			std::string description;			
			std::string dataContent;
			getSingleScheduledTestResultsList(m_pMainDB, testID, description, dataContent);
			
			HTTPServerTemplateFileResponse resp(filePath, description, dataContent);
			response = resp.responseString();
		}
		else if (request.getPath() == "/single_details" && request.hasParams())
		{
			long runID = atoi(request.getParam("runid").c_str());
			
			std::string filePath = m_webContentPath + "single_details.tplt";
			
			std::string dataContent;
			formatDBSingleTestResponseToHTMLDL(m_pMainDB, runID, dataContent);
			
			HTTPServerTemplateFileResponse resp(filePath, dataContent);
			response = resp.responseString();
		}
		else if (request.getPath() == "/single_components" && request.hasParams())
		{
			long runID = atoi(request.getParam("run_id").c_str());
			long testID = atoi(request.getParam("test_id").c_str());
			
			std::string filePath = m_webContentPath + "single_components.tplt";
			
			std::string dataContent;
			getSingleScheduledTestComponentsList(m_pMainDB, testID, runID, dataContent);
			
			HTTPServerTemplateFileResponse resp(filePath, dataContent);
			response = resp.responseString();			
		}
		else
		{
			std::string requestedPath = request.getPath();
			if (requestedPath.size() > 1 && requestedPath.substr(0, 1) == "/") // an actual relative path was specified
			{
				requestedPath = requestedPath.substr(1); // knock off the leading slash
			}
			else if (requestedPath.size() == 1 && requestedPath == "/")
			{
				// default is index.html
				requestedPath = "index.html";
			}
			
			if (m_webContentPath.empty())
			{
				std::string content = "<html>\n<head><title>Sitemon Web Interface</title></head>\n<body>\n";
				content += "<h3>Sitemon Web Interface</h3>\nError: Web Content Path not configured.\n</body>\n</html>\n";
				
				HTTPServerResponse resp(500, content);
				response = resp.responseString();
			}
			else
			{
				if (requestedPath.find("..") != -1) // try and guard against obvious exploits
				{
					HTTPServerResponse resp1(500, "<h3>Error occured.</h3>");
					
					response = resp1.responseString();
				}
				else
				{
					std::string filePath = m_webContentPath + requestedPath;
			
					HTTPServerFileResponse resp(filePath);
					response = resp.responseString();
				}
			}
		}
	}
	else
	{
		HTTPServerResponse resp(500, "<html>\n<body>\nCouldn't understand request\n</body>\n</html>");
		response = resp.responseString();
	}
	
	m_pSocket->send(response);
	
	m_pSocket->close();
	
	delete m_pSocket;
}
