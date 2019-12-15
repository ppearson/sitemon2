/*
 Sitemon
 Copyright 2010-2019 Peter Pearson.
 
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

#ifndef STRING_HELPER_H
#define STRING_HELPER_H

#include <string>
#include <vector>

class StringHelpers
{
public:

	static void split(const std::string& str, std::vector<std::string>& lines, const std::string& sep = "\n");
	static void toLower(std::string& str);
	
	static std::string formatSize(size_t amount);
	static std::string formatNumberThousandsSeparator(size_t value);

	static std::string base64Encode(const std::string& inputString);
	static std::string base64Decode(const std::string& inputString);

	static std::string generateRandomASCIIString(unsigned int length);
	
};

#endif
