/*!**********************************************************************

  module: hwd26err.h

  -----------------------------------------------------------------------

  responsible:  Holger Bischoff
				Markus Oezgen

  special area: WebAgent

  description:  Error type

  see also:

  -----------------------------------------------------------------------

  copyright:           Copyright (c) 1998-2005 SAP AG



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

#ifndef HWD26ERR_H
#define HWD26ERR_H

/*=======================================================================
 *  INCLUDES
 *=====================================================================*/

#include <WINDOWS.H>
#include <sql.h>
#include <sqlext.h>

#include "gwd00.h"
#include "hwd06waerr.h" /* public error codes */
#include "hwd25log.h"

#if defined(__cplusplus)
extern "C" {
#endif

/*!**********************************************************************

  Chapter: twd26ErrP

  description: The twd26ErrP describes an error with error-id and two
               string arguments. 
  
************************************************************************/

typedef struct st_wa_err *twd26ErrP;

/*!**********************************************************************
  Function: wd26CreateErr

  description: Create a new error-object.

  arguments:
      
  return value: A pointer to the created error object or NULL if
                memory allocation failed. */

twd26ErrP wd26CreateErr();


/*!**********************************************************************
  Function: wd26DestroyErr

  description: Free memory of an error-object.

  arguments:
    err [in] Pointer to the error-object that should be freed.
      
  return value: void */

void wd26DestroyErr( twd26ErrP err );

    
/*!**********************************************************************
  Function: wd26CopyErr

  description: Copy error information

  arguments:
    destErr   [in] Pointer to the destination error-object.
    sourceErr [in] Pointer to the source error-object.
      
  return value: void */

void wd26CopyErr( twd26ErrP destErr, twd26ErrP sourceErr );


/*!**********************************************************************
  Function: wd26CopyMsg

  description: Copy the error message into an application buffer

  arguments:
    err    [in]  Pointer to the error-object.
    buf    [out] Pointer ti buffer for error message
    bufLen [in]  Length of *buf.
    
  return value: void */

void wd26CopyMsg( twd26ErrP err, char *buf, SAPDB_UInt4 bufLen );

    
/*!**********************************************************************
  Function: wd26GetMsg

  description: Get error message

  arguments:
    err [in] Pointer to the error-object.
      
  return value: A pointer to the message string. The message string is
                "Success." if the error has not been set. */

const char* wd26GetMsg( twd26ErrP err );


/*!**********************************************************************
  Function: wd26GetId

  description: Get error id.

  arguments:
    err [in] Pointer to the error-object.
      
  return value: The id of the error or ERR_NO_ERROR_WD26 if error has
                not been set. Valid error-ids are listed below. */

SAPDB_UInt4 wd26GetId( twd26ErrP err );

/*!**********************************************************************
  Function: wd26GetNativeOdbcErr

  description: Get the native odbc error id. This the id returned by SAB DB.

  arguments:
    err [in] Pointer to the error-object.
      
  return value: The id of the error or 0 if err is not a ODBC error */

SAPDB_Int4 wd26GetNativeOdbcErr( twd26ErrP err );
    
/*!**********************************************************************
  Function: wd26SetErr

  description: Set arguments of the error.

  arguments:
    err    [inout] Pointer to the error-object
    errId  [in]    The error-number (see below).
    arg1   [in]    An optional argument for the error (filename etc.).
                   Is set to NULL if no argument is available.
    arg2   [in]    An optional second argument for the error. Maby NULL.    
      
  return value: void

  NOTE: The strings in arg1 and arg2 are copied. */

void wd26SetErr( twd26ErrP   err,
                 SAPDB_Int2  errId,
                 const char *arg1,
                 const char *arg2 );


/*!**********************************************************************
  Function: wd26SetOdbcErr

  description: Sets the error-arguments for ODBC-errors.

  arguments:
    err    [inout] Pointer to the error-object
    henv   [in]    ODBC environment-handle
    hdbc   [in]    ODBC connection-handle (maby SQL_NULL_HDBC)
    hstmt  [in]    ODBC statement-handle (maby SQL_NULL_HSTMT)
      
  return value: void

  This function uses SQLError to retrieve the ODBC error-message. */

void wd26SetOdbcError( twd26ErrP  err,
                       RETCODE    sqlRetcode,
                       SQLHENV    henv,
                       SQLHDBC    hdbc,
                       SQLHSTMT   hstmt );


/*!**********************************************************************
  Function: wd26ResetErr

  description: Initializes the error arguments. The error-id is set
               to ERR_NO_ERROR and the two argument string are set
               to "".

  arguments:
    err    [inout] Pointer to the error-object
      
  return value: void */

void wd26ResetErr( twd26ErrP err );


/*!**********************************************************************
  Function: wd26LogErr

  description: Write error-message to log file.

  arguments:
    err    [inout] Pointer to the error-object
    log    [in]    The log-object that is used to write the message.
    
  return value: void */

void wd26LogErr( twd26ErrP err, twd25LogP log );


/*!**********************************************************************
  Function: wd26PrintErr

  description: Write error-message to stdout.

  arguments:
    err    [inout] Pointer to the error-object
    
  return value: void */

void wd26PrintErr( twd26ErrP err );

/* general errors */
#define ERR_NO_ERROR_WD26 sapdbwa_Error_NoError		/* this error is no error :-)	*/
#define ERR_MEMORY_WD26   sapdbwa_Error_Memory		/* Memory allocation failure	*/
#define ERR_ODBC_WD26     sapdbwa_Error_ODBC		/* ODBC-error					*/
#define ERR_INTERNAL_WD26 sapdbwa_Error_Internal	/* Unexpected internal error	*/
													/* == a bug in the WebAgent		*/
#define ERR_UNKNOWN_WD26  sapdbwa_Error_Unknown
#define ERR_INIT_MT_WD26  sapdbwa_Error_InitMT		/* init multithreading failed	*/

/* WebAgent errors */
#define ERR_INI_FILE_OPEN_WD26         10 /* Cannot open ini file        */
#define ERR_NON_UNIQUE_WD26            12 /* non unique name for session-
                                           * pool or user dll            */
#define ERR_NO_USERDLLS_WD26           13 /* No User-Dll spcified        */
                                          /* param in ini file too long  */
#define ERR_PARAM_TOO_LONG_WD26        sapdbwa_Error_ParamTooLong
                                          /* Poolsize is 0 or too big    */
#define ERR_POOLSIZE_WD26              sapdbwa_Error_PoolSize
#define ERR_NO_USER_WD26               16 /* No username specified       */
#define ERR_NO_PASSWORD_WD26           17 /* no password specified       */
#define ERR_NO_PREFIX_WD26             18 /* no path prefix for user dll */
#define ERR_NO_USERDLL_WD26            19 /* user dll filename not spec. */
#define ERR_NO_SUCH_POOL_WD26          20 /* Sessionpool with given name
                                           * not found                   */
#define ERR_USERDLL_LOAD_FAILED_WD26   21
#define ERR_USERDLL_UNLOAD_FAILED_WD26 22
#define ERR_NO_SUCH_FUNCTION_WD26      23
#define ERR_NAME_TOO_LONG_WD26         24
#define ERR_HTTP_API_WD26              25
#define ERR_SERVICE_FUNC_ERROR_WD26    26
#define ERR_NO_DATASOURCE_WD26         27 /* no datasource specified for
                                           * userdll                     */
#define ERR_NO_INIT_FUNC_WD26          28
#define ERR_NO_EXIT_FUNC_WD26          29
#define ERR_NO_SERVICE_FUNC_WD26       30
#define ERR_INIT_FUNC_ERROR_WD26       31
#define ERR_EXIT_FUNC_ERROR_WD26       32
#define ERR_EXCEPTION_WD26             33
#define ERR_USERDLL_NOT_INIT_WD26      34
#define ERR_INVALID_CONNECT_ATTR_WD26  35
#define ERR_DBC_NOT_ACTIVE_WD26        36
#define ERR_CANT_SET_COOKIE_WD26       37
#define ERR_WEBSESSION_LOCKED_WD26     sapdbwa_Error_WebSessionLocked
#define ERR_NO_WEBSESSION_WD26         sapdbwa_Error_NoWebSession
#define ERR_WEBSESSION_TIMEOUT_WD26    sapdbwa_Error_WebSessionTimeout
#define ERR_WEBSESSIONS_DISABLED_WD26  sapdbwa_Error_WebSessionsDisabled
#define ERR_SQLCLASS_WD26              sapdbwa_Error_SQLClass
#define ERR_UNKNOWN_POOLTYPE_WD26      43
#define ERR_UNKNOWN_ISOLATION_WD26     44
#define ERR_NO_SERVERDB_WD26           45
#define ERR_INVALID_DATE_FORMAT_WD26   sapdbwa_Error_InvalidDateFormat
#define ERR_NO_CLASSID_WD26            47
#define ERR_BUFFER_TOO_SMALL_WD26      48
#define ERR_NOT_CONNECTED_WD26         49
#define ERR_READ_REGISTRY_WD26         50

/* New errors in SAP DB 73 */
#define ERR_WRITE_REGISTRY_WD26				60
#define ERR_MEMORY_LEAK_ASSUMPTION_WD26		61
#define ERR_NO_SERVICE_NAME_WD26			62
#define ERR_NO_LIBRARY_WD26					63
#define ERR_NO_SSL_URL_SPECIFIED_WD26		64
#define ERR_HEADERS_ALREADY_SENT_WD26		sapdbwa_Error_HeadersAlreadySent
#define ERR_SESSION_POOL_LOAD_FAILED_WD26	66
#define ERR_SESSION_POOL_UNLOAD_FAILED_WD26	67
#define ERR_OPEN_REGISTRY_WD26				68
    
/* DBFS error codes */
#define ERR_CANT_GET_ODBC_HANDLES_WD26 sapdbwa_Error_CannotGetODBCHandles
#define ERR_CANT_GET_DBCONNECTION_WD26 sapdbwa_Error_CannotGetDBConnection
#define ERR_INVALID_FILE_DESC_WD26     sapdbwa_Error_InvalidFileDescription
#define ERR_FILE_NOT_FOUND_WD26        sapdbwa_Error_FileNotFound
#define ERR_FILE_OPEN_WD26             sapdbwa_Error_FileOpen
#define ERR_WRITE_ERROR_WD26           sapdbwa_Error_WriteError
#define ERR_DIR_NOT_FOUND_WD26         sapdbwa_Error_DirectoryNotFound
#define ERR_NOT_DIR_WD26               sapdbwa_Error_NotDirectory
#define ERR_NOT_FILE_WD26              sapdbwa_Error_NotFile
#define ERR_DIR_EXISTS_WD26            sapdbwa_Error_DirectoryExists
#define ERR_DIR_NOT_EMPTY_WD26         sapdbwa_Error_DirectoryNotEmpty
#define ERR_FILE_OR_DIR_NOT_FOUND_WD26 sapdbwa_Error_FileOrDirectoryNotFound
#define ERR_FILE_OR_DIR_EXISTS_WD26    sapdbwa_Error_FileOrDirectoryExists
#define ERR_INVALID_FILENAME_WD26      sapdbwa_Error_InvalidFilename
#define ERR_FILENAME_TOO_LONG_WD26     sapdbwa_Error_FilenameTooLong
#define ERR_DIFFERENT_PATH_NAMES_WD26  sapdbwa_Error_DifferentPathNames
#define ERR_COMPRESS_WD26              sapdbwa_Error_Compress
#define ERR_UNCOMPRESS_WD26            sapdbwa_Error_Uncompress
#define ERR_SUBDIR_NOT_FOUND_WD26      sapdbwa_Error_SubDirectoryNotFound
#define ERR_DESTDIR_IS_SUBDIR_WD26     sapdbwa_Error_DestinationDirectoryIsSubDirectory
#define ERR_PATH_TOO_LONG_WD26         sapdbwa_Error_PathTooLong
#define ERR_NO_MORE_ENTRIES_WD26       sapdbwa_Error_NoMoreEntries
#define ERR_LINK_TARGET_NOT_FOUND_WD26 sapdbwa_Error_LinkTargetNotFound
#define ERR_MAX_POOL_SIZE_REACHED_WD26 sapdbwa_Error_MaxPoolSizeReached
    
#define ERR_MSG_MEMORY_WD26 "Memory allocation failure!\n"
    
/*!**********************************************************************

  EndChapter: twd26ErrP
  
************************************************************************/

#if defined(__cplusplus)
} /* extern "C" */
#endif
    
#endif

