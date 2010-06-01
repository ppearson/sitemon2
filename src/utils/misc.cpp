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

#ifdef _MSC_VER
#include <direct.h>
#include <windows.h>
#else
#include <dlfcn.h>
#include <unistd.h>
#endif

#include <string.h>
#include <stdio.h>
#include "misc.h"

char *getCurrentDirectory(bool appendFinal)
{
#ifdef _MSC_VER	
#if _MSC_VER > 1200
	char *szCurrentDir = _getcwd(NULL, 0);
#else
	char *szCurrentDir = getcwd(NULL, 0);
#endif
#else
	char *szCurrentDir = getcwd(NULL, 0);
#endif
	
	if (szCurrentDir && appendFinal)
	{
#ifdef _MSC_VER
		strcat(szCurrentDir, "\\");
#else
		strcat(szCurrentDir, "/");
#endif
	}
	
	return szCurrentDir;
}

// this gets the path the Sitemon executable file is in
// regardless of the current working directory from which
// Sitemon is started. This allows Sitemon to be started
// from anywhere at the command prompt and still find the
// config file in the same directory as the executable
std::string getExecutablePath(bool appendFinal)
{
#ifdef _MSC_VER
	TCHAR szHome[MAX_PATH];
	::GetModuleFileName(NULL, szHome, MAX_PATH);

	std::string path(szHome);

	int finalSlash = path.rfind("\\");
	if (finalSlash != -1)
	{
		if (appendFinal)
			finalSlash ++;

		std::string finalPath = path.substr(0, finalSlash);
		return finalPath;
	}
	else
	{
		return getCurrentDirectory(appendFinal);
	}

#else
	int ret;
	Dl_info DlInfo;
	ret = dladdr((void *)getExecutablePath, &DlInfo);
	
	if (ret)
	{
		std::string path(DlInfo.dli_fname);
		
		int finalSlash = path.rfind("/");
		if (finalSlash != -1)
		{
			if (appendFinal)
				finalSlash ++;
			
			std::string finalPath = path.substr(0, finalSlash);
			return finalPath;
		}
		else
		{
			return getCurrentDirectory(appendFinal);
		}
	}
	else
	{
		return getCurrentDirectory(appendFinal);
	}
#endif
}

bool isFullPath(const std::string &path)
{
	if (path.empty())
		return false;
	
#ifdef _MSC_VER
	if (path.find(":") == -1)
#else
	if (path[0] != '/')
#endif
	{
		return false;
	}
	
	return true;
}

bool isNumber(char *str)
{
	if (!str)
		return false;
	
	int length = strlen(str);
	
	for (int i = 0; i < length; i++)
	{
		if (!isdigit(str[i]))
		{
			return false;
		}		
	}
	
	return true;
}

bool isPath(char *str)
{
	return false;
}