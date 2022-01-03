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

#include "utils/string_helper.h"

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
	std::string tableCode = "<table class=\"params\">\n";
	addStringValueToHTMLTable(response.finalURL, "Final URL:", tableCode);
	addLongValueToHTMLTable(response.responseCode, "Response code:", tableCode);
	
	std::string httpVersion = HTTPResponse::getHTTPResponseVersionAsString(response.httpVersion);
	addStringValueToHTMLTable(httpVersion, "HTTP Version:", tableCode);
	
	if (response.redirectCount > 0)
	{
		addLongValueToHTMLTable(response.redirectCount, "Redirect count:", tableCode);
	}
	
	if (!response.contentType.empty())
	{
		addStringValueToHTMLTable(response.contentType, "Content Type", tableCode);
	}
	
	addDoubleValueToHTMLTable(response.lookupTime, "Lookup time:", tableCode);
	addDoubleValueToHTMLTable(response.connectTime, "Connect time:", tableCode);
	if (response.sslHandshakeTime > 0.0f)
	{
		addDoubleValueToHTMLTable(response.sslHandshakeTime, "TLS time:", tableCode);
	}
	addDoubleValueToHTMLTable(response.dataStartTime, "Data start time:", tableCode);
	addDoubleValueToHTMLTable(response.totalTime, "Total time:", tableCode);
	
	addLongValueToHTMLTable(response.downloadSize, "Download size:", tableCode);
	addLongValueToHTMLTable(response.contentSize, "Content size:", tableCode);
	
	if (response.componentDownloadSize > 0)
	{
		addLongValueToHTMLTable(response.totalDownloadSize, "Comp. download size:", tableCode);
		addLongValueToHTMLTable(response.totalContentSize, "Comp. content size:", tableCode);
	}
	tableCode += "</table>\n";
	
	output = tableCode;
}

void addStringValueToHTMLTable(const std::string& value, const std::string &title, std::string &output)
{
	std::string newString = " <tr><th>" + title + "</th><td>" + value + "</td></tr>\n";
	output += newString;
}

void addIntValueToHTMLTable(int value, const std::string &title, std::string &output, bool thousandsSep)
{
	std::string strValue = thousandsSep ? StringHelpers::formatNumberThousandsSeparator(value) : std::to_string(value);
	std::string newString = " <tr><th>" + title + "</th><td>" + strValue + "</td></tr>\n";
	output += newString;
}

void addLongValueToHTMLTable(long value, const std::string &title, std::string &output, bool thousandsSep)
{
	std::string strValue = thousandsSep ? StringHelpers::formatNumberThousandsSeparator(value) : std::to_string(value);
	std::string newString = " <tr><th>" + title + "</th><td>" + strValue + "</td></tr>\n";
	output += newString;
}

void addDoubleValueToHTMLTable(double value, const std::string &title, std::string &output)
{
	// TODO: might want to use a different approach here, so we can control significant digits?
	
	std::string newString = " <tr><th>" + title + "</th><td>" + std::to_string(value) + "</td></tr>\n";
	output += newString;
}
