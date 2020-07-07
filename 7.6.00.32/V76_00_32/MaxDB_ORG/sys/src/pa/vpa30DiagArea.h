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
#ifndef VPA30DIAGAREA_H
#define VPA30DIAGAREA_H
/*!**********************************************************************

  module: vpa30DiagArea.h

  -----------------------------------------------------------------------

  responsible:  

  special area: ODBC-Driver

  description:  diagnostic-Area

  see also:     http://www.microsoft.com/data/odbc/

  -----------------------------------------------------------------------

  copyright:          Copyright (c) 1998-2005 SAP AG

************************************************************************/

/*=======================================================================
 *  INCLUDES
 *=====================================================================*/

#include "vpa00global.h"
#include "hsp77.h"

#include "vpr09DynaDesc.h"
#include "vpa31DiagRec.h"
#include "vpa41ESQBlock.h"

/*!**********************************************************************

  Chapter: datastructures

  decription: typedef tpr09DynamicDescriptor* tpa30DiagArea;
************************************************************************/

/* Use ADT tpi01DynamicDescriptor */
typedef tpr09DynamicDescriptor* tpa30DiagArea;

/*!**********************************************************************

  EndChapter: datastructures

************************************************************************/

/*!**********************************************************************

  Chapter: exported functions

************************************************************************/

/*!
  Function: pa30AddDiagRec

  description: Allocates a new dignostic record.

  arguments:
    tpa30DiagArea *diagArea  [in]  Pointer to diagnistic area.
  
  return value:
    Pointer to the allocated diagnistic record or NULL if the allocation
    failed (not enough memory).
  */

tpa31DiagRec* pa30AddDiagRec( tpa30DiagArea *diagArea );


/*!*********************************************************************
  Function: pa30DeleteDiagArea

  description: Deletes the contents and the diagnistic area itself.

  arguments:
    tpa30DiagArea *diagArea  [in]  Pointer to diagnistic area.
    
  return value: always API_OK
  */

API_RETCODE pa30DeleteDiagArea( tpa30DiagArea *diagArea );

                            
/*!*********************************************************************
  Function: pa30FreeDiagArea

  description: Deletes the contents of a diagnistic area.

  arguments:
    diagArea  [in]  Pointer to diagnistic area.
      
  return value: always API_OK
  */

API_RETCODE pa30FreeDiagArea( tpa30DiagArea *diagArea );


/*!*********************************************************************
  Function: pa30GetDiagField

  description: Return a header or record field of a diagnostic area.

  arguments:
    tpa30DiagArea *diagArea    [in]  Pointer to diagnostic area
    UDWORD        odbcVersion  [in]  SQL_OV_ODBC2 or SQL_OV_ODBC3
    SWORD         recNo        [in]  Record number
    SWORD         diagId       [in]  Id for field to retrieve
    PTR           valuePtr     [out] Target for the retrieved informtion
    SWORD         bufferLength [in]  Length of *valuePtr
    SWORD        *strLengthPtr [out] Total length of information is
                                     stored in *strLengthPtr.
    tsp77encoding *nativeEnc   [in]  encoding if char data are written to
                                     valuePtr

  return value: RETCODE (SQL_SUCCESS, SQL_ERROR, SQL_SUCCES_WITH_INFO)

  This function is used by the ODBC function SQLGetDigaField to retrieve
  the information that is stored in the diagnostic header or record.
  If odbcVersion is SQL_OV_ODBC3 then ODBC 2.x states are mapped to
  3.x states.
  */

SQLRETURN pa30GetDiagField( tpa30DiagArea *diagArea,
                            UDWORD         odbcVersion,
                            SWORD          recNo,
                            SWORD          diagId,
                            PTR            valuePtr,
                            SWORD          bufferLength,
                            SWORD         *strLengthPtr,
                            const tsp77encoding *nativeEncoding);

/*!*********************************************************************
  Function: pa30GetDiagRecEx

  description: Returns diagnistic record with given index.

  arguments:
    tpa30DiagArea *diagArea  [in]  Pointer to dignostic area
    SWORD          index     [in]  Index of the record. First record
                                   has index 1.
  return value: Pointer to diagnostic record or NULL if a record with
                the given index does not exist.
  */

tpa31DiagRec* pa30GetDiagRecEx( tpa30DiagArea *diagArea, SWORD index );


/*!*********************************************************************
  Function: pa30InitDiagArea

  description: Initialization of a diagnose area.

  arguments:
    tpa30DiagArea *diagArea  [in]  Pointer to diagnostic area.
    
  return value: API_RETCODE (API_OK for success, API_NOT_OK for failure)
  */

API_RETCODE pa30InitDiagArea( tpa30DiagArea *diagArea );


/*!*********************************************************************
  Function: pa30OrderDiagArea

  description: Orders the diagnose records as described in the ODBC
               reference manual.

  arguments:
    tpa30DiagArea *diagArea  [in]  Pointer to diagnostic area.
      
  return value: API_RETCODE (API_OK for success, API_NOT_OK for failure)
  */

API_RETCODE pa30OrderDiagArea( tpa30DiagArea *diagArea );


/*!*********************************************************************
  Function: pa30PutError

  description: Create and initialize a new error block.

  arguments:
    tpa30DiagArea *diagArea  [in]  Pointer to diagnostic area.
    UWORD          sqlState  [in]  sql state id
    api_esq_block *esqblkPtr [in]  pointer to esq block or NULL
      
  return value: API_RETCODE (API_OK for success, API_NOT_OK for failure)
  */

API_RETCODE pa30PutError( tpa30DiagArea *diagArea,
                          UWORD          sqlState,
                          tpa41ESQBlock *esqblkPtr);

/*!**********************************************************************

  EndChapter: exported functions

************************************************************************/
#endif
