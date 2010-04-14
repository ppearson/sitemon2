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