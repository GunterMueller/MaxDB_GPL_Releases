/* @lastChanged: "2001-05-29"
 * @filenameupddsnupddsn.c
 * @purpose:     "Updates Registry for ODBC Test"
 * @release:     7.3.0.0
 * @see:         ""
 *
 * @Copyright (c) 2000-2005 SAP AG"



    ========== licence begin  GPL
    Copyright (c) 2000-2005 SAP AG

    This program is free software; you can redistribute it and/or
    modify it under the terms of the GNU General Public License
    as published by the Free Software Foundation; either version 2
    of the License, or (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
    ========== licence end




 */

#include <stdio.h>
#include <string.h>
#ifdef WIN32
int __stdcall SQLWritePrivateProfileString(char *lpszSection,	char *lpszKey, 
	char *lpszString, char *lpszFile);	
#else
#include "RunTime/RTE_IniFileHandling.h"
#include "SAPDBCommon/SAPDB_Names.h"
#endif

int main(int argc, char ** argv)
{
    int globalMode;
  	char *lpszDSN, *lpszKey, *lpszString, *p;

    globalMode = !strcmp(argv[1], "-global");
	lpszDSN = lpszKey = lpszString = 0;
    if ( globalMode && argc > 3 )
    {
	    lpszDSN = argv[2];
	    lpszKey = argv[3];
	    lpszString = argv[4];
    }
    else if ( argc > 2 )
    {
	    lpszDSN = argv[1];
	    lpszKey = argv[2];
	    lpszString = argv[3];
    }

    if ( !lpszDSN )
    {
        printf("Usage : upddsn [ -global ] DSN Key [ String ].\n");
    	return(-1);
    }

    if (lpszDSN) {
	    for(;*lpszDSN == '"' || *lpszDSN == ' ';) lpszDSN++;
	    for(p=lpszDSN+strlen(lpszDSN)-1;p>lpszDSN && *p == '"' || *p == ' ';p--)
		    *p = '\0';
	}
	if (lpszKey) {
	    for(;*lpszKey == '"' || *lpszKey == ' ';) lpszKey++;
	    for(p=lpszKey+strlen(lpszKey)-1;p>lpszKey && *p == '"' || *p == ' ';p--)
		    *p = '\0';
	}
	if (lpszString) {
	    for(;*lpszString == '"' || *lpszString == ' ';) lpszString++;
	    for(p=lpszString+strlen(lpszString)-1;p>lpszString && *p == '"' || *p == ' ';p--)
		    *p = '\0';
	}
#ifdef WIN32
    return(SQLWritePrivateProfileString(lpszDSN, lpszKey, lpszString, "ODBC.INI"));
#else
    {
        tsp00_ErrTextc      ErrText;
        RTE_IniFileResult   Ok;

        if ( globalMode )
        {
            RTE_PutConfigString (SAPDB_ODBC_INI_FILE, lpszDSN, lpszKey, lpszString, ErrText, &Ok);
        }
        else
        {
            RTE_PutUserConfigString (NULL, SAPDB_ODBC_INI_FILE, lpszDSN, lpszKey, lpszString, ErrText, &Ok);
        }

        if ( SAPDB_INIFILE_RESULT_OK != Ok )
        {
            printf("Setting [%s]:%s=%s failed:%s\n", lpszDSN, lpszKey, lpszString, ErrText);
            return 1;
        }
        return 0;
    }
#endif
}
