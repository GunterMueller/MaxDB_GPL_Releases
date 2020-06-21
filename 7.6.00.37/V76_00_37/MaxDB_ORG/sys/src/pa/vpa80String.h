/*



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





*/
#ifndef VPA80STRING_H
#define VPA80STRING_H
/*!**********************************************************************

  module: vpa80String.h

  -----------------------------------------------------------------------

  responsible:  

  special area: ODBC-Driver

  description:  string manipulating functions

  see also:     http://www.microsoft.com/data/odbc/

  -----------------------------------------------------------------------

  copyright:            Copyright (c) 1998-2005 SAP AG

************************************************************************/

/*=======================================================================
 *  INCLUDES
 *=====================================================================*/

#include "vpa00global.h"
#include "vpr05IfCom_String.h"

/*!**********************************************************************

  Chapter: exported functions

************************************************************************/

/*!
  Function: pa80AllocString

  description: Allocates and copies a string of given length.

  When an eror occurs (e.g not enough memory) API_NOT_OK is returned.
  If the string was successfully copied the function return API_OK.

  arguments:
    tsp77encoding *destEncoding       [in]   Encoding of destination buffer
    UCHAR   **dest    [out]  Points to the char-pointer that should be
                             allocated.
    UCHAR   *source   [in]   The string that is copied.
    UDWORD  length    [in]   length of source in bytes
    UDWORD  maxLength [in]   max. length to check source if length = SQL_NTS
    
  return value: API_RETCODE (API_OK or API_TRUNCATE)
  */
API_RETCODE pa80AllocString (const tsp77encoding *destEncoding,
                             UCHAR  **dest, 
                             UCHAR   *source,
                             UDWORD   length,
                             UDWORD   maxLength);

/*!*********************************************************************
  Function: pa80CopyFixedString

  description: Copies a fixed string into a buffer of given length.
               Length buffers have type SDWORD.

  The source string must not contain a null terminator. Trailing blanks
  of source are ignored.
  If the length of source +1 (null-terminator) is bigger than maxLen then
  only maxLen characters are copied, *sqlStatePtr is set to 01004
  (string data right truncated) and SQL_SUCCESS_WITH_INFO is returned.
  If the dest buffer is big enough for source the function returns
  SQL_SUCCESS.

  arguments:
    UCHAR   *source      [in]   The string that is copied.
    SDWORD   maxLen      [in]   The length of *dest.
    SDWORD   sourceLen   [in]   The length of source.
    UCHAR   *dest        [out]  Points to the target buffer.
    SDWORD  *totalLen    [out]  Points to a variable that holds the total
                                length of source.
    UWORD   *sqlStatePtr [out]  Points a sql-state variable
    
  return value: RETCODE (SQL_SUCCESS or SQL_SUCCESS_WITH_INFO)
  */

RETCODE pa80CopyFixedString( UCHAR   *source,
                             SDWORD   sourceLen,
                             SDWORD   maxLen,
                             UCHAR   *dest,
                             SDWORD  *totalLen,
                             UWORD   *sqlStatePtr );


/*!*********************************************************************
  Function: pa80CopyFixedStringEncoding

  description: Copies a fixed string into a buffer of given length.
               Length buffers have type UDWORD.

  Note: The source string is regarded as UTF8 encoded string!
        (usually strings from precompiler runtime (UTF8) are processed)

  The source string must not contain a null terminator. Trailing blanks
  of source are ignored.
  If the length of source +1 (null-terminator) is bigger than maxLen then
  only maxLen characters are copied, *sqlStatePtr is set to 01004
  (string data right truncated) and SQL_SUCCESS_WITH_INFO is returned.
  If the dest buffer is big enough for source the function returns
  SQL_SUCCESS.

  arguments:
    tsp77encoding *destEncoding       [in]   Encoding of destination buffer
    VOID    *dest                     [out]  Points to the target buffer.
    UDWORD   destLen                  [in]   The length of *dest in byte count
    UDWORD  *charLen                  [out]  Pointer to the total
                                             length written in char count
    UDWORD  *byteLen                  [out]  Pointer to the total
                                             length written in byte count
    UCHAR   *source                   [in]   The string that is copied.
    UDWORD   sourceLen                [in]   The length of source in bytes
    UWORD   *sqlStatePtr              [out]  Points a sql-state variable
    
  return value: RETCODE (SQL_SUCCESS or SQL_SUCCESS_WITH_INFO)
  */

RETCODE pa80CopyFixedStringEncoding ( const tsp77encoding *destEncoding,
                                      VOID    *dest,
                                      UDWORD   destLen,
                                      UDWORD  *charLen,
                                      UDWORD  *byteLen,
                                      UCHAR   *source,
                                      UDWORD   sourceLen,
                                      UWORD   *sqlStatePtr );


/*!*********************************************************************
  Function: pa80CopyStringEncodingExt

  description: Copies a string into a buffer of given length and given
               encoding. The source is always in ASCII

  Note: The source string is regarded as ASCII encoded string!
        (usually string constants are processed)

  If the length of source +1 (null-terminator) is bigger than maxLen then
  only maxLen characters are copied, *sqlStatePtr is set to 01004
  (string data right truncated) and SQL_SUCCESS_WITH_INFO is returned.
  If the dest buffer is big enough for source the function returns
  SQL_SUCCESS.

  arguments:
    tsp77encoding *destEncoding       [in]   Encoding of destination buffer
    UCHAR         *dest               [out]  Points to the target buffer.
    tsp00_Uint4    destBufferLength   [in]   The length of *dest.
    UDWORD        *charLen            [out]  Pointer to the total
                                             length written in char count
    UDWORD        *byteLen            [out]  Pointer to the total
                                             length written in byte count
    tsp77encoding *sourceEncoding     [in]   Encoding of source buffer
    UCHAR         *source             [in]   The string that is copied.
    UWORD         *sqlStatePtr        [out]  Points a sql-state variable
    
  return value: RETCODE (SQL_SUCCESS or SQL_SUCCESS_WITH_INFO)
  */
RETCODE pa80CopyStringEncodingExt (const tsp77encoding *destEncoding,
                                   VOID        *dest,
                                   tsp00_Uint4  destBufferLength,
                                   UDWORD      *charLen,
                                   UDWORD      *byteLen,
                                   const tsp77encoding *sourceEncoding,
                                   const void  *source,
                                   UWORD       *sqlStatePtr);

/*!*********************************************************************
  Function: pa80CopyStringEncoding

  description: special case of pa80CopyStringEncodingExt.
               The source is always in ASCII

  Note: The source string is regarded as ASCII encoded string!
        (usually string constants are processed)

  If the length of source +1 (null-terminator) is bigger than maxLen then
  only maxLen characters are copied, *sqlStatePtr is set to 01004
  (string data right truncated) and SQL_SUCCESS_WITH_INFO is returned.
  If the dest buffer is big enough for source the function returns
  SQL_SUCCESS.

  arguments:
    tsp77encoding *destEncoding       [in]   Encoding of destination buffer
    UCHAR         *dest               [out]  Points to the target buffer.
    tsp00_Uint4    destBufferLength   [in]   The length of *dest.
    UDWORD        *charLen            [out]  Pointer to the total
                                             length written in char count
    UDWORD        *byteLen            [out]  Pointer to the total
                                             length written in byte count
    tsp77encoding *sourceEncoding     [in]   Encoding of source buffer
    UCHAR         *source             [in]   The string that is copied.
    UWORD         *sqlStatePtr        [out]  Points a sql-state variable
    
  return value: RETCODE (SQL_SUCCESS or SQL_SUCCESS_WITH_INFO)
  */
RETCODE pa80CopyStringEncoding (const tsp77encoding *destEncoding,
                                VOID        *dest,
                                tsp00_Uint4  destBufferLength,
                                UDWORD      *charLen,
                                UDWORD      *byteLen,
                                const void  *source,
                                UWORD       *sqlStatePtr);

/*!*********************************************************************
  Function: pa80CopyString

  description: Copies a string into a buffer of given length.
               Length buffers have type SWORD.

  If the length of source +1 (null-terminator) is bigger than maxLen then
  only maxLen characters are copied, *sqlStatePtr is set to 01004
  (string data right truncated) and SQL_SUCCESS_WITH_INFO is returned.
  If the dest buffer is big enough for source the function returns
  SQL_SUCCESS.

  arguments:
    VOID    *source      [in]   The string that is copied.
    SWORD    maxLen      [in]   The length of *dest.
    UCHAR   *dest        [out]  Points to the target buffer.
    SWORD   *totalLen    [out]  Points to a variable that holds the total
                                length of source.
    UWORD   *sqlStatePtr [out]  Points a sql-state variable
    
  return value: RETCODE (SQL_SUCCESS or SQL_SUCCESS_WITH_INFO)
  */

RETCODE pa80CopyString( const char  *source,
                        SWORD  maxLen,
                        char   *dest,
                        SWORD  *totalLen,
                        UWORD  *sqlStatePtr );


/*!*********************************************************************
  Function: pa80CopyStringD

  description: Copies a string into a buffer of given length.
               Length buffers have type SDWORD.

  If the length of source +1 (null-terminator) is bigger than maxLen then
  only maxLen characters are copied, *sqlStatePtr is set to 01004
  (string data right truncated) and SQL_SUCCESS_WITH_INFO is returned.
  If the dest buffer is big enough for source the function returns
  SQL_SUCCESS.

  arguments:
    UCHAR   *source      [in]   The string that is copied.
    SDWORD   maxLen      [in]   The length of *dest.
    UCHAR   *dest        [out]  Points to the target buffer.
    SDWORD  *totalLen    [out]  Points to a variable that holds the total
                                length of source.
    UWORD   *sqlStatePtr [out]  Points a sql-state variable
    
  return value: RETCODE (SQL_SUCCESS or SQL_SUCCESS_WITH_INFO)
  */

RETCODE pa80CopyStringD( const char   *source,
                         SDWORD   maxLen,
                         char   *dest,
                         SDWORD  *totalLen,
                         UWORD   *sqlStatePtr );

/*!*********************************************************************
  Function: pa80CopyTpr05String

  description: Copies a tpr05 string into a buffer of given length.
               Length buffers have type UDWORD.

  If the length of source + null-terminator is bigger than maxLen then
  only maxLen-terminator size characters are copied and terminated,
  *sqlStatePtr is set to 01004
  (string data right truncated) and SQL_SUCCESS_WITH_INFO is returned.
  If the dest buffer is big enough for source the function returns
  SQL_SUCCESS.

  arguments:
    VOID           *dest        [out]  Points to the target buffer.
    UDWORD          maxLen      [in]   The length of *dest.
    UDWORD         *totalLen    [out]  Points to a variable that holds the total
                                       length of source.
    tpr05_String   *source      [in]   The string that is copied.
    UWORD          *sqlStatePtr [out]  Points a sql-state variable
    
  return value: RETCODE (SQL_SUCCESS or SQL_SUCCESS_WITH_INFO)
  */
RETCODE pa80CopyTpr05String( VOID          *dest,
                             UDWORD         maxLen,
                             UDWORD        *totalLen,
                             tpr05_String  *source,
                             UWORD         *sqlStatePtr);

/* wrapper for sp78convertString: source string contains always
   a terminator, even then, the source buffer length is not sufficient.
*/
API_RETCODE pa80convertString (const tsp77encoding *destEncoding,
                               void                *destBuffer,
                               tsp00_Uint4          destBufferLength,
                               tsp00_Uint4         *destBytesWritten,
                               const tsp77encoding *srcEncoding,
                               const void          *srcBuffer,
                               tsp00_Uint4          srcBufferLength);

/* pa80ODBCtoEncoding writes tpr05 string into buffer according
   to encoding type */
API_RETCODE pa80ODBCtoEncoding (tpr05_StringEncoding encodingType,
                                PTR          *buffer,
                                tsp00_Uint4   maxBufLen,
                                tsp00_Uint4  *usedBufLen,
                                tpr05_String *dest);

/* pa80ODBCtoC() - API miscellaneous function,
   copy ODBC string to C (tpr05_String, ASCII) 
   additional \0 is appended */
API_RETCODE pa80ODBCtoC ( tpr05_String        **dest,
                          SQLCHAR                *src,
                          SQLSMALLINT           srcLen);

API_RETCODE pa80ODBCtoTpr05 ( tpr05_String        *dest,
                              const tsp77encoding *srcEncoding,
                              VOID                *src,
                              SQLINTEGER           srcLen);

/* pa80ODBCtoCW() - API miscellaneous function,
   copy ODBC string to C (tpr05_String, UCS2native)
   additional terminator is appended */
API_RETCODE pa80ODBCtoCW ( tpr05_String **dest,
                           SQLWCHAR     *src,
                           SQLSMALLINT   srcLen);

/* pa80containsWildCard - - API miscellaneous function,
   returns true, if the character '%' or '_' is contained in string */
int pa80containsWildCard (const void *string,
                         long length,
                         const tsp77encoding *srcEncoding);

/*!**********************************************************************

  EndChapter: exported functions

************************************************************************/

#endif
