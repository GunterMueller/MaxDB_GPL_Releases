/*!---------------------------------------------------------------------
  @file           RTESys_SharedLibCode.c
  @author         JoergM
  @brief          System Calls for shared library access
  @see

\if EMIT_LICENCE

    ========== licence begin  GPL
    Copyright (c) 2001-2005 SAP AG

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
---------------------------------------------------------------------*/




/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#if defined(__GNUC__) || defined(__INTEL_COMPILER)
#  define HAS_RTLD_LOCAL_UNDERMINES_EXCEPTION_HANDLING
#endif

#include "RunTime/System/RTESys_SharedLib.h"
#include "SAPDBCommon/SAPDB_sprintf.h"

#ifdef WIN32

#  include <windows.h>

#else

#  if !(defined SUN || defined SOLARIS || defined AIX || defined OSF1 || defined NMP || defined HPUX || defined LINUX || defined FREEBSD)
#    error SUN, SOLARIS, AIX, OSF1, NMP, HPUX, FREEBSD or LINUX not defined !
#  endif

#  if defined SUN || defined SOLARIS || defined AIX || defined OSF1 || defined NMP || defined LINUX || ( defined HPUX && defined BIT64 ) || defined (FREEBSD)
#    include <dlfcn.h>
#  elif defined HPUX && !defined(BIT64)
#    include <dl.h>
#    include <errno.h>
#  endif

#  include <errno.h>
/* #  include <limits.h> */

#endif

#include <stdlib.h>
#include <string.h>


/*===========================================================================*
 *  DEFINES                                                                  *
 *===========================================================================*/

/*===========================================================================*
 *  MACROS                                                                   *
 *===========================================================================*/

/*===========================================================================*
 *  STRUCTURES, TYPES, UNIONS ...                                            *
 *===========================================================================*/

#if defined HPUX && !defined(BIT64)
static struct error_array_
{
  int Value;
  char *szText;
} ErrorArray[] =
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
 *  STATIC FUNCTION PROTOTYPES                                               *
 *===========================================================================*/

static void FillLocalErrorText( SAPDB_Char const * DLFunction,
                                SAPDB_Int4         errorValue,
                                SAPDB_Char *       errorText,
                                SAPDB_Int4         maxErrorTextLengthInBytes);

/*===========================================================================*
 *  GLOBAL FUNCTIONS                                                         *
 *===========================================================================*/

SAPDB_Bool RTESys_LoadSharedLibrary( SAPDB_Char const                * sharedLibName,
                                     RTE_LoadMode                      loadMode,
                                     SAPDB_Char                      * errorText,
                                     SAPDB_Int4                        maxErrorTextLengthInBytes,
                                     RTE_Path                          fullPath,
                                     RTE_SharedLibHandle             * pSharedLibHandle )
{
    SAPDB_Bool          result;
    SAPDB_Char const *  DLFunction;
    SAPDB_Int4          errorValue;
    SAPDB_Int4          len;
    SAPDB_Int4          iSearch;
    RTE_SharedLibHandle hLib;
#ifndef WIN32
    int                 dlMode;
#endif

    len = (int)strlen(sharedLibName);
    if ( len < 1 )
    {
        strncpy(errorText, "-no shared lib name given-", maxErrorTextLengthInBytes-1);
        errorText[maxErrorTextLengthInBytes-1] = 0;
        return false;
    }

    for ( iSearch = (len-1); iSearch > 0; --iSearch)
    {
        switch ( sharedLibName[iSearch] )
        {
        case '.':
            iSearch = 0;
            break;
        case '/':
            iSearch = 1; /* Force end of loop */
            break;
#ifdef WIN32
        case '\\':
            iSearch = 1; /* Force end of loop */
            break;
#endif
        default:
            break;
        }
    }

    if ( iSearch == 0 )
    {
        len += (SAPDB_Int4)strlen(SAPDB_DLL_EXTENSION);
    }
    len += 1; /* 0 byte termination */

    if ( len > sizeof(RTE_Path) )
    {
        strncpy(errorText, "shared lib name too long", maxErrorTextLengthInBytes-1);
        errorText[maxErrorTextLengthInBytes-1] = 0;
        return false;
    }

    strcpy(fullPath, sharedLibName);
    if ( iSearch == 0 )
    {
        strcat(fullPath, SAPDB_DLL_EXTENSION);
    }

#ifdef WIN32

    hLib = LoadLibraryEx(fullPath, NULL, LOAD_WITH_ALTERED_SEARCH_PATH);

    result = ( hLib > (RTE_SharedLibHandle)HINSTANCE_ERROR );
    if ( !result )
    {
        DLFunction = "LoadLibraryEx";
        errorValue = GetLastError();
        hLib = 0;
    }

#else

#  if defined SUN || defined SOLARIS || defined AIX || defined OSF1 || defined NMP || defined LINUX || defined FREEBSD || ( defined HPUX && defined BIT64 )

    dlMode = RTLD_LAZY;

#    ifdef HAS_RTLD_LOCAL_UNDERMINES_EXCEPTION_HANDLING

    dlMode = dlMode | RTLD_GLOBAL;

#    else

#      if !defined OSF1

    if ( RTE_LoadSymbolsGlobal == loadMode )
    {
        dlMode = dlMode | RTLD_GLOBAL;
    } 
    else if ( RTE_LoadSymbolsLocal == loadMode )
    {
        dlMode = dlMode | RTLD_LOCAL;
    }

#      endif

#    endif

# ifdef AIX
    if ( fullPath[strlen(fullPath)-1] == ')' ) /* for member of big archive ... i.e. "libpam.a(shr_64.o)" */
    {
        dlMode |= RTLD_MEMBER;
    }
# endif

    hLib = (RTE_SharedLibHandle) dlopen (fullPath, dlMode);
    result = ( 0 != hLib );
    if ( !result )
    {
        DLFunction = "dlopen";
        errorValue = errno;
    }

#  elif defined HPUX && !defined(BIT64)

    hLib = (RTE_SharedLibHandle) shl_load (fullPath, BIND_DEFERRED, 0);
    result = ( 0 != hLib );
    if ( !result )
    {
        DLFunction = "shl_load";
        errorValue = errno;
    }

#  endif

#endif

    if ( result )
    {
        *pSharedLibHandle = hLib;
    }
    else
    {
        FillLocalErrorText (DLFunction, errorValue, errorText, maxErrorTextLengthInBytes);
    }

    return result;
}

/*---------------------------------*/

SAPDB_Bool RTESys_UnloadSharedLibrary( RTE_SharedLibHandle sharedLibHandle,
                                       SAPDB_Char        * errorText,
                                       SAPDB_Int4          maxErrorTextLengthInBytes )
{
    SAPDB_Bool result;
    SAPDB_Char const *DLFunction;
    SAPDB_Int4 errorValue;

#ifdef WIN32

    result = ( 0 != FreeLibrary(sharedLibHandle) );
    if (!result)
    {
       DLFunction = "FreeLibrary";
       errorValue = GetLastError();
    }

#else

# if defined SUN || defined SOLARIS || defined AIX || defined OSF1 || defined NMP || defined LINUX || ( defined HPUX && defined BIT64 ) || defined FREEBSD

    result = ( 0 == dlclose (sharedLibHandle) );
    if (!result)
    {
       DLFunction = "dlclose";
       errorValue = errno;
    }

# elif defined HPUX && !defined(BIT64)

    result = ( 0 == shl_unload ((shl_t) sharedLibHandle) );
    if (!result)
    {
       DLFunction = "shl_unload";
       errorValue = errno;
    }

# endif

#endif

    if ( !result )
    {
        FillLocalErrorText (DLFunction, errorValue, errorText, maxErrorTextLengthInBytes);
    }

    return result;
}

/*---------------------------------*/

SAPDB_Bool RTESys_GetProcAddressFromSharedLibrary( RTE_SharedLibHandle        sharedLibHandle,
                                                   SAPDB_Char const         * symbolicName,
                                                   SAPDB_Char               * errorText,
                                                   SAPDB_Int4                 maxErrorTextLengthInBytes,
                                                   RTE_SharedLibProcAddress * procAddress )
{
    SAPDB_Bool result;
    SAPDB_Char const *DLFunction;
    SAPDB_Int4 errorValue;

    union
    {
#ifdef WIN32
        FARPROC pV;
#else
        void *pV;
#endif
        RTE_SharedLibProcAddress p;
    } u;

    u.p = 0;

#ifdef WIN32

    u.pV = GetProcAddress( sharedLibHandle, symbolicName );
    result = ( 0 != u.p );
    if ( !result )
    {
        DLFunction = "GetProcAddress";
        errorValue = GetLastError();
    }

#else

#  if defined SUN || defined SOLARIS || defined AIX || defined OSF1 || defined NMP || defined LINUX || ( defined HPUX && defined BIT64 ) || defined FREEBSD

    u.pV = dlsym ((void *) sharedLibHandle, symbolicName);
    result = ( 0 != u.p );
    if ( !result )
    {
        DLFunction = "dlsym";
        errorValue = errno;
    }

#  elif defined HPUX && !defined(BIT64)

    result = ( 0 == shl_findsym ((shl_t *) &sharedLibHandle, symbolicName, TYPE_UNDEFINED, (void *)&u.p) );
    if ( !result )
    {
        DLFunction = "shl_findsym";
        errorValue = errno;
    }

#  endif

#endif

    if ( result )
    {
        *procAddress = u.p;
    }
    else
    {
        FillLocalErrorText (DLFunction, errorValue, errorText, maxErrorTextLengthInBytes);
    }

    return result;
}

/*===========================================================================*
 *  STATIC FUNCTION CODE                                                     *
 *===========================================================================*/

static void FillLocalErrorText(SAPDB_Char const * DLFunction,
                               SAPDB_Int4 errorValue,
                               SAPDB_Char * errorText,
                               SAPDB_Int4 maxErrorTextLengthInBytes)
{
#ifdef WIN32

    LPVOID lpMsgBuf;
    FormatMessage( FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
                   NULL,
                   errorValue,
                   MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
                   (LPTSTR) &lpMsgBuf,
                   0,
                   NULL );
    SAPDB_sprintf(errorText, maxErrorTextLengthInBytes, "%s failed:%s", DLFunction, lpMsgBuf);
    LocalFree(lpMsgBuf);

#elif defined HPUX && !defined(BIT64)

    int  i;
    for (i = 0; ErrorArray[i].szText != 0; i++)
    {
        if (ErrorArray[i].Value == errorValue)
        {
            SAPDB_sprintf(errorText, maxErrorTextLengthInBytes, "%s failed:%s", DLFunction, ErrorArray[i].szText);
            break;
        }
    }

    if (0 == ErrorArray[i].szText)
    {
        SAPDB_sprintf(errorText, maxErrorTextLengthInBytes, "%s failed:Unknown error code %d", DLFunction, errorValue);
    }

#elif defined SUN || defined SOLARIS || defined AIX || defined OSF1 || defined NMP || defined LINUX || ( defined HPUX && defined BIT64 ) || defined FREEBSD

    SAPDB_sprintf(errorText, maxErrorTextLengthInBytes, "%s failed:%s", DLFunction, dlerror() );

#endif

    errorText[maxErrorTextLengthInBytes-1] = 0;
}

/*===========================================================================*
 *  END OF CODE                                                              *
 *===========================================================================*/
