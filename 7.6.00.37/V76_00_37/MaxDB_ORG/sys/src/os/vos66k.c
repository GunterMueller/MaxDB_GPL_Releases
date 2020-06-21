/*!
  @file           vos66k.c
  @author         RaymondR
  @brief          Virtual File ( Kernel Part )
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

\endif
*/




//
//  INCLUDE FILES
//
#include "gos00.h"
#include "heo00.h"
#include "heo46.h"
#include "geo007_1.h"
#include "geo007_2.h"
#include "gos00k.h"
#include "hos06.h"
#include "gos44.h"
#include "geo002.h"

//
//  DEFINES
//

#define MOD__  "VOS66KC : "
#define MF__   MOD__"UNDEFINED"


#if defined (KERNEL)
 #define  SECURITY_ATTR        &kgs.AdminSA
#else
 #define  SECURITY_ATTR        NULL
#endif

#define MAX_HANDLES		          256
#define FILE_CREATED            1
#define FILE_EXISTED            2

//
//  MACROS
//
#define  ENTER_CRIT_SEC()                                                    \
           if (!CritSecInitialized) { CritSecInitialized = TRUE;             \
             InitializeCriticalSection(&CritSec); }                          \
           EnterCriticalSection(&CritSec); CritSecCount++;

#define  EXIT_CRIT_SEC()                                                     \
           if (CritSecCount) { CritSecCount--;                               \
             LeaveCriticalSection(&CritSec); }

#define  DELETE_CRIT_SEC()                                                   \
            { DeleteCriticalSection(&CritSec); \
              CritSecInitialized = FALSE; }


//
//  LOCAL TYPE AND STRUCT DEFINITIONS
//


//
//  EXTERNAL VARIABLES
//


//
//  EXPORTED VARIABLES
//


//
//  LOCAL VARIABLES
//
static CRITICAL_SECTION CritSec;
static BOOL             CritSecInitialized   = FALSE;
static INT              CritSecCount         = 0;
static HANDLE           Handles[MAX_HANDLES];

//
//  LOCAL FUNCTION PROTOTYPES
//
static VOID sql66c_io_write            ( HANDLE              hFileHandle,
                                         PVOID               pBlockPtr,
                                         ULONG               ulBlockLength,
                                         tsp00_VfReturn       *lError,
                                         tsp00_ErrText         pcErrText);

static BOOL sql66c_handle_known_errors ( PSZ                 pszName,
                                         LONG                rc,
                                         PCHAR               pcErrText);

static VOID sql66c_put_desc            ( HANDLE              hFileHandle,
                                         tsp00_VfReturn       *lError,
                                         tsp00_ErrText         pcErrText);

static  tsp00_Int4 sql66c_store_handle   ( HANDLE              hFileHandle );
static  HANDLE   sql66c_get_handle     ( tsp00_Int4            hfFileHandle );
static  void     sql66c_leave_handle   ( tsp00_Int4            hfFileHandle );

//
// ========================== GLOBAL FUNCTIONS ================================
//


/*
 * Opens the file for overwriting
 */

VOID  sql66kc_open ( PSZ                 pszHostFileName,
                     tsp00_Bool          no_magic_record,
                     tsp00_Int4            *hfFileHandle,
                     tsp00_VfReturn       *lError,
                     tsp00_ErrText         pcErrText)
  {
  #undef  MF__
  #define MF__ MOD__"sql66kc_open"
  APIRET       rc = NO_ERROR;
  PATHNAME     szPhysFileName;
  HANDLE       hFileHandle;


  DBGIN;

  sql44c_subst_log_parts ( szPhysFileName, pszHostFileName );

  hFileHandle = CreateFile( szPhysFileName,
                            GENERIC_READ            |
                            GENERIC_WRITE,
                            FILE_SHARE_READ,
                            SECURITY_ATTR,
                            CREATE_ALWAYS,
                            FILE_ATTRIBUTE_NORMAL,
                            NULL);

  if ( hFileHandle == INVALID_HANDLE_VALUE )
    rc = GetLastError();
  else
    {
#	  if defined (_WIN64)
     *hfFileHandle = sql66c_store_handle( hFileHandle );

     if ( *hfFileHandle == 0 )
       {
       rc = GetLastError();
       CLOSE_FILE( hFileHandle );
       }
#	  else
	   *hfFileHandle = (tsp00_Int4)hFileHandle;
#	  endif
    }

  if ( rc != NO_ERROR )
    {
    *lError       = VF_NOTOK;
    *hfFileHandle = (tsp00_Int4)-1;

    if ( sql66c_handle_known_errors ( pszHostFileName,
                                      rc , pcErrText ) == FALSE )
      {
      DBG1 (( MF__, "Cannot open file '%s', rc = %d", pszHostFileName, rc ));
      MSGD (( ERR_OPEN, pszHostFileName, rc                               ));
      sql46c_build_error_string ( pcErrText, ERRMSG_VF_OPEN,  rc           );
      }
    }
  else
    {
    *lError = VF_OK;

    if ( !no_magic_record )
      sql66c_put_desc ( hFileHandle, lError, pcErrText);
    }

  DBGOUT;
  return ;
  }


/*------------------------------*/

/*
 * Opens the file for appending
 */

VOID  sql66kc_append_open ( PSZ                 pszHostFileName,
                            tsp00_Int4            *hfFileHandle,
                            tsp00_VfReturn       *lError,
                            tsp00_ErrText         pcErrText)
  {
  #undef  MF__
  #define MF__ MOD__"sql66kc_append_open"
  ULONG        Action;
  APIRET       rc = NO_ERROR;
  PATHNAME     szPhysFileName;
  HANDLE       hFileHandle;


  DBGIN;

  sql44c_subst_log_parts ( szPhysFileName, pszHostFileName );

  hFileHandle = CreateFile( szPhysFileName,
                            GENERIC_READ            |
                            GENERIC_WRITE,
                            FILE_SHARE_READ,
                            SECURITY_ATTR,
                            OPEN_EXISTING,
                            FILE_ATTRIBUTE_NORMAL,
                            NULL);

  if ( hFileHandle == INVALID_HANDLE_VALUE )
    { 
	  hFileHandle = CreateFile( szPhysFileName,
                              GENERIC_READ          |
                              GENERIC_WRITE,
                              FILE_SHARE_READ,
                              SECURITY_ATTR,
                              CREATE_NEW,
                              FILE_ATTRIBUTE_NORMAL,
                              NULL);

	  if ( hFileHandle == INVALID_HANDLE_VALUE )
  		rc = GetLastError();
 	  else
		  Action = FILE_CREATED;
    }
  else
    Action = FILE_EXISTED;

  if ( rc == NO_ERROR )
    {
    if ( Action == FILE_EXISTED ) 
      { 
   		ULONG ulEndOfFilePosition;

      rc = SET_FILE_PTR ( hFileHandle, 0, FILE_END, 
                          &ulEndOfFilePosition, NULL ) ;
      }

#	  if defined (_WIN64)
     *hfFileHandle = sql66c_store_handle( hFileHandle );

     if ( *hfFileHandle == 0 )
       {
       rc = GetLastError();
       CLOSE_FILE( hFileHandle );
       }
#	  else
	   *hfFileHandle = (tsp00_Int4)hFileHandle;
#	  endif
    }

  if ( rc != NO_ERROR )
    {
    *lError       = VF_NOTOK;
	  *hfFileHandle = (tsp00_Int4)-1;

    if ( sql66c_handle_known_errors ( pszHostFileName,
                                      rc , pcErrText ) == FALSE )
      {
      DBG1 (( MF__, "Cannot open file '%s', rc = %d", pszHostFileName, rc ));
      MSGD (( ERR_OPEN, pszHostFileName, rc                               ));
      sql46c_build_error_string ( pcErrText, ERRMSG_VF_OPEN,  rc           );
      }
    }
  else
    *lError = VF_OK;

  DBGOUT;
  return ;
  }


/*------------------------------*/


VOID sql66kc_close ( tsp00_Int4            hfFileHandle,
                     tsp00_VfReturn       *lError,
                     tsp00_ErrText         pcErrText)
  {
  #undef  MF__
  #define MF__ MOD__"sql06kc_close"

  APIRET       rc = NO_ERROR;
  HANDLE       hFileHandle;

  DBGIN;

#	if defined (_WIN64)
   hFileHandle = sql66c_get_handle( hfFileHandle );
#	else
	 hFileHandle = (HANDLE)hfFileHandle;
#	endif

  // --- close file
  rc = CLOSE_FILE( hFileHandle );

  if (rc != NO_ERROR)
    {
    sql46c_build_error_string ( pcErrText, ERRMSG_VF_CLOSE, rc );
    *lError = VF_NOTOK;
    }
  else
    *lError = VF_OK;

#	if defined (_WIN64)
   sql66c_leave_handle( hfFileHandle );
#	endif

  DBGOUT;
  return ;
  }

/*------------------------------*/


VOID sql66kc_write ( tsp00_Int4            hfFileHandle,
                     char                *Buf ,
                     tsp00_Int4            BufLen,
                     tsp00_VfReturn       *lError,
                     tsp00_ErrText         pcErrText)
  {
  #undef  MF__
  #define MF__ MOD__"sql06kc_close"

  APIRET       rc = NO_ERROR;
  HANDLE       hFileHandle;

  DBGIN;

#	if defined (_WIN64)
   hFileHandle = sql66c_get_handle( hfFileHandle );
#	else
	 hFileHandle = (HANDLE)hfFileHandle;
#	endif

  sql66c_io_write ( hFileHandle, Buf, (ULONG )BufLen, lError, pcErrText);

  DBGOUT;

  return ;
  }


//
// ========================== LOCAL FUNCTIONS =================================
//

/*
 * The description is put (3. and 4.byte);
 *
 * If record length is 4kB the first block is written;
 */

static VOID sql66c_put_desc ( HANDLE              hFileHandle,
                              tsp00_VfReturn       *lError,
                              tsp00_ErrText         pcErrText) 
  {
  #undef  MF__
  #define MF__ MOD__"sql66c_put_desc"
  char *         MagicRec;

  DBGIN;

  MagicRec = (char*)alloca( XPARAM(ulDBPageSize));

  memset ( MagicRec, 0, XPARAM(ulDBPageSize) ) ;
  MagicRec[2] = (char) (XPARAM(ulDBPageSize) >> 8);
  MagicRec[3] = (char)  XPARAM(ulDBPageSize);

  sql66c_io_write ( hFileHandle, MagicRec, XPARAM(ulDBPageSize), lError, pcErrText);

  DBGOUT;

  return ;
  }

/*------------------------------*/

static VOID sql66c_io_write ( HANDLE              hFileHandle,
                              PVOID               pBlockPtr,
                              ULONG               ulBlockLength,
                              tsp00_VfReturn       *lError,
                              tsp00_ErrText         pcErrText) 
  {
  #undef  MF__
  #define MF__ MOD__"sql66c_io_write"
  APIRET       rc       = NO_ERROR;
  ULONG        ulBytesWritten ;

  rc = WRITE_FILE ( hFileHandle, pBlockPtr, ulBlockLength, &ulBytesWritten );

  DBG3 (( MF__, "ulBytesWritten = %d\n", ulBytesWritten ));

  // --- write failed ?
  if ( (rc != NO_ERROR) || (ulBytesWritten != ulBlockLength) )
    {
    *lError = VF_NOTOK;
    DBG1 (( MF__, ERRMSG_VF_WRITE ));
    MSGD (( ERR_WRITE, rc         ));
    sql46c_build_error_string ( pcErrText, ERRMSG_VF_WRITE, rc );
    }
  else
    *lError = VF_OK;

  DBGOUT;
  return ;
  }

/*------------------------------*/

static BOOL sql66c_handle_known_errors ( PSZ                 pszName,
                                         LONG                rc,
                                         PCHAR               pcErrText)
  {
  #undef  MF__
  #define MF__ MOD__"sql66c_handle_known_errors"

  DBGIN;


  switch ( rc )
    {
    case ERROR_NOT_READY :
    case ERROR_BUSY :
      DBG1 (( MF__, ERRMSG_VF_DEVICE_NOT_READY  ));
      MSGD (( ERR_DEVICE_NOT_READY, pszName ));
      sql46c_build_error_string ( pcErrText, ERRMSG_VF_DEVICE_NOT_READY, 0 );
      return ( TRUE );

    case ERROR_SHARING_VIOLATION:
      DBG1 (( MF__, ERRMSG_VF_FILE_SHARING_VIOLATION ));
      MSGD (( ERR_FILE_SHARING_VIOLATION, pszName ));
      sql46c_build_error_string ( pcErrText, ERRMSG_VF_FILE_SHARING_VIOLATION, 0 );
      return ( TRUE );

    case ERROR_DISK_FULL:
      DBG1 (( MF__, ERRMSG_VF_DISK_FULL ));
      MSGD (( ERR_DISK_FULL, pszName ));
      sql46c_build_error_string ( pcErrText, ERRMSG_VF_DISK_FULL, 0 );
      return ( TRUE );

    case ERROR_PATH_NOT_FOUND:
      DBG1 (( MF__, ERRMSG_VF_WRONG_PATH ));
      MSGD (( ERR_WRONG_PATH, pszName ));
      sql46c_build_error_string ( pcErrText, ERRMSG_VF_WRONG_PATH, 0 );
      return ( TRUE );

    case ERROR_FILE_NOT_FOUND:
      DBG1 (( MF__, ERRMSG_VF_WRONG_FILE_DEVICE_NAME ));
      MSGD (( ERR_WRONG_FILE_DEVICE_NAME, pszName ));
      sql46c_build_error_string ( pcErrText, ERRMSG_VF_WRONG_FILE_DEVICE_NAME, 0 );
      return ( TRUE );

    case ERROR_WRITE_PROTECT:
      DBG1 (( MF__, ERRMSG_VF_MEDIA_WRITE_PROTECTED ));
      MSGD (( ERR_MEDIA_WRITE_PROTECTED, pszName ));
      sql46c_build_error_string ( pcErrText, ERRMSG_VF_MEDIA_WRITE_PROTECTED, 0 );
      return ( TRUE );

    case ERROR_ACCESS_DENIED :
      DBG1 (( MF__, ERRMSG_VF_ACCESS_DENIED ));
      MSGD (( ERR_ACCESS_DENIED_ON_XX, pszName ));
      sql46c_build_error_string ( pcErrText, ERRMSG_VF_ACCESS_DENIED, 0 );
      return ( TRUE );

    }

  DBGOUT;
  return (FALSE);
  }

/*------------------------------*/


#if defined (_WIN64)

static  tsp00_Int4 sql66c_store_handle( HANDLE     hFileHandle )
  {
# undef  MF__
# define MF__ MOD__"sql66c_store_handle"
  
  tsp00_Int4				 FileHandle;
  static tsp00_Int4  LastHandle = 0;
  
  DBGIN;

  ENTER_CRIT_SEC();
    
  for ( FileHandle = LastHandle + 1; ; FileHandle++ )
    {
    if ( FileHandle == MAX_HANDLES )
      FileHandle = 0;

    if ( Handles[FileHandle] == 0 )
      {
      LastHandle            = FileHandle;
      Handles[FileHandle] = hFileHandle;
      break;
      }

    if ( FileHandle == LastHandle )
      {
      SetLastError (ERROR_NO_SYSTEM_RESOURCES);
      FileHandle = (tsp00_Int4)-1;
      break;
      }
    }
  EXIT_CRIT_SEC();

  DBGOUT;

  return FileHandle + 1;
  }

/*------------------------------*/

static  HANDLE sql66c_get_handle( tsp00_Int4   hfFileHandle )
  {
# undef  MF__
# define MF__ MOD__"sql66c_get_handle"

  DBGPAS;

  if (( hfFileHandle > 0 ) && ( hfFileHandle <= MAX_HANDLES ))
    return Handles[hfFileHandle - 1];
  
  return INVALID_HANDLE_VALUE;
  }


/*------------------------------*/

static  void sql66c_leave_handle( tsp00_Int4   hfFileHandle )
  {
# undef  MF__
# define MF__ MOD__"sql66c_leave_handle"

  DBGPAS;

  if (( hfFileHandle > 0 ) && ( hfFileHandle <= MAX_HANDLES ))
    Handles[hfFileHandle - 1] = 0;
  
  return;
  }

#endif
//
// =============================== END ========================================
//
