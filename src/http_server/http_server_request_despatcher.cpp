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

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "http_server_request_despatcher.h"

#include "../http_engine.h"

#include "http_server.h"

#include "http_server_db_helpers.h"
#include "http_server_html_formatters.h"
#include "http_server_load_testing_helpers.h"

HTTPServerRequestDespatcher::HTTPServerRequestDespatcher(HTTPServer& httpServer, SQLiteDB* pMonitoringDB, SQLiteDB* pLoadTestingDB) :
								m_server(httpServer),
								m_pMonitoringDB(pMonitoringDB), m_pLoadTestingDB(pLoadTestingDB), m_pResultsSaver(NULL)
{
	m_webContentPath = httpServer.getWebContentPath();
	m_authenticationType = httpServer.getAuthenticationType();
}

void HTTPServerRequestDespatcher::registerMappings()
{
	m_requestMappings["/inline_simple"] = &HTTPServerRequestDespatcher::inlineSimple;
	m_requestMappings["/history"] = &HTTPServerRequestDespatcher::history;
	m_requestMappings["/monitoring"] = &HTTPServerRequestDespatcher::monitoring;
	m_requestMappings["/add_single_test"] = &HTTPServerRequestDespatcher::addSingleTest;
	m_requestMappings["/add_script_test"] = &HTTPServerRequestDespatcher::addScriptTest;
	m_requestMappings["/edit_monitor_test"] = &HTTPServerRequestDespatcher::editMonitorTest;
	m_requestMappings["/edit_script_test"] = &HTTPServerRequestDespatcher::editScriptTest;
	m_requestMappings["/add_script_page"] = &HTTPServerRequestDespatcher::addScriptPage;
	m_requestMappings["/edit_script_page"] = &HTTPServerRequestDespatcher::editScriptPage;
	m_requestMappings["/view_single_test"] = &HTTPServerRequestDespatcher::viewSingleTest;
	m_requestMappings["/single_details"] = &HTTPServerRequestDespatcher::singleDetails;
	m_requestMappings["/single_components"] = &HTTPServerRequestDespatcher::singleComponents;
	m_requestMappings["/view_script_test"] = &HTTPServerRequestDespatcher::viewScriptTest;
	m_requestMappings["/script_details"] = &HTTPServerRequestDespatcher::scriptDetails;

	m_requestMappings["/run_man_single"] = &HTTPServerRequestDespatcher::runManualSingleTest;
	m_requestMappings["/run_man_script"] = &HTTPServerRequestDespatcher::runManualScriptTest;

	m_requestMappings["/delete_single_test"] = &HTTPServerRequestDespatcher::deleteSingleTest;
	m_requestMappings["/delete_script_test"] = &HTTPServerRequestDespatcher::deleteScriptTest;
	m_requestMappings["/delete_script_step"] = &HTTPServerRequestDespatcher::deleteScriptStep;

	m_requestMappings["/load_testing"] = &HTTPServerRequestDespatcher::loadTesting;
	m_requestMappings["/load_test_results"] = &HTTPServerRequestDespatcher::loadTestingRunResults;
}

void HTTPServerRequestDespatcher::handleRequest(HTTPServerRequest &request, std::string &response)
{
	if (m_authenticationType != eHTTPAuthNone)
	{
		if (!request.hasAuthenticationHeader())
		{
			HTTPServerAuthenticationResponse resp;
			response = resp.responseString();
			return;
		}
		
		// otherwise, see if the authentication is valid
		if (!request.isAcceptedAuthenticationHeader())
		{
			// TODO: do this properly
			HTTPServerAuthenticationResponse resp;
			response = resp.responseString();
			return;
		}
		
		// now check username and password
		const std::string& authUsername = request.getAuthUsername();
		const std::string& authPassword = request.getAuthPassword();
		
		if (!m_server.areAuthCredentialsValid(authUsername, authPassword))
		{
			// TODO: again, do this properly, maybe with some counter to delay responses...
			HTTPServerAuthenticationResponse resp;
			response = resp.responseString();
			return;
		}
	}
	
	std::map<std::string, MFP>::iterator itFind = m_requestMappings.find(request.getPath());

	if (itFind != m_requestMappings.end())
	{
		MFP fp = (*itFind).second;

		(this->*fp)(request, response);
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

void HTTPServerRequestDespatcher::inlineSimple(HTTPServerRequest &request, std::string &response)
{
	if (request.hasParams())
	{
		std::string url = request.getParam("url");
		std::string acceptCompressed = request.getParam("accept_compressed");
		std::string downloadContent = request.getParam("download_content");

		std::string content;

		HTTPEngine engine;
		HTTPRequest httpTestRequest(url);
		if (acceptCompressed == "1")
		{
			httpTestRequest.setAcceptCompressed(true);
		}
		if (downloadContent == "1")
		{
			httpTestRequest.setDownloadContent(true);
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

		addResponseToSingleTestHistoryTable(m_pMonitoringDB, httpTestResponse);

		HTTPServerResponse resp(200, content);
		response = resp.responseString();
	}
}

void HTTPServerRequestDespatcher::history(HTTPServerRequest &request, std::string &response)
{
	int offset = 0;
	if (request.hasParams())
	{
		offset = atoi(request.getParam("start").c_str());
	}

	std::string filePath = m_webContentPath + "history.tplt";

	std::string dataContent;
	getSingleTestHistoryList(m_pMonitoringDB, dataContent, offset);

	HTTPServerTemplateFileResponse resp(filePath, dataContent);
	response = resp.responseString();
}

void HTTPServerRequestDespatcher::monitoring(HTTPServerRequest &request, std::string &response)
{
	std::string filePath = m_webContentPath + "monitoring.tplt";

	std::string singleTests;
	getSingleScheduledTestsList(m_pMonitoringDB, singleTests);

	std::string scriptTests;
	getScriptScheduledTestsList(m_pMonitoringDB, scriptTests);

	HTTPServerTemplateFileResponse resp(filePath, singleTests, scriptTests);
	response = resp.responseString();
}

void HTTPServerRequestDespatcher::addSingleTest(HTTPServerRequest &request, std::string &response)
{
	if (request.isPost()) // actual submission
	{
		std::string thisResponse;
		if (addSingleScheduledTest(m_pMonitoringDB, request, thisResponse))
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

void HTTPServerRequestDespatcher::addScriptTest(HTTPServerRequest &request, std::string &response)
{
	if (request.isPost()) // actual submission
	{
		std::string thisResponse;
		if (addScriptScheduledTest(m_pMonitoringDB, request, thisResponse))
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

void HTTPServerRequestDespatcher::editMonitorTest(HTTPServerRequest &request, std::string &response)
{
	if (request.isPost()) // actual submission
	{
		std::string thisResponse;
		if (editSingleScheduledTest(m_pMonitoringDB, request, thisResponse))
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

		generateEditSingleScheduledTestForm(m_pMonitoringDB, testID, formContent);

		HTTPServerTemplateFileResponse resp(templatePath, title, formContent);
		response = resp.responseString();
	}
}

void HTTPServerRequestDespatcher::editScriptTest(HTTPServerRequest &request, std::string &response)
{
	if (request.isPost()) // actual submission
	{
		std::string thisResponse;
		if (editScriptScheduledTest(m_pMonitoringDB, request, thisResponse))
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

		generateEditScriptScheduledTestForm(m_pMonitoringDB, testID, formContent, tableContent, addNewPageLink);

		HTTPServerTemplateFileResponse resp(templatePath, formContent, tableContent, addNewPageLink);
		response = resp.responseString();
	}
}

void HTTPServerRequestDespatcher::addScriptPage(HTTPServerRequest &request, std::string &response)
{
	if (request.isPost()) // actual submission
	{
		std::string thisResponse;
		if (addScriptScheduledTestPage(m_pMonitoringDB, request, thisResponse))
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

		generateAddScriptScheduledTestPageForm(m_pMonitoringDB, scriptID, formContent);

		HTTPServerTemplateFileResponse resp(templatePath, title, formContent);
		response = resp.responseString();
	}
}

void HTTPServerRequestDespatcher::editScriptPage(HTTPServerRequest &request, std::string &response)
{
	if (request.isPost()) // actual submission
	{
		std::string thisResponse;
		if (editScriptScheduledTestPage(m_pMonitoringDB, request, thisResponse))
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

		generateEditScriptScheduledTestPageForm(m_pMonitoringDB, testID, formContent);

		HTTPServerTemplateFileResponse resp(templatePath, title, formContent);
		response = resp.responseString();
	}
}

void HTTPServerRequestDespatcher::viewSingleTest(HTTPServerRequest &request, std::string &response)
{
	std::string filePath = m_webContentPath + "view_single_test.tplt";

	long testID = 0;
	std::string strTestID = request.getParam("testid");
	if (!strTestID.empty())
		testID = atoi(strTestID.c_str());

	std::string description;
	std::string dataContent;
	getSingleScheduledTestResultsList(m_pMonitoringDB, testID, description, dataContent);

	HTTPServerTemplateFileResponse resp(filePath, description, dataContent);
	response = resp.responseString();
}

void HTTPServerRequestDespatcher::singleDetails(HTTPServerRequest &request, std::string &response)
{
	long runID = atoi(request.getParam("runid").c_str());

	std::string filePath = m_webContentPath + "single_details.tplt";

	std::string dataContent;
	formatDBSingleTestResponseToHTMLDL(m_pMonitoringDB, runID, dataContent);

	HTTPServerTemplateFileResponse resp(filePath, dataContent);
	response = resp.responseString();
}

void HTTPServerRequestDespatcher::singleComponents(HTTPServerRequest &request, std::string &response)
{
	long runID = atoi(request.getParam("run_id").c_str());
	long testID = atoi(request.getParam("test_id").c_str());

	std::string filePath = m_webContentPath + "single_components.tplt";

	std::string dataContent;
	getSingleScheduledTestComponentsList(m_pMonitoringDB, testID, runID, dataContent);

	HTTPServerTemplateFileResponse resp(filePath, dataContent);
	response = resp.responseString();
}

void HTTPServerRequestDespatcher::viewScriptTest(HTTPServerRequest &request, std::string &response)
{
	std::string filePath = m_webContentPath + "view_script_test.tplt";

	long testID = 0;
	std::string strTestID = request.getParam("testid");
	if (!strTestID.empty())
		testID = atoi(strTestID.c_str());

	std::string description;
	std::string dataContent;
	getScriptScheduledTestResultsList(m_pMonitoringDB, testID, description, dataContent);

	HTTPServerTemplateFileResponse resp(filePath, description, dataContent);
	response = resp.responseString();
}

void HTTPServerRequestDespatcher::scriptDetails(HTTPServerRequest &request, std::string &response)
{
	long testID = atoi(request.getParam("test_id").c_str());
	long runID = atoi(request.getParam("run_id").c_str());

	std::string filePath = m_webContentPath + "script_details.tplt";

	std::string dataContent;
	getScriptScheduledTestResultsDetails(m_pMonitoringDB, testID, runID, dataContent);

	HTTPServerTemplateFileResponse resp(filePath, dataContent);
	response = resp.responseString();
}

void HTTPServerRequestDespatcher::deleteSingleTest(HTTPServerRequest &request, std::string &response)
{
	unsigned long testID = atoi(request.getParam("test_id").c_str());

	std::string output;

	if (deleteSingleTestFromDB(m_pMonitoringDB, testID, output))
	{
		HTTPServerRedirectResponse resp("/monitoring");
		response = resp.responseString();
	}
	else
	{
		HTTPServerResponse resp(500, output);
		response = resp.responseString();
	}
}

void HTTPServerRequestDespatcher::deleteScriptTest(HTTPServerRequest &request, std::string &response)
{
	unsigned long testID = atoi(request.getParam("test_id").c_str());

	std::string output;

	if (deleteScriptTestFromDB(m_pMonitoringDB, testID, output))
	{
		HTTPServerRedirectResponse resp("/monitoring");
		response = resp.responseString();
	}
	else
	{
		HTTPServerResponse resp(500, output);
		response = resp.responseString();
	}
}

void HTTPServerRequestDespatcher::deleteScriptStep(HTTPServerRequest &request, std::string &response)
{
	unsigned long testID = atoi(request.getParam("test_id").c_str());
	unsigned long pageID = atoi(request.getParam("page_id").c_str());

	std::string output;

	if (deleteScriptStepFromDB(m_pMonitoringDB, testID, pageID, output))
	{
		std::string newURL = "/edit_script_test?test_id=";
		char szTemp[8];
		memset(szTemp, 0, 8);
		sprintf(szTemp, "%ld", testID);
		newURL.append(szTemp);

		HTTPServerRedirectResponse resp(newURL);
		response = resp.responseString();
	}
	else
	{
		HTTPServerResponse resp(500, output);
		response = resp.responseString();
	}
}

void HTTPServerRequestDespatcher::loadTesting(HTTPServerRequest &request, std::string &response)
{
	std::string output;

	if (getLoadTestRunsList(m_pLoadTestingDB, output))
	{
		std::string filePath = m_webContentPath + "load_testing.tplt";

		HTTPServerTemplateFileResponse resp(filePath, output);
		response = resp.responseString();
	}
	else
	{
		HTTPServerResponse resp(500, output);
		response = resp.responseString();
	}
}

void HTTPServerRequestDespatcher::loadTestingRunResults(HTTPServerRequest &request, std::string &response)
{
	std::string output;

	if (getLoadTestRunResults(m_pLoadTestingDB, request, output))
	{
		std::string filePath = m_webContentPath + "load_testing_run_results.tplt";

		HTTPServerTemplateFileResponse resp(filePath, output);
		response = resp.responseString();
	}
	else
	{
		HTTPServerResponse resp(500, output);
		response = resp.responseString();
	}
}

void HTTPServerRequestDespatcher::runManualSingleTest(HTTPServerRequest &request, std::string &response)
{
	if (request.isPost())
	{
		unsigned long testID = request.getParamAsLong("test_id");

		if (testID != -1)
		{
			::runManualSingleTest(m_pMonitoringDB, m_pResultsSaver, testID);
		}
	}
}

void HTTPServerRequestDespatcher::runManualScriptTest(HTTPServerRequest &request, std::string &response)
{
	if (request.isPost())
	{
		unsigned long testID = request.getParamAsLong("test_id");

		if (testID != -1)
		{
			::runManualScriptTest(m_pMonitoringDB, m_pResultsSaver, testID);
		}
	}
}
