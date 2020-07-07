/*!**********************************************************************

  module: vwd26err.c

  -----------------------------------------------------------------------

  responsible:  Holger Bischoff
				Markus Oezgen

  special area: WebAgent

  description:  Error type

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

/*=======================================================================
 *  INCLUDES
 *=====================================================================*/

#include <stdio.h>
#include <string.h>

#include "hwd26err.h"
#include "hwd03waapi.h" /* prototypes for WA... functions */

#include "hsp81.h"
#include "hsp83.h"

#include "SAPDBCommon/SAPDB_string.h"

/*!**********************************************************************

  prototypes for internal functions

************************************************************************/

const char* wd26_GetComprErrStr( sapdbwa_Int4 comprErr );


char* wd26_GetErrMsg( sapdbwa_Int2 errId );

void wd26_SetOdbcErrorMessage( twd26ErrP  err,
                               SQLHENV    henv,
                               SQLHDBC    hdbc,
                               SQLHSTMT   hstmt );

/*!**********************************************************************

  Chapter: twd26ErrP

************************************************************************/

#define ERR_ARG_LEN 1024 /* MAX_FILENAME_LEN_WD00+1 */

#define ERR_MSG_NO_ERROR "Success."
/* general errors */
#define ERR_MSG_MEMORY "Memory allocation failure"
#define ERR_MSG_ODBC   "ODBC-Error: %s  return-code: %s"
#define ERR_MSG_INTERNAL "Internal runtime-error: %s"
#define ERR_MSG_UNKNOWN "Message not available."
#define ERR_MSG_INIT_MT "Init multithreading failed: %s"

/* WebAgent errors */
#define ERR_MSG_INI_FILE_OPEN "Cannot open ini file %s."
#define ERR_MSG_NON_UNIQUE "Non unique name %s."
#define ERR_MSG_NO_USERDLLS "No user-dll specified."
#define ERR_MSG_PARAM_TOO_LONG "One parameter for %s is too long."
#define ERR_MSG_POOLSIZE "Poolsize for %s out of range."
#define ERR_MSG_NO_USER "No user specified for %s."
#define ERR_MSG_NO_PASSWORD "No password specified for %s."
#define ERR_MSG_NO_PREFIX "No path prefix specified for %s."
#define ERR_MSG_NO_USERDLL "No user-dll specified for %s."
#define ERR_MSG_NO_SUCH_POOL "No such session pool: %s."
#define ERR_MSG_USERDLL_LOAD_FAILED "Loading of user-dll %s failed. %s"
#define ERR_MSG_USERDLL__UNLOAD_FAILED "Unload of user-dll %s failed. %s"
#define ERR_MSG_NO_SUCH_FUNCTION "Function %s not found in %s"
#define ERR_MSG_NAME_TOO_LONG "The name %s is too long."
#define ERR_MSG_HTTP_API "Invalid HTTP-API function."
#define ERR_MSG_SERVICE_FUNC_ERROR "The service-function in user-dll %s returned sapdbwa_RTC_ERROR."
#define ERR_MSG_NO_DATASOURCE "No datasource or driver specified for %s."
#define ERR_MSG_NO_INIT_FUNC "No InitFunc specified for user-dll %s."
#define ERR_MSG_NO_EXIT_FUNC "No ExitFunc specified for user-dll %s."
#define ERR_MSG_NO_SERVICE_FUNC "No ServiceFunc specified for user-dll %s."
#define ERR_MSG_INIT_FUNC_ERROR "The init-function in user-dll %s returned sapdbwa_RTC_ERROR."
#define ERR_MSG_EXIT_FUNC_ERROR "The exit-function in user-dll %s returned sapdbwa_RTC_ERROR."
#define ERR_MSG_EXCEPTION "The %s-function in user-dll %s throwed an exception."
#define ERR_MSG_USERDLL_NOT_INIT "%s-function: The user-dll %s is not initialized."
#define ERR_MSG_INVALID_CONNECT_ATTR "Setting of connect attribute not allowed."
#define ERR_MSG_DBC_NOT_ACTIVE "DBC is not connected."
#define ERR_MSG_CANT_SET_COOKIE "Can't set Cookie %s."
#define ERR_MSG_WEBSESSION_LOCKED "The web-session is currently locked by another thread."
#define ERR_MSG_NO_WEBSESSION "No web-session exists."
#define ERR_MSG_WEBSESSION_TIMEOUT "The websession is timed out."
#define ERR_MSG_WEBSESSIONS_DISABLED "WebSessions are disabled for user dll %s."
#define ERR_MSG_SQLCLASS "SQL-Class: %s %s"
#define ERR_MSG_UNKNOWN_POOLTYPE "Unknown SessionPoolType for %s."
#define ERR_MSG_UNKNOWN_ISOLATION "Unknown Isolation for %s."
#define ERR_MSG_NO_SERVERDB "No ServerDb specified for %s."
#define ERR_MSG_INVALID_DATE_FORMAT "The string %s is not a date."
#define ERR_MSG_NO_CLASSID "No class-id specified for %s."
#define ERR_MSG_BUFFER_TOO_SMALL "The buffer is too small."
#define ERR_MSG_NOT_CONNECTED "Connection is not alive."
#define ERR_MSG_READ_REGISTRY "Cannot read registry: %s (%s)"

/* New errors in SAP DB 72 Tool */
#define ERR_MSG_WRITE_REGISTRY "Cannot write registry: %s (%s)"
#define ERR_MSG_MEMORY_LEAK_ASSUMPTION "Memory leak assumption: %s (%s)"
#define ERR_MSG_NO_SERVICE_NAME "There is no service name defined for service %s"
#define ERR_MSG_NO_LIBRARY "There is no library defined for service %s"
#define ERR_MSG_NO_SSL_URL_SPECIFIED "There is no SSL URL specified"
#define ERR_MSG_HEADERS_ALREADY_SENT "The response-headers have already been sent."
#define ERR_MSG_SESSION_POOL_LOAD_FAILED	"Session pool could not be loaded: %s"
#define ERR_MSG_SESSION_POOL_UNLOAD_FAILED	"Session pool could not be unloaded properly: %s"
#define ERR_MSG_OPEN_REGISTRY "Cannot open registry: %s"

/* DBFS error messages */
#define ERR_MSG_CANT_GET_ODBC_HANDLES "Can't get ODBC handles."
#define ERR_MSG_CANT_GET_DBCONNECTION "Can't get DB-connection."
#define ERR_MSG_INVALID_FILE_DESC "Invalid file-descriptor."
#define ERR_MSG_FILE_NOT_FOUND "File %s not found."
#define ERR_MSG_FILE_OPEN "Error opening file %s."
#define ERR_MSG_WRITE_ERROR "Error writing to file %s."
#define ERR_MSG_DIR_NOT_FOUND "Directory %s not found."
#define ERR_MSG_NOT_DIR "%s is not a directory."
#define ERR_MSG_NOT_FILE "%s is not a file."
#define ERR_MSG_DIR_EXISTS "The directory %s already exists."
#define ERR_MSG_DIR_NOT_EMPTY "The directory %s is not empty."
#define ERR_MSG_FILE_OR_DIR_NOT_FOUND "File or directory %s not found."
#define ERR_MSG_FILE_OR_DIR_EXISTS "File or directory %s already exists."
#define ERR_MSG_INVALID_FILENAME "%s is an invalid filename."
#define ERR_MSG_FILENAME_TOO_LONG "The file or directory name %s is too long."
#define ERR_MSG_DIFFERENT_PATH_NAMES "The paths %s and %s have different prefixes."
#define ERR_MSG_COMPRESS "Error while compressing data: %s."
#define ERR_MSG_UNCOMPRESS "Error while uncompressing data: %s."
#define ERR_MSG_SUBDIR_NOT_FOUND "Subdirectory %s not found."
#define ERR_MSG_DESTDIR_IS_SUBDIR "Destination directory %s is a subdirectory of the source directory %s."
#define ERR_MSG_PATH_TOO_LONG "The path %s is too long."
#define ERR_MSG_NO_MORE_ENTRIES "No more entries in directory."
#define ERR_MSG_LINK_TARGET_NOT_FOUND "Target of link %s not found."
#define ERR_MSG_MAX_POOL_SIZE_REACHED "The maximum amount of session for this pool is reached."

static char *wd26ErrorMessages[] =
{
    ERR_MSG_NO_ERROR,
    ERR_MSG_MEMORY,
    ERR_MSG_ODBC,
    ERR_MSG_INTERNAL,
    ERR_MSG_UNKNOWN,
    ERR_MSG_INIT_MT,
    "", "", "", "",
    ERR_MSG_INI_FILE_OPEN,
    "",
    ERR_MSG_NON_UNIQUE,
    ERR_MSG_NO_USERDLLS,
    ERR_MSG_PARAM_TOO_LONG,
    ERR_MSG_POOLSIZE,
    ERR_MSG_NO_USER,
    ERR_MSG_NO_PASSWORD,
    ERR_MSG_NO_PREFIX,
    ERR_MSG_NO_USERDLL,
    ERR_MSG_NO_SUCH_POOL,
    ERR_MSG_USERDLL_LOAD_FAILED,
    ERR_MSG_USERDLL__UNLOAD_FAILED,
    ERR_MSG_NO_SUCH_FUNCTION,
    ERR_MSG_NAME_TOO_LONG,
    ERR_MSG_HTTP_API,
    ERR_MSG_SERVICE_FUNC_ERROR,
    ERR_MSG_NO_DATASOURCE,
    ERR_MSG_NO_INIT_FUNC,
    ERR_MSG_NO_EXIT_FUNC,
    ERR_MSG_NO_SERVICE_FUNC,
    ERR_MSG_INIT_FUNC_ERROR,
    ERR_MSG_EXIT_FUNC_ERROR,
    ERR_MSG_EXCEPTION,
    ERR_MSG_USERDLL_NOT_INIT,
    ERR_MSG_INVALID_CONNECT_ATTR,
    ERR_MSG_DBC_NOT_ACTIVE,
    ERR_MSG_CANT_SET_COOKIE,
    ERR_MSG_WEBSESSION_LOCKED,
    ERR_MSG_NO_WEBSESSION,
    ERR_MSG_WEBSESSION_TIMEOUT,
    ERR_MSG_WEBSESSIONS_DISABLED,
    ERR_MSG_SQLCLASS,
    ERR_MSG_UNKNOWN_POOLTYPE,
    ERR_MSG_UNKNOWN_ISOLATION,
    ERR_MSG_NO_SERVERDB,
    ERR_MSG_INVALID_DATE_FORMAT,
    ERR_MSG_NO_CLASSID,
    ERR_MSG_BUFFER_TOO_SMALL,
    ERR_MSG_NOT_CONNECTED,
    ERR_MSG_READ_REGISTRY,
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",

	/* New errors in SAP DB 72 Tool */
    ERR_MSG_WRITE_REGISTRY,
	ERR_MSG_MEMORY_LEAK_ASSUMPTION,
	ERR_MSG_NO_SERVICE_NAME,
	ERR_MSG_NO_LIBRARY,
	ERR_MSG_NO_SSL_URL_SPECIFIED,
    ERR_MSG_HEADERS_ALREADY_SENT,
	ERR_MSG_SESSION_POOL_LOAD_FAILED,
	ERR_MSG_SESSION_POOL_UNLOAD_FAILED,
    ERR_MSG_OPEN_REGISTRY
}; /* wd26ErrorMessages */
    

static char *wd26DBFSErrorMessages[] =
{
    "", "", "", "", "", "", "", "", "", "",
    ERR_MSG_CANT_GET_ODBC_HANDLES,
    ERR_MSG_CANT_GET_DBCONNECTION,
    ERR_MSG_INVALID_FILE_DESC,
    ERR_MSG_FILE_NOT_FOUND,
    ERR_MSG_FILE_OPEN,
    ERR_MSG_WRITE_ERROR,
    ERR_MSG_DIR_NOT_FOUND,
    ERR_MSG_NOT_DIR,
    ERR_MSG_NOT_FILE,
    ERR_MSG_DIR_EXISTS,
    ERR_MSG_DIR_NOT_EMPTY,
    ERR_MSG_FILE_OR_DIR_NOT_FOUND,
    ERR_MSG_FILE_OR_DIR_EXISTS,
    ERR_MSG_INVALID_FILENAME,
    ERR_MSG_FILENAME_TOO_LONG,
    ERR_MSG_DIFFERENT_PATH_NAMES,
    ERR_MSG_COMPRESS,
    ERR_MSG_UNCOMPRESS,
    ERR_MSG_SUBDIR_NOT_FOUND,
    ERR_MSG_DESTDIR_IS_SUBDIR,
    ERR_MSG_PATH_TOO_LONG,
    ERR_MSG_NO_MORE_ENTRIES,
    ERR_MSG_LINK_TARGET_NOT_FOUND,
	ERR_MSG_MAX_POOL_SIZE_REACHED
}; /* wd26DBFSErrorMessages */

struct st_wa_err
{
    sapdbwa_Int2   errId;
    char           errArg1[ERR_ARG_LEN];
    char           errArg2[ERR_ARG_LEN];
    char          *msg; /* the complete error message */
    SQLINTEGER     nativeOdbcErr;
    UCHAR          sqlstate[10];
    sapdbwa_Int2   odbcReturnCode;
}; /* st_wa_err */


twd26ErrP wd26CreateErr()
{
    twd26ErrP  newErr;
    tsp00_Bool allocatOK;
    
    sapdbwa_SQLALLOCAT( sizeof(struct st_wa_err),
                   (sapdbwa_UInt1**) &newErr,
                   &allocatOK );
    if (!allocatOK) {
        /* memory allocation error */
        return NULL;
    }; /* if */

    newErr->msg = NULL;
    wd26ResetErr( newErr );
    
    return newErr;
} /* wd26CreateErr */


void wd26DestroyErr( twd26ErrP err )
{
    
    if (!err) {
        return;
    };

    if (err->msg != NULL) {
        sapdbwa_SQLFREE( (sapdbwa_UInt1*) err->msg );
    }; /* if */
    
    sapdbwa_SQLFREE( (sapdbwa_UInt1*) err );

} /* wd26DestroyErr */


void wd26CopyErr( twd26ErrP destErr, twd26ErrP sourceErr )
{
    if (!destErr || !sourceErr) {
        return;
    };

    SAPDB_memcpy( destErr, sourceErr, sizeof( struct st_wa_err ) );
    /* dont share errMsg buffer */
    destErr->msg = NULL;
} /* wd26CopyErr */


void wd26CopyMsg( twd26ErrP err, char *buf, sapdbwa_UInt4 bufLen )
{
    const char *msg = NULL;
    sapdbwa_UInt4 copyLen = 0;

    if (!err || !buf) {
        return;
    }

    msg = wd26GetMsg( err );
    copyLen = sapdbwa_MIN( bufLen-1, strlen( msg )-1 );

    strncpy( buf, msg, copyLen );
    buf[copyLen] = '\0';    
} /* wd26CopyMsg */


const char* wd26GetMsg( twd26ErrP err )
{
    tsp00_Bool allocatOK;
    sapdbwa_Int4 msgLen = 0;
    
    if (err == NULL) {
        return (ERR_MSG_UNKNOWN);
    }; /* if */
    
    if (err->msg == NULL) {
        msgLen = MAX_ERRORTEXT_LEN_WD00;
        if (err->errArg1 != NULL) {
            msgLen += strlen(err->errArg1);
        }; /* if */
        if (err->errArg2 != NULL) {
            msgLen += strlen(err->errArg2);
        }; /* if */
        sapdbwa_SQLALLOCAT( msgLen,
                       (sapdbwa_UInt1**) &err->msg,
                       &allocatOK );
        if (allocatOK) {
            sprintf( err->msg,
                     wd26_GetErrMsg( err->errId ),
                     err->errArg1, err->errArg2 );
        } else {
            err->msg = NULL;
        }; /* else */
    }; /* if */
    
    return err->msg;
} /* wd26GetMsg */


sapdbwa_UInt4 wd26GetId( twd26ErrP err )
{
    if (err != NULL) {
        return err->errId;
    } else {
        return ERR_UNKNOWN_WD26;
    }; /* else */
} /* wd26GetId */


sapdbwa_Int4 wd26GetNativeOdbcErr( twd26ErrP err )
{
    if (!err) {
        return 0;
    };

    return (sapdbwa_Int4) err->nativeOdbcErr;
} /* wd26GetNativeOdbcErr */


void wd26SetErr( twd26ErrP   err,
                 sapdbwa_Int2  errId,
                 const char *arg1,
                 const char *arg2 )
{
    if (!err) {
        return;
    };

    wd26ResetErr( err );
    err->errId = errId;
    if (errId == ERR_COMPRESS_WD26 || errId == ERR_UNCOMPRESS_WD26) {
        if (arg1 != NULL) {
            sapdbwa_Int4 comprErr = *((sapdbwa_Int4*) arg1);
            strcpy( err->errArg1, wd26_GetComprErrStr( comprErr ) );
        }; /* if */
    } else {
        if (arg1 != NULL) {
            strncpy( err->errArg1, arg1, sizeof(err->errArg1)-1 );
            err->errArg1[sizeof(err->errArg1)-1] = '\0';
            /* set '...' to show that arg was longer */
            err->errArg1[sizeof(err->errArg1)-2] = '.';
            err->errArg1[sizeof(err->errArg1)-3] = '.';
            err->errArg1[sizeof(err->errArg1)-4] = '.';
        }; /* if */
        if (arg2 != NULL) {
            strncpy( err->errArg2, arg2, sizeof(err->errArg2)-1 );
            err->errArg2[sizeof(err->errArg1)-1] = '\0';
            /* set '...' to show that arg was longer */
            err->errArg1[sizeof(err->errArg1)-2] = '.';
            err->errArg1[sizeof(err->errArg1)-3] = '.';
            err->errArg1[sizeof(err->errArg1)-4] = '.';
        }; /* if */
    }; /* else */
} /* wd26SetErr */


void wd26SetOdbcError( twd26ErrP  err,
                       RETCODE    sqlRetcode,
                       SQLHENV    henv,
                       SQLHDBC    hdbc,
                       SQLHSTMT   hstmt )
{
    if (!err) {
        return;
    };

    err->odbcReturnCode = sqlRetcode;
    switch (sqlRetcode) {
    case SQL_ERROR:
        wd26_SetOdbcErrorMessage( err, henv, hdbc, hstmt );
        sprintf( err->errArg2, "SQL_ERROR (%d).", err->nativeOdbcErr );
        break;
    case SQL_SUCCESS_WITH_INFO:
        wd26_SetOdbcErrorMessage( err, henv, hdbc, hstmt );
        strcpy( err->errArg2, "SQL_SUCCESS_WITH_INFO." );
        break;
    case SQL_INVALID_HANDLE:
        strcpy( err->errArg1, "" );
        strcpy( err->errArg2, "SQL_INVALID_HANDLE." );
        break;
    case SQL_NO_DATA_FOUND:
        strcpy( err->errArg1, "" );
        strcpy( err->errArg2, "SQL_NO_DATA_FOUND." );
        break;
    case SQL_SUCCESS:
        strcpy( err->errArg1, "" );
        strcpy( err->errArg2, "SQL_SUCCESS." );
        break;
    default:
        strcpy( err->errArg1, "" );
        strcpy( err->errArg2, "Unknown ODBC return code!!!" );
        break;
    }; /* switch */
    
    err->errId = ERR_ODBC_WD26;
} /* wd26SetOdbcError */
    


void wd26ResetErr( twd26ErrP err )
{
    if (!err) {
        return;
    };

    sapdbwa_SQLFREE( (sapdbwa_UInt1*) err->msg );
    err->msg = NULL;
    
    err->errId      = ERR_NO_ERROR_WD26;
    err->errArg1[0] = '\0';
    err->errArg2[0] = '\0';
    err->nativeOdbcErr = 0;
    err->sqlstate[0] = '\0';
    err->odbcReturnCode = SQL_SUCCESS;
} /* wd26ResetErr */


void wd26LogErr( twd26ErrP err, twd25LogP log )
{
    char msgBuff[2*MAX_ERRORTEXT_LEN_WD00+2*ERR_ARG_LEN];

    if (!err||!log) {
        return;
    };

	/* Is there an error? */
	if ( err->errId == ERR_NO_ERROR_WD26 )
		return;

	/* Write error to log */
    sprintf( msgBuff,
             wd26_GetErrMsg( err->errId ),
             err->errArg1,
             err->errArg2 );
    strcat( msgBuff, "\n" );
    wd25WriteLogMsg( log, msgBuff );

	/* Reset error */
	wd26ResetErr( err );
} /* wd26LogErr */


void wd26PrintErr( twd26ErrP err )
{
    char msgBuff[2*MAX_ERRORTEXT_LEN_WD00+2*ERR_ARG_LEN];

    if (!err) {
        return;
    };

    sprintf( msgBuff,
             wd26_GetErrMsg( err->errId ),
             err->errArg1,
             err->errArg2 );
    strcat( msgBuff, "\n" );
    printf( msgBuff );
} /* wd26PrintErr */


void WINAPI sapdbwa_DestroyErr( sapdbwa_ErrP err )
{
    wd26DestroyErr( err );
} /* sapdbwa_DestroyErr */


sapdbwa_ErrP WINAPI sapdbwa_CreateErr()
{
    return wd26CreateErr();
} /* sapdbwa_CreateErr */

    
void WINAPI sapdbwa_CopyErr( sapdbwa_ErrP errIn, sapdbwa_ErrP errOut )
{
    if (errIn != NULL) {
        wd26CopyErr( errOut, errIn );
    };
} /* sapdbwa_CopyErr */


void WINAPI sapdbwa_CopyErrMsg( sapdbwa_ErrP err, char *buf, sapdbwa_UInt4 bufLen )
{
    if (err != NULL) {
        wd26CopyMsg( err, buf, bufLen );
    };
} /* sapdbwa_CopyErrMsg */


const char* WINAPI sapdbwa_GetErrMsg( sapdbwa_ErrP err )
{
    if (err != NULL) {
        return wd26GetMsg( err );
    } else {
        return "";
    }; /* else */
} /* sapdbwa_GetErrMsg */


sapdbwa_UInt4 WINAPI sapdbwa_GetErrId( sapdbwa_ErrP err )
{
    if (err != NULL) {
        return wd26GetId( err );
    } else {
        return ERR_UNKNOWN_WD26;
    }; /* else */
} /* sapdbwa_GetErrId */


sapdbwa_Int4 WINAPI sapdbwa_GetNativeOdbcErr( sapdbwa_ErrP err )
{
    if (err != NULL) {
        return wd26GetNativeOdbcErr( err );
    } else {
        return 0;
    }; /* else */
} /* sapdbwa_GetNativeOdbcErr */


sapdbwa_Int2 WINAPI sapdbwa_GetOdbcReturnCode( sapdbwa_ErrP err )
{
    if (err != NULL) {
        return err->odbcReturnCode;
    } else {
        return 0;
    }; /* else */
} /* sapdbwa_GetNativeOdbcErr */


const char *sapdbwa_GetSqlState( sapdbwa_ErrP err )
{
    if (err != NULL) {
        return (const char*) err->sqlstate;
    } else {
        return NULL;
    }; /* else */
} /* sapdbwa_GetSqlState */

/*!**********************************************************************

  EndChapter: twd26ErrP
  
************************************************************************/
/*!**********************************************************************

  Chapter: internal functions
  
************************************************************************/

/* copied from hpa104CsObject.h */
/*--------------------------------------------------------------------*/
/* Error & Return Codes for CsCompr and CsDecompr                     */
/*--------------------------------------------------------------------*/
#define CS_END_INBUFFER          3     /* End of input buffer ........*/
#define CS_END_OUTBUFFER         2     /* End of output buffer .......*/
#define CS_END_OF_STREAM         1     /* End of data ................*/
#define CS_OK                    0

#define CS_IEND_OF_STREAM       -1     /* End of data (internal) .....*/
#define CS_IEND_OUTBUFFER       -2     /* End of output buffer .......*/
#define CS_IEND_INBUFFER        -3     /* End of input buffer ........*/

#define CS_ERROR               -10     /* First Error Code ...........*/
#define CS_E_OUT_BUFFER_LEN    -10     /* Invalid output length ......*/
#define CS_E_IN_BUFFER_LEN     -11     /* Invalid input length .......*/
#define CS_E_NOSAVINGS         -12
#define CS_E_INVALID_SUMLEN    -13     /* Invalid len of stream ......*/
#define CS_E_IN_EQU_OUT        -14     /* inbuf == outbuf ............*/
#define CS_E_INVALID_ADDR      -15     /* inbuf == NULL,outbuf == NULL*/
#define CS_E_FATAL             -19     /* Internal Error ! ...........*/
#define CS_E_BOTH_ZERO         -20     /* inlen = outlen = 0 .........*/
#define CS_E_UNKNOWN_ALG       -21     /* unknown algorithm ..........*/
#define CS_E_UNKNOWN_TYPE      -22

/* for decompress */
#define CS_E_FILENOTCOMPRESSED -50     /* Input not compressed .......*/
#define CS_E_MAXBITS_TOO_BIG   -51     /* maxbits to large ...........*/
#define CS_E_BAD_HUF_TREE      -52     /* bad hufman tree   ..........*/
#define CS_E_NO_STACKMEM       -53     /* no stack memory in decomp ..*/
#define CS_E_INVALIDCODE       -54     /* invalid code ...............*/
#define CS_E_BADLENGTH         -55     /* bad lengths ................*/

#define CS_E_STACK_OVERFLOW    -60     /* stack overflow in decomp    */
#define CS_E_STACK_UNDERFLOW   -61     /* stack underflow in decomp   */

/* only Windows */
#define CS_NOT_INITIALIZED     -71     /* storage not allocated ......*/

/* values for comprError. more are in hpa104CsObject.h */
#define ERR_NO_MEMORY_PA110       -100 /* memory allocation failed */
#define ERR_NOT_INITIALIZED_PA110 -101 /* Init was not called */
#define ERR_INCONSISTENCE_PA110   -102 /* data to decompress is inconsistent */

const char* wd26_GetComprErrStr( sapdbwa_Int4 comprErr )
{
    switch (comprErr) {
    case CS_END_INBUFFER: /* End of input buffer ........*/
        return "CS_END_INBUFFER";
        break;
    case CS_END_OUTBUFFER: /* End of output buffer */
        return "CS_END_OUTBUFFER";
        break;
    case CS_END_OF_STREAM: /* End of data .........*/
        return "CS_END_OF_STREAM";
        break;
    case CS_OK: 
        return "CS_OK";
        break;
    case CS_IEND_OF_STREAM: /* End of data (internal) .....*/
        return "CS_IEND_OF_STREAM";
        break;
    case CS_IEND_OUTBUFFER: /* End of output buffer .......*/
        return "CS_IEND_OUTBUFFER";
        break;
    case CS_IEND_INBUFFER: /* End of input buffer ........*/    
        return "CS_IEND_INBUFFER";
        break;
    case CS_E_OUT_BUFFER_LEN: /* Invalid output length ......*/
        return "CS_E_OUT_BUFFER_LEN";
        break;
    case CS_E_IN_BUFFER_LEN: /* Invalid input length .......*/
        return "CS_E_IN_BUFFER_LEN";
        break;
    case CS_E_NOSAVINGS:
        return "CS_E_NOSAVINGS";
        break;
    case CS_E_INVALID_SUMLEN: /* Invalid len of stream ......*/
        return "CS_E_INVALID_SUMLEN";
        break;
    case CS_E_IN_EQU_OUT: /* inbuf == outbuf ............*/
        return "CS_E_IN_EQU_OUT";
        break;
    case CS_E_INVALID_ADDR: /* inbuf == NULL,outbuf == NULL*/
        return "CS_E_INVALID_ADDR";
        break;
    case CS_E_FATAL: /* Internal Error ! ...........*/
        return "CS_E_FATAL";
        break;
    case CS_E_BOTH_ZERO: /* inlen = outlen = 0 .........*/
        return "CS_E_BOTH_ZERO";
        break;
    case CS_E_UNKNOWN_ALG: /* unknown algorithm ..........*/
        return "CS_E_UNKNOWN_ALG";
        break;
    case CS_E_UNKNOWN_TYPE:               
        return "CS_E_UNKNOWN_TYPE";
        break;
        /* for decompress */
    case CS_E_FILENOTCOMPRESSED: /* Input not compressed .......*/
        return "CS_E_FILENOTCOMPRESSED";
        break;
    case CS_E_MAXBITS_TOO_BIG: /* maxbits to large ...........*/
        return "CS_E_MAXBITS_TOO_BIG";
        break;
    case CS_E_BAD_HUF_TREE: /* bad hufman tree   ..........*/
        return "CS_E_BAD_HUF_TREE";
        break;
    case CS_E_NO_STACKMEM: /* no stack memory in decomp ..*/
        return "CS_E_NO_STACKMEM";
        break;
    case CS_E_INVALIDCODE: /* invalid code ...............*/
        return "CS_E_INVALIDCODE";
        break;
    case CS_E_BADLENGTH: /* bad lengths ................*/
        return "CS_E_BADLENGTH";
        break;
    case CS_E_STACK_OVERFLOW: /* stack overflow in decomp    */
        return "CS_E_STACK_OVERFLOW";
        break;
    case CS_E_STACK_UNDERFLOW: /* stack underflow in decomp   */            
        return "CS_E_STACK_UNDERFLOW";
        break;
    case CS_NOT_INITIALIZED: /* storage not allocated ......*/
        return "CS_NOT_INITIALIZED";
        break;
    case ERR_NO_MEMORY_PA110:
        return "ERR_NO_MEMORY_PA110";
        break;
    case ERR_NOT_INITIALIZED_PA110:
        return "ERR_NOT_INITIALIZED_PA110";
        break;
    case ERR_INCONSISTENCE_PA110:
        return "ERR_INCONSISTENCE_PA110";
    default:
        return "Unknown compress-error!";
        break;
    }; /* switch */
} /* wd26_GetComprErrStr */


char* wd26_GetErrMsg( sapdbwa_Int2 errId )
{
    if (errId<100) {
        return wd26ErrorMessages[errId];
    } else {
        return wd26DBFSErrorMessages[errId-100];
    }; /* else */
} /* wd26_GetErrMsg */


void wd26_SetOdbcErrorMessage( twd26ErrP  err,
                               SQLHENV    henv,
                               SQLHDBC    hdbc,
                               SQLHSTMT   hstmt )
{

    RETCODE                 retcode;
    SQLWCHAR                messageText[ODBC_ERROR_TEXT_LEN_WD00+1];    /* SQLWCHAR is an unsigned short! */
    SQLWCHAR                sqlState[10];
    SQLWCHAR               *pSqlState = sqlState;
	tsp81_UCS2Char const   *srcAtUCS2 = NULL;
    tsp00_Byte             *destAtUCS2 = NULL;
    SWORD                   errLen;

    if (!err) {
        return;
    };

    retcode = SQLErrorW( henv, hdbc, hstmt,
                         sqlState,
                        &err->nativeOdbcErr,
                         messageText,
                         ODBC_ERROR_TEXT_LEN_WD00+1,
                        &errLen );

	sp83UTF8ConvertFromUCS2((const tsp81_UCS2Char*) sqlState,  
							(const tsp81_UCS2Char*) sqlState + sp81UCS2strlen((const tsp81_UCS2Char*)sqlState) * 2,
						    &srcAtUCS2, WD_IS_UCS2_SWAPED,
                            (tsp00_Byte*)err->sqlstate, (tsp00_Byte*)(err->sqlstate + sizeof(err->sqlstate)),
                            &destAtUCS2);

    if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO) {
	    sp83UTF8ConvertFromUCS2((const tsp81_UCS2Char*) messageText,  
							    (const tsp81_UCS2Char*) messageText + sp81UCS2strlen((const tsp81_UCS2Char*)messageText) * 2,
							    &srcAtUCS2, WD_IS_UCS2_SWAPED,
                                (tsp00_Byte*)err->errArg1, (tsp00_Byte*)(err->errArg1 + sizeof(err->errArg1)),
                                &destAtUCS2);
    } else {
        strcpy( err->errArg1, "Can't get ODBC-Error!" );
    };

}

/*!**********************************************************************

  EndChapter: internal functions
  
************************************************************************/
