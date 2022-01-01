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

#include "http_server_html_formatters.h"
#include "http_form_generator.h"

bool generateAddSingleScheduledTestForm(std::string &output)
{
	HTTPFormGenerator formGen("add_single_test", "Add", true);
	
	HTTPFormTextItem description("Description", "description", 40);
	HTTPFormTextItem url("URL", "url", 50);
	HTTPFormSelectItem interval("Interval", "interval", 2);
	interval.addOption("1");
	interval.addOption("5");
	interval.addOption("10");
	interval.addOption("15");
	interval.addOption("20");
	interval.addOption("30");
	interval.addOption("60");
	
	HTTPFormTextItem expectedPhrase("Expected Phrase", "expected_phrase", 60);
	HTTPFormCheckItem acceptCompressed("Accept compressed content", "accept_compressed", true);
	HTTPFormCheckItem downloadComponents("Download components", "download_components", true);
	
	formGen.addItem(description);
	formGen.addItem(url);
	formGen.addItem(interval);
	formGen.addItem(expectedPhrase);
	formGen.addItem(acceptCompressed);
	formGen.addItem(downloadComponents);
	
	output = formGen.getGeneratedCode();	
	
	return true;
}

bool generateAddScriptScheduledTestForm(std::string &output)
{
	HTTPFormGenerator formGen("add_script_test", "Add", true);
	
	HTTPFormTextItem description("Description", "description", 60);
	HTTPFormSelectItem interval("Interval", "interval", 2);
	interval.addOption("1");
	interval.addOption("5");
	interval.addOption("10");
	interval.addOption("15");
	interval.addOption("20");
	interval.addOption("30");
	interval.addOption("60");
	
	HTTPFormCheckItem acceptCompressed("Accept compressed content", "accept_compressed", true);
	HTTPFormCheckItem downloadComponents("Download components", "download_components", true);
	
	formGen.addItem(description);
	formGen.addItem(interval);
	formGen.addItem(acceptCompressed);
	formGen.addItem(downloadComponents);
	
	///
	
	HTTPFormTextItem page1Description("Page 1 desc", "p1_desc", 50);
	HTTPFormTextItem page1URL("Page 1 URL", "p1_url", 50);
	HTTPFormTextItem page2Description("Page 2 desc", "p2_desc", 50);
	HTTPFormTextItem page2URL("Page 2 URL", "p2_url", 50);
	HTTPFormTextItem page3Description("Page 3 desc", "p3_desc", 50);
	HTTPFormTextItem page3URL("Page 3 URL", "p3_url", 50);
	HTTPFormTextItem page4Description("Page 4 desc", "p4_desc", 50);
	HTTPFormTextItem page4URL("Page 4 URL", "p4_url", 50);
	HTTPFormTextItem page5Description("Page 5 desc", "p5_desc", 50);
	HTTPFormTextItem page5URL("Page 5 URL", "p5_url", 50);

	formGen.addSeparator();
	
	formGen.addItem(page1Description);
	formGen.addItem(page1URL);
	formGen.addItem(page2Description);
	formGen.addItem(page2URL);
	formGen.addItem(page3Description);
	formGen.addItem(page3URL);
	formGen.addItem(page4Description);
	formGen.addItem(page4URL);
	formGen.addItem(page5Description);
	formGen.addItem(page5URL);
	
	output = formGen.getGeneratedCode();
	
	return true;
}

void formatResponseToHTMLDL(const HTTPResponse& response, std::string &output)
{
	char szTemp[2048];
	memset(szTemp, 0, 2048);
	
	std::string format = "<dl>\n";
	addStringToDL(format, "Final URL");
	addLongToDL(format, "Response code");
	addFloatToDL(format, "Lookup time");
	addFloatToDL(format, "Connect time");
	addFloatToDL(format, "Data start time");
	addFloatToDL(format, "Total time");
	addLongToDL(format, "Download size");
	addLongToDL(format, "Content size");
	format += "</dl>\n";
	
	sprintf(szTemp, format.c_str(), response.finalURL.c_str(), response.responseCode, response.lookupTime, response.connectTime,
			response.dataStartTime, response.totalTime, response.totalDownloadSize, response.totalContentSize);
	
	output.assign(szTemp);
}

void addStringToDL(std::string &output, const std::string &title)
{
	std::string newString = " <dt>" + title + "</td>\n <dd>%s</dd>\n";
	output += newString;
}

void addIntToDL(std::string &output, const std::string &title)
{
	std::string newString = " <dt>" + title + "</td>\n <dd>%i</dd>\n";
	output += newString;
}

void addLongToDL(std::string &output, const std::string &title)
{
	std::string newString = " <dt>" + title + "</td>\n <dd>%ld</dd>\n";
	output += newString;
}

void addFloatToDL(std::string &output, const std::string &title)
{
	std::string newString = " <dt>" + title + "</td>\n <dd>%f</dd>\n";
	output += newString;
}

void formatResponseToHTMLTable(const HTTPResponse& response, std::string &output)
{
	char szTemp[4096];
	memset(szTemp, 0, 4096);
	
	std::string format = "<table class=\"params\">\n";
	addStringToTable(format, "Final URL:");
	addLongToTable(format, "Response code:");
	
	addStringToTable(format, "HTTP Version:");
	
	addFloatToTable(format, "Lookup time:");
	addFloatToTable(format, "Connect time:");
	if (response.sslHandshakeTime > 0.0f)
	{
		addFloatToTable(format, "TLS time:");
	}
	addFloatToTable(format, "Data start time:");
	addFloatToTable(format, "Total time:");
	addLongToTable(format, "Download size:");
	addLongToTable(format, "Content size:");
	format += "</table>\n";
	
	std::string httpVersion = HTTPResponse::getHTTPResponseVersionAsString(response.httpVersion);
	
	if (response.sslHandshakeTime > 0.0f)
	{
		sprintf(szTemp, format.c_str(), response.finalURL.c_str(), response.responseCode, httpVersion.c_str(), response.lookupTime, response.connectTime,
				response.sslHandshakeTime, response.dataStartTime, response.totalTime, response.totalDownloadSize, response.totalContentSize);
	}
	else
	{
		sprintf(szTemp, format.c_str(), response.finalURL.c_str(), response.responseCode, httpVersion.c_str(), response.lookupTime, response.connectTime,
				response.dataStartTime, response.totalTime, response.totalDownloadSize, response.totalContentSize);
	}
	
	output.assign(szTemp);
}

void addStringToTable(std::string &output, const std::string &title)
{
	std::string newString = " <tr><th>" + title + "</th><td>%s</td></tr>\n";
	output += newString;
}

void addIntToTable(std::string &output, const std::string &title)
{
	std::string newString = " <tr><th>" + title + "</th><td>%i</td></tr>\n";
	output += newString;
}

void addLongToTable(std::string &output, const std::string &title)
{
	std::string newString = " <tr><th>" + title + "</th><td>%ld</td></tr>\n";
	output += newString;
}

void addFloatToTable(std::string &output, const std::string &title)
{
	std::string newString = " <tr><th>" + title + "</th><td>%f</td></tr>\n";
	output += newString;
}
