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

#include "http_server_html_formatters.h"
#include "http_form_generator.h"

bool generateAddSingleScheduledTestForm(std::string &output)
{
	HTTPFormGenerator formGen("add_monitor_test", "Add", true);
	
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

bool formatResponseToHTMLDL(HTTPResponse &response, std::string &output)
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
			response.dataStartTime, response.totalTime, response.contentSize, response.downloadSize);
	
	output.assign(szTemp);
	
	return true;
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