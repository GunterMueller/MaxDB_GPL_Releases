/*!
  -----------------------------------------------------------------------------

  module: Tools_System.cpp

  -----------------------------------------------------------------------------

  responsible:  BerndV

  special area: universal stuff for tool development

  -----------------------------------------------------------------------------

  Copyright (c) 1998-2005 SAP AG


    ========== licence begin  GPL
    Copyright (c) 1998-2005 SAP AG

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




  -----------------------------------------------------------------------------
*/

#ifdef _WIN32
  #include <windows.h>
  #include <direct.h>
  #include <errno.h>
#else
  #include <errno.h>
  #include <signal.h>
  #include <unistd.h>
  #include <limits.h>
  #include <sys/utsname.h>
  #include <dirent.h>
#endif

#include <sys/types.h>
#include <sys/stat.h>
#include <memory.h> 
#include <string.h> 
#include <stdio.h> 
#include <stdlib.h>

#include "ToolsCommon/Tools_System.hpp"
#include "SAPDBCommon/SAPDB_string.h"

/*
  -------------------------------------------------------------------------
  definition:     Macros
  -------------------------------------------------------------------------
 */
#ifdef _WIN32
  #define TOOLS_SLEEP(a)             Sleep(a*1000)
  #define TOOLS_PATHSLASH_STRING     "\\"
  #define TOOLS_PATHSLASH_CHAR       '\\'
  #define TOOLS_PATHDRIVE_CHAR       ':'
  #define TOOLS_GETCWD               _getcwd
  #define TOOLS_MAXPATH              MAX_PATH
#else
  #define TOOLS_SLEEP(a)             sleep(a)
  #define TOOLS_PATHSLASH_STRING     "/"
  #define TOOLS_PATHSLASH_CHAR       '/'
  #define TOOLS_GETCWD               getcwd
  #define TOOLS_MAXPATH              _POSIX_PATH_MAX
#endif

/*
  -------------------------------------------------------------------------
  function:     ToolsSys_sleep
  -------------------------------------------------------------------------
 */
void ToolsSys_sleep(unsigned int nSeconds)
{
  TOOLS_SLEEP(nSeconds);
} // end ToolsSys_sleep

/*
  -------------------------------------------------------------------------
  function:     ToolsSys_sysname
  -------------------------------------------------------------------------
 */
const char * ToolsSys_sysname()
{
  #ifdef _WIN32
    static char szName[100] = "";
    OSVERSIONINFO oVersion;
    oVersion.dwOSVersionInfoSize = sizeof(oVersion);
    if (GetVersionEx(&oVersion)) {
             if ((oVersion.dwPlatformId   == VER_PLATFORM_WIN32_WINDOWS)  &&
                 (oVersion.dwMajorVersion == 4                         )  &&
                 (oVersion.dwMinorVersion == 0                         )      ) {
        SAPDB_strcpy(szName, "Windows 95");
      } else if ((oVersion.dwPlatformId   == VER_PLATFORM_WIN32_WINDOWS)  &&
                 (oVersion.dwMajorVersion == 4                         )  &&
                 (oVersion.dwMinorVersion == 10                        )      ) {
        SAPDB_strcpy(szName, "Windows 98");
      } else if ((oVersion.dwPlatformId   == VER_PLATFORM_WIN32_WINDOWS)  &&
                 (oVersion.dwMajorVersion == 4                         )  &&
                 (oVersion.dwMinorVersion == 90                        )      ) {
        SAPDB_strcpy(szName, "Windows Me");
      } else if ((oVersion.dwPlatformId   == VER_PLATFORM_WIN32_NT     )  &&
                 (oVersion.dwMajorVersion == 3                         )  &&
                 (oVersion.dwMinorVersion == 51                        )      ) {
        SAPDB_strcpy(szName, "Windows NT 3.51");
      } else if ((oVersion.dwPlatformId   == VER_PLATFORM_WIN32_NT     )  &&
                 (oVersion.dwMajorVersion == 4                         )  &&
                 (oVersion.dwMinorVersion == 0                         )      ) {
        SAPDB_strcpy(szName, "Windows NT 4.0");
      } else if ((oVersion.dwPlatformId   == VER_PLATFORM_WIN32_NT     )  &&
                 (oVersion.dwMajorVersion == 5                         )  &&
                 (oVersion.dwMinorVersion == 0                         )      ) {
        SAPDB_strcpy(szName, "Windows 2000");
      } else if ((oVersion.dwPlatformId   == VER_PLATFORM_WIN32_NT     )  &&
                 (oVersion.dwMajorVersion == 5                         )  &&
                 (oVersion.dwMinorVersion == 1                         )      ) {
        SAPDB_strcpy(szName, "Windows XP");
      } else if ((oVersion.dwPlatformId   == VER_PLATFORM_WIN32_NT     )  &&
                 (oVersion.dwMajorVersion == 5                         )  &&
                 (oVersion.dwMinorVersion == 2                         )      ) {
        SAPDB_strcpy(szName, "Windows Server 2003 family");
      } else {
        SAPDB_strcpy(szName, "Windows");
      } // end if
    } // end if
    return szName;
  #else
    static struct utsname oUtsname;
    memset(&oUtsname, 0, sizeof(oUtsname));
    if (uname(&oUtsname) < 0) memset(&oUtsname, 0, sizeof(oUtsname));
    return oUtsname.sysname;
  #endif
} // end ToolsSys_sysname


/*
  -------------------------------------------------------------------------
  function:     ToolsSys_getcwd
  -------------------------------------------------------------------------
 */
const char * ToolsSys_getcwd
      ( bool bWithSlash)
{
  static char aBuffer[TOOLS_MAXPATH + 1] = "";
  if (TOOLS_GETCWD(aBuffer, 500) != NULL && bWithSlash) {
    strcat(aBuffer, TOOLS_PATHSLASH_STRING);
  } // end if
  return aBuffer;
} // end ToolsSys_getcwd

/*
  -------------------------------------------------------------------------
  function:     ToolsSys_fullpath
  -------------------------------------------------------------------------
 */
const char * ToolsSys_fullpath
      ( const char * szRelpath)
{
  static char aBuffer[TOOLS_MAXPATH + 1] = "";

  #ifdef _WIN32
    _fullpath(aBuffer, szRelpath, 500);
  #else
    if (szRelpath[0] != '/') {
      SAPDB_strcpy(aBuffer, ToolsSys_getcwd(true));
      strcat(aBuffer, szRelpath);
    } else {
      SAPDB_strcpy(aBuffer, szRelpath);
    } // end if
  #endif

  return aBuffer;
} // end ToolsSys_fullpath


/*
  -------------------------------------------------------------------------
  function:     ToolsSys_joindirs
  -------------------------------------------------------------------------
 */
const char * ToolsSys_joinpath
      ( const char * sPart1,
        const char * sPart2 )
{
  static char aBuffer[TOOLS_MAXPATH + 1] = "";

  if ((strlen(sPart1) + strlen(sPart2) + strlen(TOOLS_PATHSLASH_STRING)) <= TOOLS_MAXPATH) {
    SAPDB_strcpy(aBuffer, sPart1);
    strcat(aBuffer, TOOLS_PATHSLASH_STRING);
    strcat(aBuffer, sPart2);
  } // end if
  return aBuffer;
} // end ToolsSys_joinpath

/*
  -------------------------------------------------------------------------
  function:     ToolsSys_createdir
  -------------------------------------------------------------------------
 */
bool ToolsSys_createdir
      ( const char * szPathIn )
{
  char   szPath[TOOLS_MAXPATH + 1];
  bool   bOK   = true;
  char * pAct  = szPath; 
  char * pEnd;

  SAPDB_strcpy(szPath, szPathIn);

  #ifdef _WIN32
    if ( pAct[1] == TOOLS_PATHDRIVE_CHAR ) {
      pAct += 2;
    } // end if
  #endif

  if ( *pAct == TOOLS_PATHSLASH_CHAR ) {
    pAct++ ;
  } // end if

  do
  {
    pEnd = strchr ( pAct, TOOLS_PATHSLASH_CHAR );
    if (pEnd != NULL) {
      *pEnd = '\0';
    } // end if

    #ifdef _WIN32
      bOK = CreateDirectory ( szPath, NULL ) != 0;
      if (!bOK) {
        bOK = bOK || (GetLastError() == ERROR_ALREADY_EXISTS);
      } // end if
    #else
      bOK = mkdir ( szPath, 0775 ) == 0;
      if (!bOK) {
        bOK = bOK || (errno == EEXIST);
      } // end if
    #endif

    if (pEnd != NULL) {
      *pEnd = TOOLS_PATHSLASH_CHAR;
      pAct  = pEnd + 1;
    } // end if

    } while (bOK && pEnd != NULL);

  return bOK;
} // end ToolsSys_createdir

/*
  -------------------------------------------------------------------------
  function:     ToolsSys_removeemptydir
  -------------------------------------------------------------------------
 */
bool ToolsSys_removeemptydir
      ( const char * szPathIn )
{
  return (rmdir(szPathIn) == 0);
} // end ToolsSys_removeemptydir

/*
  -------------------------------------------------------------------------
  function:     ToolsSys_fileexists
  -------------------------------------------------------------------------
 */
bool ToolsSys_fileexists
      ( const char * szFile )
{
  bool bExists = false;
  
  struct stat sStat; 

  bExists = (stat(szFile, &sStat) == 0);

  return bExists;
} // end ToolsSys_fileexists

/*
  -------------------------------------------------------------------------
  function:     ToolsSys_killprocess
  -------------------------------------------------------------------------
 */
bool ToolsSys_killprocess
      ( unsigned int nPid ) 
{
  bool bOK = false;

#ifdef _WIN32
  HANDLE hProcess  = NULL;
  UINT   uExitCode = 0;

  hProcess = OpenProcess(PROCESS_TERMINATE, FALSE, nPid);
  if (hProcess != NULL) {
    bOK = TerminateProcess(hProcess, uExitCode) == TRUE;
    CloseHandle(hProcess);
  } else {
    bOK = (ERROR_INVALID_PARAMETER == GetLastError());
  } // end if

#else

  bOK = kill(nPid, SIGINT) == 0;
  if (!bOK) {
    bOK = bOK || (errno == ESRCH);
  } // end if

#endif

  return bOK;
} // end ToolsSys_killprocess


/*
  -------------------------------------------------------------------------
  function:     ToolsSys_errnotext
  -------------------------------------------------------------------------
 */
#ifdef E2BIG
  #define E2BIG_ENTRY     { E2BIG     , "E2BIG : argument list to long"             },
#else
  #define E2BIG_ENTRY
#endif

#ifdef EACCES
  #define EACCES_ENTRY    { EACCES    , "EACCES : permission denied"                },
#else
  #define EACCES_ENTRY
#endif

#ifdef EAGAIN
  #define EAGAIN_ENTRY    { EAGAIN    , "EAGAIN : No more processes or not enough memory or maximum nesting level reached" },
#else
  #define EAGAIN_ENTRY
#endif

#ifdef EBADF
  #define EBADF_ENTRY     { EBADF     , "EBADF : bad file number"                   },
#else
  #define EBADF_ENTRY
#endif

#ifdef ECHILD
  #define ECHILD_ENTRY    { ECHILD    , "ECHILD : no spawnde processes"             },
#else
  #define ECHILD_ENTRY
#endif

#ifdef EDEADLOCK
  #define EDEADLOCK_ENTRY { EDEADLOCK , "EDEADLOCK : resource deadlock would error" },
#else
  #define EDEADLOCK_ENTRY
#endif

#ifdef EDOM
  #define EDOM_ENTRY      { EDOM      , "EDOM : math argument"                      },
#else
  #define EDOM_ENTRY
#endif

#ifdef EEXIST
  #define EEXIST_ENTRY    { EEXIST    , "EEXIST : file exist"                       },
#else
  #define EEXIST_ENTRY
#endif

#ifdef EINVAL
  #define EINVAL_ENTRY    { EINVAL    , "EIVAL : invalid argument"                  },
#else
  #define EINVAL_ENTRY
#endif

#ifdef EMFILE
  #define EMFILE_ENTRY    { EMFILE    , "EMFILE : too many open files"              },
#else
  #define EMFILE_ENTRY
#endif

#ifdef ENOENT
  #define ENOENT_ENTRY    { ENOENT    , "ENOENT : no such file or directory"        },
#else
  #define ENOENT_ENTRY
#endif

#ifdef ENOEXEC
  #define ENOEXEC_ENTRY   { ENOEXEC   , "ENOEXEC : exec format error"               },
#else
  #define ENOEXEC_ENTRY
#endif

#ifdef ENOMEM
  #define ENOMEM_ENTRY    { ENOMEM    , "ENOMEM : not enough memory"                },
#else
  #define ENOMEM_ENTRY
#endif

#ifdef ENOSPC
  #define ENOSPC_ENTRY    { ENOSPC    , "ENOSPC : no space left on device"          },
#else
  #define ENOSPC_ENTRY
#endif

#ifdef ERANGE
  #define ERANGE_ENTRY    { ERANGE    , "ERANGE : result too large"                 },
#else
  #define ERANGE_ENTRY
#endif

#ifdef EXDEV
  #define EXDEV_ENTRY     { EXDEV     , "EXDEV : cross-device link"                 },
#else
  #define EXDEV_ENTRY
#endif

#define ERRNO_TO_TEXT {  \
   E2BIG_ENTRY           \
   EACCES_ENTRY          \
   EAGAIN_ENTRY          \
   EBADF_ENTRY           \
   ECHILD_ENTRY          \
   EDEADLOCK_ENTRY       \
   EDOM_ENTRY            \
   EEXIST_ENTRY          \
   EINVAL_ENTRY          \
   EMFILE_ENTRY          \
   ENOENT_ENTRY          \
   ENOEXEC_ENTRY         \
   ENOMEM_ENTRY          \
   ENOSPC_ENTRY          \
   ERANGE_ENTRY          \
   EXDEV_ENTRY           \
  { 0 , NULL       },    \
  { 0 , "unknown errno" }}
/*
  -------------------------------------------------------------------------
 */
const char * ToolsSys_errnotext
      ( int nErrno )
{
  int j = 0;
  struct 
  {
    int          nValue;
    const char * szText;
  } errnoArray[] = ERRNO_TO_TEXT;

  while ((errnoArray[j].szText !=  NULL) && (errnoArray[j].nValue != nErrno)) {
    j++;
  } /* end while */
  if (errnoArray[j].szText ==  NULL) {
    j++;
  } /* end if */

  return errnoArray[j].szText;
} // end ToolsSys_errnotext
/*
  -------------------------------------------------------------------------
 */
const char * ToolsSys_errnotext
      ( )
{
  return ToolsSys_errnotext(ToolsSys_errno());
} // end ToolsSys_errnotext
/*
  -------------------------------------------------------------------------
  function:     ToolsSys_errno
  -------------------------------------------------------------------------
 */
int ToolsSys_errno
      ( )
{
  return errno;
} // end ToolsSys_errno

/*
  -------------------------------------------------------------------------
  function:     ToolsSys_closedir
  -------------------------------------------------------------------------
 */
void ToolsSys_closedir
      (void * pHandle)
{
#ifdef _WIN32
  FindClose ((HANDLE) pHandle);
#else
  closedir ((DIR*) pHandle);
#endif
} // end ToolsSys_closedir

/*
  -------------------------------------------------------------------------
  function:     ToolsSys_readdir
  -------------------------------------------------------------------------
 */
bool ToolsSys_readdir
      ( void *  pHandle,
        char *  szFile,
        int     nMaxLen )
{
  bool bOk = true;
  szFile[0] = 0;

#ifdef _WIN32
  WIN32_FIND_DATA oData;
  if (FindNextFile ((HANDLE)pHandle, &oData)) {
    strncpy (szFile, oData.cFileName, nMaxLen);
    szFile[nMaxLen - 1] = 0;
  } else {
    bOk = false;
  } // end if
#else
  struct dirent* oData;
  
  oData = readdir ((DIR*) pHandle);
  if (oData != NULL) {
    strncpy (szFile, oData->d_name, nMaxLen);
    szFile[nMaxLen - 1] = 0;
  } else {
    bOk = false;
  } // end if
#endif
  
  return bOk;
} // end ToolsSys_readdir

/*
  -------------------------------------------------------------------------
  function:     ToolsSys_opendir
  -------------------------------------------------------------------------
 */
bool ToolsSys_opendir
      (       void * & pHandle,
        const char *   szPathIn,
              char *   szFile,
              int      nMaxLen )
{
  bool bOk = true;

  szFile[0] = 0;

  char szPath[TOOLS_MAXPATH];
  SAPDB_strcpy(szPath, szPathIn);

#ifdef _WIN32
  WIN32_FIND_DATA oData;

  strcat(szPath, "\\*");

  pHandle = (void*) FindFirstFile (szPath, &oData);
  if ((HANDLE)pHandle != INVALID_HANDLE_VALUE) {
    strncpy (szFile, oData.cFileName, nMaxLen);
    szFile[nMaxLen - 1] = 0;
  } else {
    bOk = false;
  } // end if
#else
  pHandle = (void *) opendir (szPath);
  if (pHandle != NULL) {
    bOk = ToolsSys_readdir(pHandle, szFile, nMaxLen);
    if (!bOk) {
      ToolsSys_closedir(pHandle);
    } // end if
  } else {
    bOk = false;
  } // end if
#endif

  return bOk;
} // end ToolsSys_opendir

/*
  -------------------------------------------------------------------------
  function:     ToolsSys_tmpname
  -------------------------------------------------------------------------
 */
const char * ToolsSys_tmpname
      ( )
{
  static char aBuffer[TOOLS_MAXPATH + 1] = "";

  static char aFile[TOOLS_MAXPATH + 1] = "";
  char    *   pFile   = NULL;

  static char aDir   [TOOLS_MAXPATH + 1] = "";
  char    *   pDir      = NULL;

  /* generate temp file name */
#ifdef _WIN32
    if (tmpnam(aBuffer) != NULL) { 
      if ((pDir = getenv("TMP")) == NULL) {
        sprintf(aFile, ".\\%s.DBM", aBuffer);
      } else {
        GetShortPathName(pDir, aDir, TOOLS_MAXPATH);
        sprintf(aFile, "%s%s.DBM", aDir, aBuffer);
      } // end if
      pFile = &aFile[0];
    } // end if
#else 
    strcpy(aFile,P_tmpdir"/DBMXXXXXX");
    int nFile = mkstemp(aFile);
    if (nFile != -1) {
      pFile = &aFile[0];
      close(nFile);
    } // end if
#endif

  return pFile;
} // end ToolsSys_tmpname