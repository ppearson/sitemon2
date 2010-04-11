
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
	
	HTTPFormTextItem referrer("Referrer", "referrer", 60);
	HTTPFormTextItem expectedPhrase("Expected Phrase", "expected_phrase", 60);
	HTTPFormCheckItem acceptCompressed("Accept compressed content", "accept_compressed", true);
	
	formGen.addItem(description);
	formGen.addItem(url);
	formGen.addItem(interval);
	formGen.addItem(referrer);
	formGen.addItem(expectedPhrase);
	formGen.addItem(acceptCompressed);
	
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