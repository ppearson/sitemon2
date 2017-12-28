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

#include "string_helper.h"

static const std::string kBase64Chars = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

void split(const std::string& str, std::vector<std::string>& tokens, const std::string& sep)
{
    int lastPos = str.find_first_not_of(sep, 0);
    int pos = str.find_first_of(sep, lastPos);
	
    while (lastPos != -1 || pos != -1)
    {
        tokens.push_back(str.substr(lastPos, pos - lastPos));
        lastPos = str.find_first_not_of(sep, pos);
        pos = str.find_first_of(sep, lastPos);
    }
}

void toLower(std::string& str)
{
	unsigned int size = str.size();
	for (unsigned int i = 0; i < size; i++)
		str[i] = tolower(str[i]);
}

std::string base64Encode(const std::string& inputString)
{
	std::string outputString;
	
	int val0 = 0;
	int val1 = -6;
	
	for (unsigned int i = 0; i < inputString.size(); i++)
	{
		const char& c = inputString[i];
		
		val0 = (val0 << 8) + c;
		val1 += 8;
		while (val1 >= 0)
		{
			outputString.push_back(kBase64Chars[(val0 >> val1) & 0x3F]);
			val1 -= 6;
		}
	}
	
	if (val1 > -6)
	{
		outputString.push_back(kBase64Chars[((val0 << 8) >> (val1 + 8)) & 0x3F]);
	}
	
	while (outputString.size() % 4)
	{
		outputString += "=";
	}
	
	return outputString;
}

std::string base64Decode(const std::string& inputString)
{
	std::string outputString;
	
	// TODO: doing this each time is obviously inefficient...
	std::vector<int> temp(256, -1);
	for (unsigned int i = 0; i < 64; i++)
	{
		temp[kBase64Chars[i]] = i;
	}
	
	int val0 = 0;
	int val1 = -8;
	
	for (unsigned int i = 0; i < inputString.size(); i++)
	{
		const char& c = inputString[i];
		
		if (temp[c] == -1)
			break;
		
		val0 = (val0 << 6) + temp[c];
		val1 += 6;
		if (val1 >= 0)
		{
			outputString.push_back((char)(val0 >> val1) & 0xFF);
			val1 -= 8;
		}
	}
		
	return outputString;
}
