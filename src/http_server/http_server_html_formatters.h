
#ifndef HTTP_SERVER_HTML_FORMATTERS_H
#define HTTP_SERVER_HTML_FORMATTERS_H

#include "http_engine.h"

bool formatResponseToHTMLDL(HTTPResponse &response, std::string &output);

void addStringToDL(std::string &output, const std::string &title);
void addIntToDL(std::string &output, const std::string &title);
void addLongToDL(std::string &output, const std::string &title);
void addFloatToDL(std::string &output, const std::string &title);

bool generateAddSingleScheduledTestForm(std::string &output);


#endif
