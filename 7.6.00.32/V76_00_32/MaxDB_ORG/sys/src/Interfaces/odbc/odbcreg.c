/* @lastChanged: "2000-07-27"
 * @filename:    odbcreg.c
 * @purpose:     "Registrierungs Tool fuer ODBC Driver"
 * @release:     7.3.0.0
 * @see:         ""
 *
 * @Copyright (c) 1999-2005 SAP AG, 2000"



    ========== licence begin  GPL
    Copyright (c) 1999-2005 SAP AG

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


#ifndef WIN32
int main(int argc, char ** argv)
{
  printf("odbcreg not yet implemented.\n");
  return(-1);
}
#else
/* odbcreg.c */
#include <windows.h>
#include <stdlib.h>
#if defined(SOLARIS)
#include <stdio.h>
#endif
#include <sys/types.h>
#include <sys/stat.h>
#include <sqlext.h>

#define ERR_INVALID_OPTION "Invalid option"
#define ERR_MISSING_DRIVERNAME "Missing drivername"
#define ERR_MISSING_DRIVER "Missing name for ODBC Driver dll"
#define ERR_MISSING_SETUP  "Missing name for ODBC Setup dll"
#define ERR_MISSING_PATH   "Missing pathname"
#define ERR_CREATE_KEY     "Can't create registy key"
#define ERR_DELETE_KEY     "Can't delete registy key"
#define ERR_CREATE_ENTRY   "Can't create registry value"
#define ERR_DELETE_ENTRY   "Can't delete registry value"
#define ERR_INCORRECT_VERSION "The version number of the ODBC is unknown"

#define WARN_VERSION_NOT_FOUND "Can't get version info of ODBC Driver"
#define WARN_DRIVER_NOT_EXIST  "The specified driver does not exist"

#define DEFAULT_DRIVER_NAME_80 "MaxDB"
#define DEFAULT_DRIVER_NAME_76 "MaxDB"
#define DEFAULT_DRIVER_NAME_75 "MaxDB"
#define DEFAULT_DRIVER_NAME_74 "SAP DB 7.4"
#define DEFAULT_DRIVER_NAME_73 "SAP DB 7.3"
#define DEFAULT_DRIVER_NAME_72 "LiveCache 7.2"
#define DEFAULT_DRIVER_NAME_71 "LiveCache"
#define DEFAULT_DRIVER_NAME_62 "SAPDB ODBC Driver"
#define DEFAULT_DRIVER_NAME_61 "ADABAS 6.1 for R/3 "

#define KEY_ODBCINST                     "SOFTWARE\\ODBC\\ODBCINST.INI"
#define KEY_ODBCINST_DRIVER KEY_ODBCINST "\\ODBC Drivers"
#define ENTRY_INSTALLED                  "Installed"
#define ENTRY_DRIVER                     "Driver"
#define ENTRY_SETUP                      "Setup"
#define ENTRY_CPTIMEOUT                  "CPTimeout"

#define KEY_MDAC_VERSION                 "Software\\Microsoft\\DataAccess"

#define DRIVER_NAME_DEFAULT "sqlod32.dll"
#define SETUP_NAME_DEFAULT  "sqlsp32.dll"

typedef struct {
  WORD   wLength;
  WORD   wValueLength;
  WORD   wType;
  WCHAR  szKey[];
} t02String;

static WCHAR szVS_VERSION_INFO[]= L"VS_VERSION_INFO";

#define LOW_WORD(dword)          (WORD) ( (dword) & 0x0000FFFF )
#define HI_WORD(dword)           (WORD) ( (dword) & 0xFFFF0000 )

int t02PrintError(char * szString);

int t02PrintWarning(char * szString);

int t02PrintUsage();

void t02GetDriverName(char *szPath, char *szDriver, char *szDriverName, char *szVersion);

VS_FIXEDFILEINFO *t02GetVersion(char *szDriverName);

void t02GetLastError();

#ifndef WIN32
int main(int argc, char ** argv)
{
  printf("odbcreg not yet implemented\n");
  return(-1);
}
#else
#include <stdio.h>

int main(int argc, char ** argv)
{
    int   i;
    BOOL  install=FALSE;
    BOOL  remove=FALSE;
    BOOL  test=FALSE;
    BOOL  show=TRUE;
    char  szDriverName[MAX_PATH]="";
    char  szDriver[MAX_PATH]=DRIVER_NAME_DEFAULT;
    char  szSetup[MAX_PATH]=SETUP_NAME_DEFAULT;
    char  szPath[MAX_PATH]="";
    char  szString[sizeof(szPath)+sizeof(szDriver)];
    char  szVersion[100]="";
    DWORD cbSize;
    HKEY  hkResult;
    LONG  ret;

    if ( argc <= 0 ) {
        return(t02PrintUsage());
    }
    else
        {
            szPath[0] = '\0';           /* PTS 1108194 */

            for (i=1; i < argc;) {
                char *parm;
                char *opt=argv[i];
                if (opt) {
                    switch (*opt) {
                    case ('-'):{}
                    case ('/'):{
                        parm = argv[i+1];
                        *opt=argv[i][1];
                        if (opt) {
                            switch(*opt) {
                            case ('\0'): {
                                break;
                            }
                            case ('?'): {}
                            case ('h'): {
                                return(t02PrintUsage());
                                break;
                            }
                            case ('t'): {
                                test = TRUE;
                                show = install = FALSE;
                                i++;
                                break;
                            }
                            case ('i'): {
                                install = TRUE;
                                show = FALSE;
                                i++;
                                break;
                            }
                            case ('r'): {
                                remove = TRUE;
                                show = FALSE;
                                i++;
                                break;
                            }
                            case ('d'): {
                                if (parm && (*parm != '-' || *parm != '/')) {
                                    strcpy(szDriver, parm);
                                    i++;
                                }
                                else {
                                    return(t02PrintError(ERR_MISSING_DRIVER));
                                }
                                i++;
                                break;
                            }
                            case ('s'): {
                                if (parm && (*parm != '-' || *parm != '/')) {
                                    strcpy(szSetup, parm);
                                    i++;
                                }
                                else {
                                    return(t02PrintError(ERR_MISSING_SETUP));
                                }
                                i++;
                                break;
                            }
                            case ('p'): {
                                if (parm && (*parm != '-' || *parm != '/')) {
                                    strcpy(szPath, argv[i+1]);
                                    i++;
                                }
                                else {
                                    return(t02PrintError(ERR_MISSING_PATH));
                                }
                                i++;
                                break;
                            }
                            default: {
                                return(t02PrintError(ERR_INVALID_OPTION));
                            }
                            }
                        } /* if (opt) */
                        break;
                    }
                    default: {
                        strcpy(szDriverName, opt);
                        i++;
                    }
                    } /* switch */
                } /* if ( opt ) */
            }
        }

    if (install) {
        if (szDriver[0] == 0)
            return(t02PrintError(ERR_MISSING_DRIVER));
        if (szSetup[0] == 0)
            return(t02PrintError(ERR_MISSING_SETUP));
        if (szPath[0] == 0)
            return(t02PrintError(ERR_MISSING_PATH));
        if (szDriverName[0] == 0)
            t02GetDriverName(szPath, szDriver, szDriverName, szVersion);
        else {
            /* check if dlls exist */
            char szString[MAX_PATH]="";
            struct _stat buf;

            if ( szPath[0] != '\0' ) {
                strcpy(szString, szPath);
                strcat(szString, "\\");
            }
            strcat( szString, szDriver);
            if (_stat( szString, &buf ) == -1) {
                return(t02PrintError(WARN_DRIVER_NOT_EXIST));
            };
        };
        if (szDriverName[0] == 0)
            return(t02PrintError(ERR_MISSING_DRIVERNAME));
        if (RegCreateKey( HKEY_LOCAL_MACHINE, KEY_ODBCINST_DRIVER, &hkResult ) == ERROR_SUCCESS ) {
            ret = RegSetValueEx( hkResult, szDriverName, 0,
                                 REG_SZ, ENTRY_INSTALLED, sizeof(ENTRY_INSTALLED));
            if (ret != ERROR_SUCCESS) {
                RegCloseKey(hkResult);
                return(t02PrintError(ERR_CREATE_ENTRY));
            }
        }
        else {
            return(t02PrintError(ERR_CREATE_KEY));
        }


        if (RegCreateKey( HKEY_LOCAL_MACHINE, KEY_ODBCINST, &hkResult )
            == ERROR_SUCCESS ) {
            if (RegCreateKey( hkResult, szDriverName, &hkResult )
                == ERROR_SUCCESS ) {
                if ( szPath[0] != '\0' ) {
                    strcpy(szString, szPath);
                    strcat(szString, "\\");
                }
                strcat(szString, szDriver);
                ret = RegSetValueEx( hkResult, ENTRY_DRIVER, 0,
                                     REG_SZ, szString, strlen(szString)+1);
                if (ret != ERROR_SUCCESS) {
                    RegCloseKey(hkResult);
                    return(t02PrintError(ERR_CREATE_ENTRY));
                }
                if ( szPath[0] != '\0' ) {
                    strcpy(szString, szPath);
                    strcat(szString, "\\");
                }
                strcat(szString, szSetup);
                ret = RegSetValueEx( hkResult, ENTRY_SETUP, 0,
                                     REG_SZ, szString, strlen(szString)+1);
                if (ret != ERROR_SUCCESS) {
                    RegCloseKey(hkResult);
                    return(t02PrintError(ERR_CREATE_ENTRY));
                }
                /* set CPTimeout to 0 to disable connection pooling */
                strcpy( szString, "0" );
                ret = RegSetValueEx( hkResult, ENTRY_CPTIMEOUT, 0,
                                     REG_SZ, szString, strlen(szString)+1 );
                if (ret != ERROR_SUCCESS) {
                    RegCloseKey(hkResult);
                    return(t02PrintError(ERR_CREATE_ENTRY));
                }
            }
            else {
                RegCloseKey(hkResult);
                return(t02PrintError(ERR_CREATE_KEY));
            }
            printf("ODBC Driver for '%s' has been successfully registered.\n", szDriverName);
        }
        else {
            return(t02PrintError(ERR_CREATE_KEY));
        }
    }
    if (show) {
        if (szDriverName[0] == 0)
            t02GetDriverName(szPath, szDriver, szDriverName, szVersion);
        if (szDriverName[0] == 0)
            return(t02PrintError(ERR_MISSING_DRIVERNAME));
        if ( RegOpenKeyEx(HKEY_LOCAL_MACHINE, KEY_ODBCINST, 0, KEY_READ, &hkResult) == ERROR_SUCCESS ) {
            if ( RegOpenKeyEx(hkResult, szDriverName, 0, KEY_READ, &hkResult) == ERROR_SUCCESS ) {
                DWORD cbType;
                strcpy(szDriver, "*** Unkown ***");
                strcpy(szSetup,  szDriver);
                cbSize = sizeof(szDriver);
                ret = RegQueryValueEx( hkResult, ENTRY_DRIVER, 0, &cbType, szDriver, &cbSize );
                cbSize = sizeof(szSetup);
                ret = RegQueryValueEx( hkResult, ENTRY_SETUP, 0, &cbType, szSetup, &cbSize );
                if (szDriverName[0] != '\0')
                    printf("DriverName=%s\n", szDriverName);
                if (szDriver[0] != '\0')
                    printf("Driver=%s\n", szDriver);
                if (szSetup[0] != '\0')
                    printf("Setup=%s\n", szSetup);
                t02GetDriverName("", szDriver, NULL, szVersion);
                if (szVersion[0] != '\0') {
                    printf("Version=%s\n", szVersion);
                }
            }
            else {
                printf("Can't found entries for '%s'.", szDriverName);
            }
        }
    }
    if (remove) {
        int success=FALSE;
        if (szDriverName[0] == 0)
            t02GetDriverName(szPath, szDriver, szDriverName, szVersion);
        if (szDriverName[0] == 0)
            return(t02PrintError(ERR_MISSING_DRIVERNAME));
        if (RegOpenKeyEx( HKEY_LOCAL_MACHINE, KEY_ODBCINST_DRIVER, 0, KEY_WRITE, &hkResult ) == ERROR_SUCCESS ) {
            ret = RegDeleteValue(hkResult, szDriverName);
            if (ret == ERROR_SUCCESS) {
                success=TRUE;
            }
            RegCloseKey(hkResult);
        }
        if (RegOpenKeyEx( HKEY_LOCAL_MACHINE, KEY_ODBCINST, 0, KEY_WRITE, &hkResult ) == ERROR_SUCCESS ) {
            ret = RegDeleteKey(hkResult, szDriverName);
            if (ret == ERROR_SUCCESS) {
                success=TRUE;
            }
            RegCloseKey(hkResult);
        }
        if (success == TRUE)
            printf("ODBC Driver registration for '%s' has been successfully removed.\n", szDriverName);
        else
            printf("ODBC Driver registration for '%s' not found.\n", szDriverName);
    }

    if (test) {
      SQLHENV henv;
      SQLHDBC hdbc = SQL_NULL_HANDLE;
      char    connectString[512];
      SQLRETURN retcode;
      SQLSMALLINT length;
      char buffer[64];

      /* show MDAC-version first */
      if ( RegOpenKeyEx(HKEY_LOCAL_MACHINE, KEY_MDAC_VERSION, 0, KEY_READ, &hkResult) == ERROR_SUCCESS ) {
        char fullInstallVer[512];
        char Version[512];
        DWORD cbfIV, cbVersion;
        DWORD cbType;

        fullInstallVer[0] = Version[0] = '\0';
        cbfIV = sizeof (fullInstallVer);
        cbVersion = sizeof (Version);

        ret = RegQueryValueEx( hkResult, "FullInstallVer", 0, &cbType, fullInstallVer, &cbfIV);
        ret = RegQueryValueEx( hkResult, "Version", 0, &cbType, Version, &cbVersion);

        if (strncmp (fullInstallVer, Version, cbfIV) == 0)
          printf ("MDAC version: %s\n\n", Version);
        else
          printf ("MDAC version:\n\tFullInstallVersion:\t%s\n\tVersion:\t\t%s\n\n", fullInstallVer, Version);
      } else
        printf ("no MDAC found\n\n");

      retcode = SQLAllocHandle (SQL_HANDLE_ENV, SQL_NULL_HANDLE, &henv);
      if (retcode == SQL_SUCCESS)
        retcode = SQLSetEnvAttr (henv, SQL_ATTR_ODBC_VERSION, (void*) SQL_OV_ODBC3, 0);
      if (retcode == SQL_SUCCESS)
        retcode = SQLAllocHandle( SQL_HANDLE_DBC, henv, &hdbc );

      sprintf (connectString, "driver={%s}", szDriverName);
      if (retcode == SQL_SUCCESS) {
        printf("\nODBC Driver test for '%s'.\n", szDriverName);
        retcode = SQLDriverConnect (hdbc, NULL, connectString, SQL_NTS, NULL, 0, NULL, SQL_DRIVER_COMPLETE_REQUIRED);
      }

      if (retcode == SQL_SUCCESS) {
        SQLGetInfo (hdbc, SQL_DRIVER_VER, buffer, sizeof(buffer), &length);
        printf("Driver version %s.\n", buffer);
      } else {
  	UCHAR sqlstate[10];
  	SDWORD esq_sql_code;
  	UCHAR error_txt[500] ;
  	SWORD used_error_txt;

	SQLError (henv, hdbc, NULL,
		  sqlstate, &esq_sql_code,
		  error_txt, sizeof(error_txt),
		  &used_error_txt);
   
        printf ("not connected (%s).\n", szDriverName);
        printf ("\tSQLstate: %s\n", sqlstate);
	printf ("\tesq_sql_code: %ld", esq_sql_code);
	printf ("\t%s\n", error_txt);

      }

      if (hdbc != SQL_NULL_HANDLE)
        SQLDisconnect (hdbc);
    }
    printf("\r\n");
    return(0);
}

void t02GetDriverName(char *szPath, char *szDriver, char *szDriverName, char *szVersion)
{
    VS_FIXEDFILEINFO *pFileInfo;
    char szString[MAX_PATH]="";
    if ( szPath[0] != '\0' ) {
        strcpy(szString, szPath);
        strcat(szString, "\\");
    }
    strcat(szString, szDriver);
    pFileInfo = t02GetVersion(szString);
    if (pFileInfo) {
        WORD *MS = (WORD*)&pFileInfo->dwProductVersionMS;
        WORD *LS = (WORD*)&pFileInfo->dwProductVersionLS;
        if (szVersion)
            sprintf(szVersion, "%d.%d.%d.%d", MS[1], MS[0], LS[1], LS[0]);
        if (szDriverName) {
            switch (MS[1]) {
            case(8): {
                switch(MS[0]) {
                case(0):{
                    strcpy(szDriverName, DEFAULT_DRIVER_NAME_80);
                    break;
                }
		}
		break;
	    }
            case(7): {
                switch(MS[0]) {
                case(1):{
                    strcpy(szDriverName, DEFAULT_DRIVER_NAME_71);
                    break;
                }
                case(2):{
                    strcpy(szDriverName, DEFAULT_DRIVER_NAME_72);
                    break;
                }
                case(3):{
                    strcpy(szDriverName, DEFAULT_DRIVER_NAME_73);
                    break;
                }
                case(4):{
                    strcpy(szDriverName, DEFAULT_DRIVER_NAME_74);
                    break;
                }
                case(5):{
                    strcpy(szDriverName, DEFAULT_DRIVER_NAME_75);
                    break;
                }
                case(6):{
                    strcpy(szDriverName, DEFAULT_DRIVER_NAME_76);
                    break;
                }
                }
                break;
            }
            case(6): {
                switch(MS[0]) {
                case(1):{
                    strcpy(szDriverName, DEFAULT_DRIVER_NAME_61);
                    break;
                }
                case(2):{
                    strcpy(szDriverName, DEFAULT_DRIVER_NAME_62);
                    break;
                }
                }
                break;
            }
            }
            if (szDriverName[0] == '\0')
	      t02PrintError(ERR_INCORRECT_VERSION);
        }
    }
    else
        t02PrintWarning(WARN_VERSION_NOT_FOUND);
}

VS_FIXEDFILEINFO *t02GetVersion(char *szDriverName)
{
    HINSTANCE hModule;
    VS_FIXEDFILEINFO *pFileInfo = NULL;
    hModule = LoadLibraryEx(szDriverName, NULL, LOAD_LIBRARY_AS_DATAFILE);
    if ( hModule ) {
        LONG lParam=0;
        HRSRC hRes;
        hRes = FindResource( hModule, MAKEINTRESOURCE(1), RT_VERSION );
        if (hRes) {
            HGLOBAL hMem;
            hMem = LoadResource(hModule, hRes);
            if (hMem) {
                t02String *pVersionInfo = hMem;
                if (pVersionInfo && pVersionInfo->wValueLength) {
                    if (!wcscmp(pVersionInfo->szKey, szVS_VERSION_INFO)) {
                        int cbA = sizeof(t02String) + sizeof(szVS_VERSION_INFO);
                        /* Align to 32 bit */
                        cbA += cbA%4;
                        pFileInfo = (VS_FIXEDFILEINFO*)((char*)pVersionInfo+cbA);
                        if (pFileInfo->dwSignature != 0xfeef04bd) {
                            pFileInfo = NULL;
                        }
                    }
                }
            }
        }
    }
    else {
        t02PrintWarning(WARN_DRIVER_NOT_EXIST);
    }
    return(pFileInfo);
}

void t02GetLastError()
{
    LPVOID lpMsgBuf;

    FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
                  NULL,
                  GetLastError(),
                  MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language
                  (LPTSTR) &lpMsgBuf,
                  0,
                  NULL
                  );

    // Display the string.
    MessageBox( NULL, lpMsgBuf, "GetLastError", MB_OK|MB_ICONINFORMATION );

    // Free the buffer.
    LocalFree( lpMsgBuf );

}

#endif

int t02PrintError(char * szString)
{
    printf("Error : %s.\n", szString);
    return(-1);
}

int t02PrintWarning(char * szString)
{
    printf("Warning : %s.\n", szString);
    return(-1);
}

int t02PrintUsage()
{
    printf("\nUsage : odbcreg [\\\"][Drivername][\\\"] [ -i[d:s:p:] | -r | -d ].\n\n"\
           "Note:           if the driver name contains spaces, the name must be qouted\n\n"\
           "Options:\n"\
           "                -i Installs a driver\n"\
           "                -r Removes a driver\n"\
           "                -p <full path including drive letter>   \n"\
           "                -d <odbc dll>  default 'sqlod32.dll'\n"\
           "                -s <setup dll> default 'sqlsp32.dll'\n"\
           "                -t test, whether the driver can be loaded and MDAC version\n"\
           "\nDefault drivernames:\n"\
           "                8.0 = %s\n"\
           "                7.6 = %s\n"\
           "                7.5 = %s\n"\
           "                7.4 = %s\n"\
           "                7.3 = %s\n"\
           "                7.2 = %s\n"\
           , DEFAULT_DRIVER_NAME_80, DEFAULT_DRIVER_NAME_76, DEFAULT_DRIVER_NAME_75, DEFAULT_DRIVER_NAME_74, DEFAULT_DRIVER_NAME_73, DEFAULT_DRIVER_NAME_72);
    return(-1);
}
#endif


