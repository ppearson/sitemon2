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

#ifndef HTTP_SERVER_HTML_FORMATTERS_H
#define HTTP_SERVER_HTML_FORMATTERS_H

#include "../http_engine.h"


void addStringToDL(std::string &output, const std::string &title);
void addIntToDL(std::string &output, const std::string &title);
void addLongToDL(std::string &output, const std::string &title);
void addFloatToDL(std::string &output, const std::string &title);

void formatResponseToHTMLTable(const HTTPResponse& response, std::string &output);

void addStringValueToHTMLTable(const std::string& value, const std::string &title, std::string &output);
void addIntValueToHTMLTable(int value, const std::string &title, std::string &output, bool thousandsSep = true);
void addLongValueToHTMLTable(long value, const std::string &title, std::string &output, bool thousandsSep = true);
void addDoubleValueToHTMLTable(double value, const std::string &title, std::string &output);

bool generateAddSingleScheduledTestForm(std::string &output);
bool generateAddScriptScheduledTestForm(std::string &output);


#endif
