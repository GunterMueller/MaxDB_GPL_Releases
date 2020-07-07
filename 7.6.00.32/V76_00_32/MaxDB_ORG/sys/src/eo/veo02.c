/*!
  @file           veo02.c
  @author         JoergM
  @special area   Interface for handle shared library functions
  @brief
  @see            example.html ...

\if EMIT_LICENCE

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


//
//   FreeBSD portions added by Kai Mosebach,
//   For more informations see : http://www.komadev.de/sapdb
//

\endif
*/
/*===========================================================================*
*  INCLUDES                                                                 *
*===========================================================================*/
#if defined(__GNUC__) || defined(__INTEL_COMPILER)
#  define HAS_RTLD_LOCAL_UNDERMINES_EXCEPTION_HANDLING
#endif

#include "heo02x.h"
#include "gip00.h"
#include "hsp77.h"
#include "SAPDBCommon/SAPDB_string.h"
#ifdef KERNEL_LZU
#include "geo60.h" /* nocheck */
#include "geo007_1.h" /* nocheck */
#define INFO_DLL_ERROR N(865), INFO_TYPE,_T("DCOM    "),_T("dlapi(%s) Error:%s")
#endif /* KERNEL_LZU */

#ifndef WIN32
#if !(defined SUN || defined SOLARIS || defined AIX || defined OSF1 || defined NMP || defined HPUX || defined LINUX || defined FREEBSD)
#error SUN, SOLARIS, AIX, OSF1, NMP, HPUX, FREEBSD or LINUX not defined !
#endif
#if defined SUN || SOLARIS || defined AIX || defined OSF1 || defined NMP || defined LINUX || defined FREEBSD|| ( defined HPUX && defined BIT64 )
#  include <dlfcn.h>
/* PTS 1105336 */
#elif defined HPUX && !defined(BIT64)
#  include <dl.h>
#  include <errno.h>
#endif
#if defined AIX
#  include <errno.h>
#  if defined(KERNEL_LZU)
#    include <sys/ldr.h>
#    include "RunTime/System/RTESys_IO.h" /* nocheck */
#    include "RunTime/System/RTESys_CommandPipe.h" /* nocheck */
#  endif
#endif
#  include <limits.h>
#  include <stdlib.h>
#endif

/*===========================================================================*
*  DEFINES                                                                  *
*===========================================================================*/

#ifdef NO_INLINES
# define _INLINE
#else
# define _INLINE	__inline
#endif

#ifndef TRUE
#define TRUE 1
#define FALSE 0
#endif /* TRUE */

/*===========================================================================*
*  MACROS                                                                   *
*===========================================================================*/

#define ENV_BUFFER_SIZE_MXEO02 4096

/*===========================================================================*
*  LOCAL CLASSES, STRUCTURES, TYPES, UNIONS ...                             *
*===========================================================================*/
#if defined HPUX && !defined(BIT64)
typedef struct error_array
{
  int Value;
  char *szText;
}error_array_t;
#endif

/*===========================================================================*
*  GLOBAL VARIABLES                                                         *
*===========================================================================*/



/*===========================================================================*
*  LOCAL VARIABLES                                                          *
*===========================================================================*/

#if defined HPUX && !defined(BIT64)
static error_array_t ErrorArray[] =
{
  {ENOEXEC, "file is not a shared library"},
  {ENOSYM,  "global symbol not found"},
  {EINVAL,  "invalid value"},
  {ENOMEM,  "no more memory"},
  {ENOENT,  "library does not exist"},
  {EACCES,  "no access permissions"},
  {0, 0}
};
#endif

/*===========================================================================*
*  LOCAL FUNCTIONS (PROTOTYPES)                                             *
*===========================================================================*/

static void eo02GetErrorText (const char *DLFunction, int ErrorValue, char *ErrText, int cbLenErrText);
#if defined(KERNEL_LZU) && defined(AIX)
static void eo02AixQueryError(const char *DLFunction, tsp00_Pathc fullName, char *ErrText, int cbLenErrText);
#endif
static char *eo02SplitPathFromFullName(char *szFullName, char *szPath, char *szModule);
static int  eo02ModifySearchPath(char *szExtPath, char *szOldPath);
static void eo02UndoModifySearchPath(char *szOldPath);

/*===========================================================================*
*  GLOBAL FUNCTIONS (CODE)                                                  *
*===========================================================================*/
HANDLE sqlLoadLibrary(char *szPath, char *ErrText, int cbLenErrText)
{
  return( sqlLoadLibraryEx(szPath, RTLD_LOCAL_EO02, ErrText, cbLenErrText) );
}

HANDLE sqlLoadLibraryEx(char *szPath, int Mode, char *ErrText, int cbLenErrText)
{
  HANDLE h=0;
  tsp00_Pathc szFullName;
  /*tsp00_Pathc szLibPath;*/
  /*tsp00_Pathc szModule; */

  /* putenv doesn't allow 'automatic' variables */
  /* static char        szOldPath[ENV_BUFFER_SIZE_MXEO02]; */
  int         len;
#ifndef WIN32
  int         dlMode;
#endif

  len = (int)strlen(szPath) + (int)strlen(DLLEXT_IP00) + 1;

  if ( len > sizeof(tsp00_Path) )
  {
    strncpy((char *)ErrText, "Path too long", cbLenErrText );
    return 0;
  }

  strcpy(szFullName, szPath);
  strcat(szFullName, DLLEXT_IP00);

  /* manipulation of PATH variable doesn't work on sun, so we can't rely on it *
  eo02SplitPathFromFullName(szFullName, szLibPath, szModule);

    switch( eo02ModifySearchPath(szLibPath,szOldPath) )
    {
    case 1:
    strncpy((char *)ErrText, "Environment path too long", cbLenErrText );
    return 0;
    case 2:
    strncpy((char *)ErrText, "putenv failed", cbLenErrText );
    return 0;
    default:
    break;
    }
  ******************************************************************************/

#ifdef WIN32
  if((h = LoadLibraryEx(szFullName, NULL, LOAD_WITH_ALTERED_SEARCH_PATH)) <= (HANDLE)HINSTANCE_ERROR)
  {
    eo02GetErrorText("LoadLibraryEx", GetLastError(), ErrText, cbLenErrText);
  }
#else
#if defined SUN || defined SOLARIS || defined AIX || defined OSF1 || defined NMP || defined LINUX || ( defined HPUX && defined BIT64 ) || defined(FREEBSD)
  dlMode = RTLD_LAZY;
#  ifdef HAS_RTLD_LOCAL_UNDERMINES_EXCEPTION_HANDLING
    dlMode = dlMode | RTLD_GLOBAL;
#  else
#    if !defined OSF1
  if ( RTLD_GLOBAL_EO02 == ( Mode & RTLD_GLOBAL_EO02 )) 
    dlMode = dlMode | RTLD_GLOBAL;
  if ( RTLD_LOCAL_EO02 == ( Mode & RTLD_LOCAL_EO02 )) 
    dlMode = dlMode | RTLD_LOCAL;
#    endif
#  endif
  h = (HANDLE) dlopen (szFullName, dlMode);
  if (h == 0)
  {
#if defined(KERNEL_LZU) && defined(AIX)
      eo02AixQueryError("dlopen", szFullName, ErrText, cbLenErrText);
#else
      eo02GetErrorText("dlopen", 0, ErrText, cbLenErrText);
#endif
  }
#elif defined HPUX && !defined(BIT64)
  h = (HANDLE) shl_load (szFullName, BIND_DEFERRED, 0);
  if (h == NULL)
  {
    eo02GetErrorText ("shl_load", errno, ErrText, cbLenErrText);
  }
#endif
#endif
  /* manifulation of PATH variable doesn't work on sun, so we can't rely on it *
  eo02UndoModifySearchPath(szOldPath);
  *                                                                            */

  return(h);
}

/*-------------*/

sqlPROC sqlGetProcAddress(HANDLE hLib, const char *szSqlFunction, char *ErrText, int cbLenErrText)
{
  sqlPROC p = NULL;
#ifdef WIN32
  union {
    FARPROC pV;
    sqlPROC p;
  } u;
  u.p = 0;
  if(!(u.pV = GetProcAddress( hLib, szSqlFunction)))
  {
    eo02GetErrorText("GetProcAddress", GetLastError(), ErrText, cbLenErrText);
  }
#else
  union {
      void *pV;
      sqlPROC p;
  } u;
#if defined SUN || defined SOLARIS || defined AIX || defined OSF1 || defined NMP || defined LINUX || ( defined HPUX && defined BIT64 ) || defined FREEBSD
  u.p = 0;
  u.pV = dlsym ((void *) hLib, szSqlFunction);
  if (u.pV == NULL)
  {
    eo02GetErrorText ("dlsym", 0, ErrText, cbLenErrText);
    return NULL;
  }
#elif defined HPUX && !defined(BIT64)
  int rc = 0;
  u.p = 0;
  rc = shl_findsym ((shl_t *) &hLib, szSqlFunction, TYPE_UNDEFINED,
    (void *)&u.p);
  if (rc != 0)
  {
    eo02GetErrorText ("shl_findsym", errno, ErrText, cbLenErrText);
    return NULL;
  }
#endif
#endif
  return(u.p);
}

/*-------------*/

boolean sqlFreeLibrary(HANDLE hLib, char *ErrText, int cbLenErrText)
{
#ifdef WIN32
  boolean rc = FreeLibrary(hLib);
  if (rc != 0)
  {
    return TRUE;
  }
  else
  {
    eo02GetErrorText("FreeLibrary", GetLastError(), ErrText, cbLenErrText);
    return FALSE;
  }
#else
  int rc = 0;

#if defined SUN || defined SOLARIS || defined AIX || defined OSF1 || defined NMP || defined LINUX || ( defined HPUX && defined BIT64 ) || defined(FREEBSD)
  rc = dlclose ((void *) hLib);
  if (rc != 0)
  {
    eo02GetErrorText ("dlclose", 0, ErrText, cbLenErrText);
    return FALSE;
  }
#elif defined HPUX && !defined(BIT64)
  rc = shl_unload ((shl_t) hLib);
  if (rc != 0)
  {
    eo02GetErrorText ("shl_unload", errno, ErrText, cbLenErrText);
    return FALSE;
  }	
#endif
  return TRUE;
#endif
}

/*===========================================================================*
*  LOCAL FUNCTIONS (CODE)                                                   *
*===========================================================================*/

#if defined(KERNEL_LZU) && defined(AIX)

/*
  Since dlerror() on AIX is not working as wanted the detailed error information
  is collected using 'loadquery()' + execerror. The latter is a pretty print program
  for the cryptic strings returned by loadquery().
 */
static void eo02AixQueryError(const char *DLFunction, tsp00_Pathc fullName, char *ErrText, int cbLenErrText)
{
    char *szErrorText = dlerror();
#ifdef KERNEL_LZU
    /* maybe same day the dlerror() call works on AIX too.... */
    if ( szErrorText )
    {
        if ( strcmp("dlsym", DLFunction) != 0 )
        {
            MSGD(( ERR_DLL_ERROR, DLFunction, szErrorText ));
        }
        else
        {
            MSGD(( INFO_DLL_ERROR, DLFunction, szErrorText ));
        }
        strncpy(ErrText, szErrorText, cbLenErrText-1);
    }
    ErrText[cbLenErrText-1] = '\0';

    {
        int iMsg;
        char **cmdArgv = 0;
        SAPDB_Int4 bufferSize = 8192;

        /* try again until either buffer exceeds mem available or loadquery is satisfied...*/
        do
        {
            cmdArgv = (char **)malloc(bufferSize);
            if ( cmdArgv )
            {
                cmdArgv[0] = (char *)"/usr/sbin/execerror";
                cmdArgv[1] = &fullName[0];
                iMsg = loadquery( L_GETMESSAGES
                                , &cmdArgv[2]
                                , bufferSize - ( 2 * sizeof(char *) ) );
                if ( (-1 == iMsg) && (ENOMEM == errno) )
                {
                    free(cmdArgv);
                    bufferSize *= 2;
                    errno = ENOMEM;
                }
            }
            else
            {
                MSGD(( ERR_DLL_ERROR, DLFunction, "not enough memory for the loadQuery() buffer" ));
                return;
            }
        } while ( (-1 == iMsg) && (ENOMEM == errno) );

        /* if loadquery got a severe proble give up */
        if ( iMsg == -1 )
        {
            free(cmdArgv);
            MSGD(( ERR_DLL_ERROR, DLFunction, "the loadQuery() call failed" ));
            return;
        }

        /* now use 'execerror' to get the pretty print */
        {
            RTESys_CommandOutputPipeContext cmdContext;

            if ( RTESys_OpenCommandVectorOutputPipe( &cmdContext, &cmdArgv[0]) != RTE_SYSTEMRC_NO_ERROR )
            {
                free(cmdArgv);
                MSGD(( ERR_DLL_ERROR, DLFunction, "calling /usr/sbin/execerror failed" ));
                return;
            }

            /* pipe waits to be emptied... */
            {
                RTE_SystemRc rc;
                SAPDB_Int4 exitStatus;
#define MESSAGE_TEXT_LIMIT  115
                SAPDB_Char MessageBuffer[MESSAGE_TEXT_LIMIT + 1];
                SAPDB_Char * nextInput;
                tsp00_VfReturn_Param returnValue;
                RTE_FileOffset bytesReadIntoBuffer;
                RTE_FileOffset remainingBufferSpace;

                SAPDB_memset(&MessageBuffer[0], 0, MESSAGE_TEXT_LIMIT+1);
                bytesReadIntoBuffer = 0;

                /* do not exceed MESSAGE_TEXT_LIMIT on output... */
                do
                {
                    RTE_FileOffset bytesReadFromPipe;

                    remainingBufferSpace = MESSAGE_TEXT_LIMIT - bytesReadIntoBuffer;
                    nextInput = &MessageBuffer[bytesReadIntoBuffer];
                    bytesReadFromPipe = 0;
                    RTESys_IORead( cmdContext.cmdOutput
                                 , nextInput
                                 , remainingBufferSpace
                                 , &bytesReadFromPipe
                                 , &returnValue);
                    if ( vf_ok == returnValue && bytesReadFromPipe > 0 )
                    {
                        SAPDB_Char *gotNewLine;

                        bytesReadIntoBuffer += bytesReadFromPipe;
                        gotNewLine = strchr(&MessageBuffer[0], '\n');
                        if ( gotNewLine )
                        {
                            while ( gotNewLine )
                            {
                                *gotNewLine = 0;
                                MSGD(( ERR_DLL_ERROR, DLFunction, &MessageBuffer[0] ));
                                bytesReadIntoBuffer = (&MessageBuffer[bytesReadIntoBuffer] - (gotNewLine+1));
                                SAPDB_memmove(&MessageBuffer[0]
                                      , gotNewLine + 1
                                      , bytesReadIntoBuffer);
                                SAPDB_memset(&MessageBuffer[bytesReadIntoBuffer]
                                     , 0
                                     , MESSAGE_TEXT_LIMIT + 1 - bytesReadIntoBuffer);
                                gotNewLine = strchr(&MessageBuffer[0], '\n');
                            }
                        }
                        else if ( bytesReadIntoBuffer == MESSAGE_TEXT_LIMIT )
                        {
                            MSGD(( ERR_DLL_ERROR, DLFunction, &MessageBuffer[0] ));
                            SAPDB_memset(&MessageBuffer[0]
                                 , 0
                                 , MESSAGE_TEXT_LIMIT+1);
                            bytesReadIntoBuffer = 0;
                        }
                    }
                } while ( vf_ok == returnValue );

                /* finally done. Release buffer and close pipe. */
                free(cmdArgv);

                rc = RTESys_CloseCommandOutputPipe(&cmdContext, &exitStatus);
                if ( RTE_SYSTEMRC_NO_ERROR != rc )
                {
                    MSGD(( ERR_DLL_ERROR, DLFunction, "closing command output pipe failed" ));
                    return;
                }
            }
        }
    }
#endif /* KERNEL_LZU */
}
#endif /* AIX */

static void eo02GetErrorText(const char *DLFunction, int ErrorValue, char *ErrText, int cbLenErrText)
{
#ifdef WIN32
  LPVOID lpMsgBuf;
    if(0 != FormatMessage( FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
      NULL, ErrorValue, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPTSTR) &lpMsgBuf, 0, NULL ) && (NULL != lpMsgBuf) )
    {
      strncpy(ErrText, lpMsgBuf, cbLenErrText-1);
    }
    else
    {
      sp77sprintf( ErrText, cbLenErrText-1, "error code:%d", ErrorValue );
    }
#ifdef KERNEL_LZU
  if ( strcmp("GetProcAddress", DLFunction) != 0 )
  {
   MSGD(( ERR_DLL_ERROR, DLFunction, lpMsgBuf ));
  }
  else
  {
   MSGD(( INFO_DLL_ERROR, DLFunction, lpMsgBuf ));
  }
#endif /* KERNEL_LZU */
  LocalFree(lpMsgBuf);
#elif defined HPUX && !defined(BIT64)
  int i;
  char ErrorMessage[32];
  char *p=NULL;
  for (i = 0; ErrorArray[i].szText != 0; i++)
  {
    if (ErrorArray[i].Value == ErrorValue)
    {
      p = ErrorArray[i].szText;
      break;
    }
  }
  if (p == NULL)
  {
    sprintf( &ErrorMessage[0], "Unknown error code:%d", ErrorValue );
    p = &ErrorMessage[0];
  }
#ifdef KERNEL_LZU
  if ( strcmp("shl_findsym", DLFunction) != 0 )
  {
   MSGD(( ERR_DLL_ERROR, DLFunction, p ));
  }
  else
  {
   MSGD(( INFO_DLL_ERROR, DLFunction, p ));
  }
#endif /* KERNEL_LZU */
  strncpy(ErrText, p, cbLenErrText-1);
#elif defined SUN || defined SOLARIS || defined AIX || defined OSF1 || defined NMP || defined LINUX || ( defined HPUX && defined BIT64 ) || defined FREEBSD
  {
    char *szErrorText = dlerror();
#ifdef KERNEL_LZU
  if ( strcmp("dlsym", DLFunction) != 0 )
  {
    MSGD(( ERR_DLL_ERROR, DLFunction, szErrorText ));
  }
  else
  {
    MSGD(( INFO_DLL_ERROR, DLFunction, szErrorText ));
  }
#endif /* KERNEL_LZU */
    strncpy(ErrText, szErrorText, cbLenErrText-1);
  }
#endif
  ErrText[cbLenErrText-1] = '\0';
  return;
}

static int eo02ModifySearchPath(char *szExtPath, char *szOldPath)
{
  const char *szEnvVar = LDLIBPATH_IP00;
  const char *szSep    = ENVSEP_IP00;
  char *szPath = getenv(szEnvVar);

  /* Buffer to hold the following String "PATH=szExtPath;szOldPath" */
  /* putenv doesn't allow 'automatic' variables                     */
  static char  szNewPath[ENV_BUFFER_SIZE_MXEO02];

  /* if 2.nd call  szOldPath = active environment variable          */
  /* don't change environment before setting new environment        */
  /* szOldPath[0] = '\0';  PTS 1104046                              */

  if (szPath == NULL)
  {
    szPath = "";
  }

  if ( strlen(szEnvVar)+strlen("=")+strlen(szExtPath)+strlen(szPath)+strlen(szSep)
    >= ENV_BUFFER_SIZE_MXEO02)
  {
    return 1;
  }

  strcpy(szNewPath, szEnvVar);
  strcat(szNewPath, "=");
  strcat(szNewPath, szExtPath);
  strcat(szNewPath, szSep);
  strcat(szNewPath, szPath);

  /* export the PATH Variable */
  if ( putenv(szNewPath) )
  {
    return 2;
  }

  szOldPath[0] = '\0';  /* PTS 1104046 */
  strcpy(szOldPath, szEnvVar);
  strcat(szOldPath, "=");
  strcat(szOldPath, szPath);

  return 0;
}

/*-------------*/

static void eo02UndoModifySearchPath(char *szOldPath)
{
  if (szOldPath[0] != '\0')
  {
    putenv(szOldPath);
  }
}

/*-------------*/

static char *eo02SplitPathFromFullName(char *szFullName, char *szPath, char *szModule)
{
#ifdef WIN32
  const char PathSep = '\\';
#else
  const char PathSep = '/';
#endif

  int i;
  int len = (int)strlen(szFullName);
  for (i=len; i > 0; i--)
  {
    if (szFullName[i] == PathSep)
    {
      break;
    }
  }
  if (szPath != NULL)
  {
    SAPDB_memcpy(szPath, szFullName, i);
    szPath[i] = '\0';          /* terminate with zero */
  }
  if (szModule != NULL)
  {
    SAPDB_memcpy(szModule, szFullName+i+1, len-i); /* this includes 0 Byte */
  }
  return szPath;
}


/*===========================================================================*
*  END OF CODE                                                              *
*===========================================================================*/
