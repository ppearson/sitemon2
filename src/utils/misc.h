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

#ifndef MISC_H
#define MISC_H

#include <string>

std::string getCurrentDirectory(bool appendFinal = true);
std::string getExecutablePath(bool appendFinal = true);

bool isFullPath(const std::string &path);

bool isNumber(char *str);
bool isPath(char *str);

#endif
