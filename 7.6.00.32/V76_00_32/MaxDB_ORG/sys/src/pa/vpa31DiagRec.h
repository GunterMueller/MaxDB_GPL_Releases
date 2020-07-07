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
#ifndef VPA31DIAGREC_H
#define VPA31DIAGREC_H
/*!**********************************************************************

  module: vpa20DiagRec.h

  -----------------------------------------------------------------------

  responsible:  

  special area: ODBC-Driver

  description:  diagnostic record

  see also:     http://www.microsoft.com/data/odbc/

  -----------------------------------------------------------------------

  copyright:            Copyright (c) 1998-2005 SAP AG

************************************************************************/

/*=======================================================================
 *  INCLUDES
 *=====================================================================*/

#include "vpa00global.h"
#include "hsp77.h"

/*!**********************************************************************

  Chapter: datastructures
  
  description: tpa31DiagRec
  
************************************************************************/

typedef struct
{
    SDWORD nativeErr;                 /* ESQ SQLCODE value          */
    UCHAR  messageText[PA_ERROR_TEXT_LEN + 1];
    UWORD  sqlState;                  /* key to sqlstate            */
    SDWORD colNumber;
    SDWORD rowNumber;
} tpa31DiagRec;

/*!**********************************************************************

  EndChapter: datastructures

************************************************************************/

/*!**********************************************************************

  Chapter: exported functions

************************************************************************/

/*!
  Function: pa31GetDiagField

  description: Returns a record field.
  
  This function is used by pa30GetDigaField to retrieve the
  information that is stored in a diagnostic record.  If odbcVersion
  is SQL_OV_ODBC3 then ODBC 2.x states are mapped to 3.x states.

  arguments:
    tpa31DiagRec *diagRec      [in]  Pointer to diagnostic record
    UDWORD        odbcVersion  [in]  SQL_OV_ODBC2 or SQL_OV_ODBC3
    SWORD         diagId       [in]  Id for field to retrieve
    PTR           valuePtr     [out] Target for the retrieved informtion
    SWORD         bufferLength [in]  Length of *valuePtr
    SWORD        *strLengthPtr [out] Total length of information is
    tsp77encoding *nativeEnc   [in]  encoding if char data are written to
                                     valuePtr
      
  return value: RETCODE (SQL_SUCCESS, SQL_ERROR, SQL_SUCCES_WITH_INFO)
*/

SQLRETURN pa31GetDiagField( tpa31DiagRec *diagRec,
                            UDWORD        odbcVersion,
                            SWORD         diagId,
                            PTR           valuePtr,
                            SWORD         bufferLength,
                            SWORD        *strLengthPtr,
                            const tsp77encoding *nativeEncoding);
                              
/*!**********************************************************************
  Function: pa31GetDiagRec

  description: Retrieve some record-field

  This function is used by SQLGetDiagRec.

  arguments:
    tpa31DiagRec *diagRec       [in]  Pointer to diagnostic record
    UDWORD        odbcVersion   [in]  SQL_OV_ODBC2 or SQL_OV_ODBC3
    UCHAR        *sqlStatePtr   [out] Target buffer for sql state string
    SDWORD       *nativeErrPtr  [out] Target for native error code
    UCHAR        *messageText   [out] Target for error messge
    SWORD         bufferLength  [in]  Length of *messageText
    SWORD        *textLengthPtr [out] Target for total length of error
                                      message.
    tsp77encoding *nativeEnc    [in]  encoding if char data are written to
                                      valuePtr
      
  return value: RETCODE (SQL_SUCCESS, SQL_ERROR, SQL_SUCCES_WITH_INFO)
  */

API_RETCODE pa31GetDiagRec( tpa31DiagRec *diagRec,
                            UDWORD        odbcVersion,
                            UCHAR        *sqlStatePtr,
                            SDWORD       *nativeErrPtr,
                            UCHAR        *messageText,
                            SWORD         bufferLength,
                            SWORD        *textLengthPtr,
                            const tsp77encoding *nativeEncoding);

/*!**********************************************************************
  Function: pa31InitDiagRec

  description: Initialize diagnostic record with given values.

  arguments:
    tpa31DiagRec *diagRec     [in]  Pointer to diagnistic record
    UWORD         sqlState    [in]  sql state id (API_ODBC_...)
    SDWORD        nativeErr   [in]  Native error code
    UCHAR        *messageText [in]  Error message text
    SDWORD        colNumber   [in]  Column number
    SDWORD        rowNumber   [in]  Row number
    
  return value: API_RETCODE (always API_OK)
  */

API_RETCODE pa31InitDiagRec( tpa31DiagRec *diagRec,
                             UWORD         sqlState,
                             SDWORD        nativeErr,
                             UCHAR        *messageText,
                             SDWORD        colNumber,
                             SDWORD        rowNumber );

/*!**********************************************************************

  EndChapter: exported functions

************************************************************************/
#endif




