/*!**********************************************************************

  module: gwd00.h

  -----------------------------------------------------------------------

  
  responsible:	Holger Bischoff
				Markus Özgen

  special area: WebAgent

  description:  Common constants and macros for webagent

  see also:

  -----------------------------------------------------------------------

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





************************************************************************/

#ifndef GWD00_H
#define GWD00_H

/*=======================================================================
 *  INCLUDES
 *=====================================================================*/

#include <assert.h>
#include <limits.h>

#if defined(__cplusplus)
#include "heo02.h"
#else
#include "heo102.h"
#endif

#include "hsp77.h"

#include "hwd01wadef.h"						/* Public WebAgent Types and Defines	*/
#include "SAPDB/SAPDBCommon/SAPDB_Types.h"	/* new types							*/

#if defined(__cplusplus)
extern "C" {
#endif

#ifndef WIN32
#define WDUNIX
#endif

/* Get swap info */
#define WD_IS_UCS2_SWAPED   sp77encodingUCS2Native == sp77encodingUCS2Swapped

/* To enable memory leak searching */
/* #define sapdbwa_MEMCHECK */

#ifndef sapdbwa_MEMCHECK
#define sapdbwa_SQLALLOCAT		sqlallocat
#define sapdbwa_SQLREALLOCAT	sqlreallocat
#define sapdbwa_SQLFREE			sqlfree
#else
#include "hwd10mem.h"

#define sapdbwa_SQLALLOCAT		sapdbwa_allocat
#define sapdbwa_SQLREALLOCAT	sapdbwa_reallocat
#define sapdbwa_SQLFREE			sapdbwa_free
#endif
    
#ifdef WDUNIX
#define stricmp(s1,s2) strcasecmp(s1,s2)
#endif

#define sapdbwa_MAX(a,b)  (((a) > (b)) ? (a) : (b))
#define sapdbwa_MIN(a,b)  (((a) < (b)) ? (a) : (b))
    
typedef void *twd00ServerHandle;

typedef struct st_wa_http_rep *twd22HttpReplyP;

typedef SAPDB_ULong	(*twd00ReadBodyFunc)( twd00ServerHandle   handle,
                                          char               *buff,
                                          SAPDB_ULong         len );

typedef const char* (*twd00GetHeaderFunc)( twd00ServerHandle   handle,
                                           const char         *fieldName );

typedef tsp00_Bool (*twd00IsSecureFunc)( twd00ServerHandle   handle );

typedef tsp00_Bool (*twd00SendHeaderFunc)( twd00ServerHandle   handle,
                                           twd22HttpReplyP     rep );


typedef SAPDB_UInt4 (*twd00SendBodyFunc)( twd00ServerHandle   handle,
                                          const void         *buff,
                                          SAPDB_UInt4         len );


/* Directory settings */

#ifdef WDUNIX

#define OS_PATH_SEP_WD00       '/'
#define WRNG_OS_PATH_SEP_WD00 '\\'

#else    

#define OS_PATH_SEP_WD00       '\\'
#define WRNG_OS_PATH_SEP_WD00  '/'

#endif

#define URLPATH_SEP_WD00       '/'
#define WRNG_URLPATH_SEP_WD00  '\\'

/* filenames */

#define MAX_FILENAME_LEN_WD00        1024 /* max. absolute path length */

#define MAX_FILEBASENAME_LEN_WD00    244  /* max. length of base name */

#define MAX_INI_FILENAME_LEN_WD00    MAX_FILENAME_LEN_WD00

#define MAX_FILE_BUF_SIZE_WD00		4096

/* ini parameter */
#define MAX_NAME_LEN_WD00           32

#define MAX_INI_PAR_LEN_WD00        100

#define MAX_ERRORTEXT_LEN_WD00      500

#define MAX_DLL_NAME_LEN_WD00       256

#define MAX_FUNC_NAME_LEN_WD00      64

#define MAX_SECTION_LEN_WD00		512

/* odbc */

#define DB_MAX_USERNAME_LEN_WD00    64 /* max length of db-username */
    
#define MAX_SQLSTR_LEN_WD00         2048

#define ODBC_ERROR_TEXT_LEN_WD00    300

#define MAX_CLASSID_LEN_WD00        50
    
/* ****** assert-macro ******** */

#ifdef DEBUG
#define sapdbwa_ASSERT(expression) assert(expression)
#define sapdbwa_ASSERT_PTR(ptr) assert(ptr!=NULL)
#define sapdbwa_TRACE(m) printf(m)
#else
#define sapdbwa_ASSERT(expression)
#define sapdbwa_ASSERT_PTR(ptr)
#define sapdbwa_TRACE(m)
#endif

#define wd00Success(ret) \
     ((ret == SQL_SUCCESS) || (ret == SQL_SUCCESS_WITH_INFO))

#define SID_COOKIE_WD00 "SID" /* cookie name for session id */

#if defined(__cplusplus)
} /* extern "C" */
#endif
    
#endif
