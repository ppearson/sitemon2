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
#else
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