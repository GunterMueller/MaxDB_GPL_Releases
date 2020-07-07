/*!**********************************************************************

  module: RegUpd_Log.

  -----------------------------------------------------------------------

  responsible:  Markus Oezgen

  special area: Registry

  description:  This component updates registry / ini file entries
                in a managed way.
                http://pts:1080/webpts?wptsdetail=yes&ErrorType=1&ErrorID=1122610

  see also:

  -----------------------------------------------------------------------

                         Copyright (c) 2002-2005 SAP AG







    ========== licence begin  GPL
    Copyright (c) 2002-2005 SAP AG

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

/***********************************************************************

	Includes

 ***********************************************************************/

#ifdef WIN32
#include <windows.h>
#endif

#include <stdio.h>
#include <assert.h>
#include <time.h>

#ifdef WIN32
    #include <direct.h>
#else
    #include <unistd.h>
#endif

/* <-- Crash handling unix */
#ifndef WIN32
    #include <signal.h>
    #include <unistd.h>
    #include <stdlib.h>
    #if defined(LINUX)
        #include <linux/version.h>
    #endif
#endif
/* Crash handling unix --> */

#include "heo06.h"
#include "heo670.h"

#include "SAPDB/SAPDBCommon/SAPDB_Types.h"

#include "geo572.h"

/***********************************************************************

	Defines

 ***********************************************************************/

/*---------------------------------------------------------------------------*/

#ifdef WIN32
#if defined(__cplusplus)
extern "C" {
#endif
    DWORD DumpException( LPEXCEPTION_POINTERS ec );
#if defined(__cplusplus)
}
#endif

    /*
    #define REGUPD_TRY
    #define REGUPD_CATCH
    */
    #define REGUPD_TRY      \
       __try
    #define REGUPD_CATCH    \
       __except (DumpException(GetExceptionInformation())) {;}
#else
    #define REGUPD_TRY
    #define REGUPD_CATCH

#if defined(__cplusplus)
extern "C" {
#endif
    void SetSignalHandler();
#if defined(__cplusplus)
}
#endif
#endif

/*---------------------------------------------------------------------------*/

#define MAX_FILENAME_LEN        1024
#define MAX_ERR_TEXT_LEN        1024
#define MAX_ARGUMENT_NAME_LEN   128
#define MAX_ARGUMENT_VALUE_LEN  1024

#define MAX_SECTION_PATH_LEN    1024
#define MAX_SECTION_LEN         256
#define MAX_KEY_LEN             256
#define MAX_VALUE_LEN           1024

/*---------------------------------------------------------------------------*/

#define APP_NAME                "Registry Update"

/*---------------------------------------------------------------------------*/

#ifdef WIN32
    #define DIRPATH_SEP '\\'
#else
    #define DIRPATH_SEP '/'
#endif

/***********************************************************************

	Structs & Types

 ***********************************************************************/

/*---------------------------------------------------------------------------*/

/***********************************************************************

	Prototypes

 ***********************************************************************/

#if defined(__cplusplus)
extern "C" {
#endif
SAPDB_Bool GetExecDirectory( char           *cmdArg,
                             SAPDB_UInt2     size      );
#if defined(__cplusplus)
}
#endif

/***********************************************************************

	End

 ***********************************************************************/
