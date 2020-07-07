/*



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
&ifdef xINHOUSEx
/*!**********************************************************************

  module: hwd06waerr.h -> sapdbwa_error.h

  -----------------------------------------------------------------------

  responsible:	Holger Bischoff
				Markus Özgen

  special area: SAPDB WebAgent Error Codes

  description:  Public description of WebAgent error codes

  see also:     

  -----------------------------------------------------------------------

  copyright:          Copyright (c) 2000-2005 SAP AG

************************************************************************/
&endif

#ifndef SAPDBWAERROR_H
#define SAPDBWAERROR_H

#include <sapdbwa_define.h>

/************************************************************************

  Chapter: sapdbwa_ErrP

  description:
  
************************************************************************/

#if defined(__cplusplus)
extern "C" {
#endif

typedef struct st_wa_err	*sapdbwa_ErrP;

sapdbwa_ErrP WINAPI sapdbwa_CreateErr();

void WINAPI sapdbwa_DestroyErr( sapdbwa_ErrP err );

void WINAPI sapdbwa_CopyErr( sapdbwa_ErrP errIn, sapdbwa_ErrP errOut );
    
void WINAPI sapdbwa_CopyErrMsg( sapdbwa_ErrP err, char *buf, sapdbwa_UInt4 bufLen );
    
const char* WINAPI sapdbwa_GetErrMsg( sapdbwa_ErrP err );

sapdbwa_UInt4 WINAPI sapdbwa_GetErrId( sapdbwa_ErrP err );

sapdbwa_Int4 WINAPI sapdbwa_GetNativeOdbcErr( sapdbwa_ErrP err );

const char *sapdbwa_GetSqlState( sapdbwa_ErrP err );
    
sapdbwa_Int2 WINAPI sapdbwa_GetOdbcReturnCode( sapdbwa_ErrP err );
    
#if defined(__cplusplus)
}
#endif

/************************************************************************

  EndChapter: sapdbwa_ErrP

************************************************************************/

#if defined(__cplusplus)
/*!**********************************************************************

  Class: sapdbwa_Err

  description:
  The sapdbwa_Err class is used by all WebAgent classes to store error
  information. When a method of any WebAgent-class failes the method
  GetErr, which exists for all classes that store error information,
  is used to get the error object. The sapdbwa_Err-class provides methods
  to retrieve error information such as error-message and -id.

************************************************************************/

class sapdbwa_Err
{
public:
    /* the use of this constructor is forbidden for applications */
    sapdbwa_Err( sapdbwa_ErrP err );
    void SetErr( sapdbwa_ErrP err );

/*!**********************************************************************
  Function: sapdbwa_Err

  description: default constructor

  arguments:
    
  return value:
*/
    sapdbwa_Err();

/*!**********************************************************************
  Function: sapdbwa_Err

  description: copy constructor

  arguments:
     err [in]  source error object.

  return value:
*/
    sapdbwa_Err( const sapdbwa_Err &err );

/*!**********************************************************************
  Function: ~sapdbwa_Err

  description: destructor

  arguments:
    
  return value:
*/
    ~sapdbwa_Err();

/*!**********************************************************************
  Function: operator=

  description: Make copy of error object.

  This can be used, to save error information in a local error object
  because the error object in the WebAgent classes (sapdbwa_WebAgent,
  sapdbwa_SessionPool, ...) is overwritten if the next method is called.

  arguments:
    errIn  [in] source error object.

  return value:
  reference to *this.
*/
    sapdbwa_Err& operator=( const sapdbwa_Err &errIn );

/*!**********************************************************************
  Function: CopyErrMsg

  description: Get copy of error message text.

  arguments:
     buf    [out]  Buffer to store text.
     bufLen [in]   Length of buffer.

  return value: void
*/
    void CopyErrMsg( char *buf, sapdbwa_UInt4 bufLen ) const;

/*!**********************************************************************
  Function: GetErrMsg

  description: Get char* pointer to error message.

  arguments:
  
  return value: Pointer to error message.
*/
    const char* GetErrMsg() const;

/*!**********************************************************************
  Function: GetErrId

  description: Return error code.

  arguments:
    
  return value: Code of the error as defined below.
*/
    sapdbwa_UInt4 GetErrId() const;

/*!**********************************************************************
  Function: GetNativeOdbcErr

  description: Return native (DB specific) error code from ODBC.

  arguments:
    
  return value: Native error code.
*/
    sapdbwa_Int4  GetNativeOdbcErr() const;

    
/*!**********************************************************************
  Function: GetOdbcReturnCode

  description: Get return-code from the ODBC-function that failed.

  arguments:
    
  return value: ODBC return-code (SQL_SUCCESS, SQL_ERROR, ... ).
*/    
    sapdbwa_Int2 GetOdbcReturnCode() const;


/*!**********************************************************************
  Function: GetSqlState

  description: Get the SQL-State from ODBC errors.

  arguments:
      
  return value: The SQL-State of the error or an empty string if err is
                not a ODBC error */

    const char *GetSqlState() const;
    
private:
    sapdbwa_ErrP     err;
    sapdbwa_Bool  ownErr;
}; // class sapdbwa_Err


/*!**********************************************************************

  EndClass: sapdbwa_Err

************************************************************************/
/************************************************************************

  Chapter: methods for sapdbwa_Err

************************************************************************/

inline sapdbwa_Err::sapdbwa_Err( sapdbwa_ErrP err )
    : ownErr(sapdbwa_False), err(err)
{
}; // sapdbwa_Err


inline void sapdbwa_Err::SetErr( sapdbwa_ErrP errP )
{
    // assert this->ownErr == sapdbwa_False
    this->ownErr = sapdbwa_False;
    this->err    = errP;
}; // SetErr


inline sapdbwa_Err::sapdbwa_Err()
    : ownErr(sapdbwa_True)
{
    this->err = sapdbwa_CreateErr();
}; // sapdbwa_Err


inline sapdbwa_Err::sapdbwa_Err( const sapdbwa_Err &sourceErr )
    : ownErr(sapdbwa_True)
{
    this->err = sapdbwa_CreateErr();
    sapdbwa_CopyErr( sourceErr.err, this->err );
}; // sapdbwa_Err


inline sapdbwa_Err::~sapdbwa_Err()
{
    if (this->ownErr) {
        sapdbwa_DestroyErr( this->err );
    }; // if
}; // ~sapdbwa_Err


inline sapdbwa_Err& sapdbwa_Err::operator=( const sapdbwa_Err &errIn )
{
    sapdbwa_CopyErr( errIn.err, this->err );
    return *this;
}; // oprator=


inline void sapdbwa_Err::CopyErrMsg( char *buf, sapdbwa_UInt4 bufLen ) const
{
    sapdbwa_CopyErrMsg( this->err, buf, bufLen );
}; // CopyErrMsg


inline const char* sapdbwa_Err::GetErrMsg() const
{
    return sapdbwa_GetErrMsg( this->err );
}; // GetErrMsg


inline sapdbwa_UInt4 sapdbwa_Err::GetErrId() const
{
    return sapdbwa_GetErrId( this->err );
}; // GetErrId


inline sapdbwa_Int4 sapdbwa_Err::GetNativeOdbcErr() const
{
    return sapdbwa_GetNativeOdbcErr( this->err );
}; // GetNativeOdbcErr


inline sapdbwa_Int2 sapdbwa_Err::GetOdbcReturnCode() const
{
    return sapdbwa_GetOdbcReturnCode( this->err );
}; // GetOdbcReturnCode


inline const char * sapdbwa_Err::GetSqlState() const
{
    return sapdbwa_GetSqlState( this->err );
}; // GetOdbcReturnCode


#endif /* if defined(__cplusplus) */

/* general errors */
#define sapdbwa_Error_NoError				0	/* this error is no error :-)		*/
#define sapdbwa_Error_Memory				1	/* Memory allocation failure		*/
#define sapdbwa_Error_ODBC					2	/* ODBC-error						*/
#define sapdbwa_Error_Internal				3	/* Unexpected internal error		*/
												/* == a bug in the WebAgent			*/
#define sapdbwa_Error_Unknown				4
#define sapdbwa_Error_InitMT				5	/* init multithreading failed		*/
#define sapdbwa_Error_ParamTooLong			14
#define sapdbwa_Error_PoolSize				15
#define sapdbwa_Error_WebSessionLocked		38
#define sapdbwa_Error_NoWebSession			39
#define sapdbwa_Error_WebSessionTimeout		40
#define sapdbwa_Error_WebSessionsDisabled	41
#define sapdbwa_Error_SQLClass				42
#define sapdbwa_Error_InvalidDateFormat		46
#define sapdbwa_Error_HeadersAlreadySent	65

/* DBFS error codes */
#define sapdbwa_Error_CannotGetODBCHandles					110
#define sapdbwa_Error_CannotGetDBConnection					111
#define sapdbwa_Error_InvalidFileDescription				112
#define sapdbwa_Error_FileNotFound							113
#define sapdbwa_Error_FileOpen								114
#define sapdbwa_Error_WriteError							115
#define sapdbwa_Error_DirectoryNotFound						116
#define sapdbwa_Error_NotDirectory							117
#define sapdbwa_Error_NotFile								118
#define sapdbwa_Error_DirectoryExists						119
#define sapdbwa_Error_DirectoryNotEmpty						120
#define sapdbwa_Error_FileOrDirectoryNotFound				121
#define sapdbwa_Error_FileOrDirectoryExists					122
#define sapdbwa_Error_InvalidFilename						123
#define sapdbwa_Error_FilenameTooLong						124
#define sapdbwa_Error_DifferentPathNames					125
#define sapdbwa_Error_Compress								126
#define sapdbwa_Error_Uncompress							127
#define sapdbwa_Error_SubDirectoryNotFound					128
#define sapdbwa_Error_DestinationDirectoryIsSubDirectory	129
#define sapdbwa_Error_PathTooLong							130
#define sapdbwa_Error_NoMoreEntries							131
#define sapdbwa_Error_LinkTargetNotFound					132
#define sapdbwa_Error_MaxPoolSizeReached					133

/*!**********************************************************************

  Chapter: WebAgent Error Codes

  Description: These are the codes returned by GetErrId.

  <UL>
  <LI> sapdbwa_Error_NoError:				This error is no error.
  <LI> sapdbwa_Error_Memory:				Memory allocation failure.
  <LI> sapdbwa_Error_ODBC:					An ODBC error occured. You can use GetNativeOdbcErr for more info.
  <LI> sapdbwa_Error_Internal:				Unexpected internal error. You found a bug in the WebAgent. Use GetErrMsg to get more info.
  <LI> sapdbwa_Error_Unknown:				No error information available.
  <LI> sapdbwa_Error_InitMT:				Initialization of the multithreading runtime failed.
  <LI> sapdbwa_Error_ParamTooLong:			The given parameter name is too long.
  <LI> sapdbwa_Error_PoolSize:				The given poolsize is too big (allowed values: 1<= poolsize <= 128).
  <LI> sapdbwa_Error_WebSessionLocked:		The requested web-session is already in use by another thread.
  <LI> sapdbwa_Error_NoWebSession:			No web-session was started.
  <LI> sapdbwa_Error_WebSessionTimeout:		The web-session timed out.
  <LI> sapdbwa_Error_WebSessionsDisabled:	The usage of web-sessions is disabled.
											Use sapdbwa_WebAgent.SetUseWebSessions in your UserInit function to enable it.
  <LI> sapdbwa_Error_SQLClass:				A SQL-Class error occured.
  <LI> sapdbwa_Error_InvalidDateFormat:		The given date is not in WebAgent format (yyyymmddhhmmss).
  <LI> sapdbwa_Error_MaxPoolSizeReached:	The maximum amount of session for this pool is reached.
  </UL>
*/

/*!**********************************************************************

  EndChapter: WebAgent Error Codes

************************************************************************/

/* cppDoc does not like the following */
/***********************************************************************

  Chapter: DBFS Error Codes

  description:
  These are the codes returned by GetErrId for operations
  on the database filesystem.
  
 - sapdbwa_Error_CannotGetODBCHandles:		ODBC handles were not available.
 - sapdbwa_Error_CannoGetDBConnection:		A database connection could not be established.
 - sapdbwa_Error_InvalidFileDescription:	The given file descriptor is not valid.
 - sapdbwa_Error_FileNotFound:				The given file does not exist.
 - sapdbwa_Error_FileOpen:					The outfile could not be opened for writing.
 - sapdbwa_Error_WriteError:				Error while writing to file.
 - sapdbwa_Error_DirectoryNotFound:			The directory was not found.
 - sapdbwa_Error_NotDirectory:				The given pathname is not a directory.
 - sapdbwa_Error_NotFile:					The given pathname is not a file.
 - sapdbwa_Error_DirectoryExists:			The directory exists.
 - sapdbwa_Error_DirectoryNotEmpty:			The directory is not empty.
 - sapdbwa_Error_FileOrDirectoryNotFound:	The given file or directory does not exist.
 - sapdbwa_Error_FileOrDirectoryExists:		A file directory with the given name already exists.
 - sapdbwa_Error_InvalidFilename:			The given filename is not valid.
 - sapdbwa_Error_FilenameTooLong:			The given filename is too long.
 - sapdbwa_Error_DifferentPathNames:
 - sapdbwa_Error_Compress:					Compression of file data failed.
 - sapdbwa_Error_Uncompress:				Decompression of file data failed.
 - sapdbwa_Error_SubDirectoryNotFound: 
 - sapdbwa_Error_DestinationDirectoryIsSubDirectory:	You cannot copy a directory to one of its subdirectories.
 - sapdbwa_Error_PathTooLong:							The given pathname is too long.
 - sapdbwa_Error_NoMoreEntries:							The directory does not contain any more entries.
 - sapdbwa_Error_LinkTargetNotFound:					Cannot find target for a link.
*/

/***********************************************************************

  EndChapter: DBFS Error Codes

************************************************************************/

#endif
