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
#ifndef VPA21DESCREC_H
#define VPA21DESCREC_H
/*!**********************************************************************

  module: vpa21DescRec.h

  -----------------------------------------------------------------------

  responsible:  

  special area: ODBC-Driver

  description:  ODBC-Descriptor-Records - Datastructures and functions

  see also:     http://www.microsoft.com/data/odbc/

  -----------------------------------------------------------------------

  copyright:            Copyright (c) 1998-2005 SAP AG

************************************************************************/

/*=======================================================================
 *  INCLUDES
 *=====================================================================*/

#include "vpa00global.h"

/*!**********************************************************************

  Chapter: datastructures

************************************************************************/

typedef struct st_desc *DescPtr;

typedef struct
{
    DescPtr       parentDesc;      /* the descriptor this record belongs to */
    SWORD         conciseType;     /* Concise data-type                    */
    UCHAR        *dataPtr;         /* ptr to parameter-/column-value       */
    SWORD         datetimeIntervalCode; /* subcode for datetime/intervall  */
    SDWORD        datetimeIntervalPrecision; /* leading precision          */
    SQLLEN       *indicatorPtr;    /* ptr to NULL indicator                */
    SQLLEN        length;          /* max or actual len of data            */
    UCHAR        *name;            /* column alias/name                    */
    SWORD         nullable;        /* Null value allowed for column?       */
    SWORD         defaultValue;    /* default value defined for column?    */
    SQLLEN        octetLength;     /* length of data/size of buffer        */
    SQLLEN       *octetLengthPtr;  /* ptr to var that holds length         */
    SWORD         parameterType;   /* input or output                      */
    SWORD         precision;       /* num of digits for exact numeric types*/
    SWORD         scale;           /* scale for decimal and numeric data   */
    SWORD         type;            /* Real SQL or C data type of col/param */
    SWORD         sqlType;         /* Application defined SQL type IPD/IRD */
    SWORD         unnamed;         /* named or unnamed                     */
    SDWORD        numPrecRadix;    /* 2 for float types,
                                    * 10 for integer types,
                                    * 0 for other types                    */
    /* internal fields
     * =============== */
    /* *** fields from SQLDA  */
    UWORD            esqType;      /* ESQ data type: SQL_TYP_XX            */
    UWORD            realEsqType;  /* real ESQ data type needed for UCS2   */
    UDWORD           esqLength;    /* length in bytes                      */
    UDWORD           esqPrecision; /* precision for certain fiel           */
    UWORD            esqScale;     /* scale for certain fields             */
    SWORD            esqIO;      /* ESQ param type: INPUT | OUTPUT | INOUT */
    
    /* *** calculated fields */
    API_HANDLE       longHndl;     /* handle to hold long data             */
    SWORD            lastData;     /* last part of long column read        */
    SWORD            convRetcode;  /* returncode from convesion (apgput)   */
    SWORD            isNull;       /* is now NULL: true/false              */
    UCHAR           *optimizePtr;  /* "hard" pointer that is not           */
    UDWORD           movedLen;     /* always correct!!                     */
    tsp00_Int4      *optimizeNullPtr; /* "hard" pointer to null ind        */
    /* *** fields from bind */
    SWORD            bound;        /* bound: true/false                    */
    UDWORD           alreadyRead;  /* already read bytes (for long data)   */
} tpa21DescRecord;


/*!**********************************************************************

  Chapter: exported functions

************************************************************************/

/*!*********************************************************************
  Function: pa21GetLongAddr

  description: Return the address of the hostvar for LONG values

  arguments:
    varaddr      [out]  Buffer to return the address.
    impl_rec_ptr [in]   IPD or IRD
    
  return value: */
  
API_RETCODE pa21GetLongAddr( UCHAR           **varaddr,
                             tpa21DescRecord  *impl_rec_ptr );


/*!*********************************************************************
  Function: pa21InitDescRecord

  description: Set default values for the descriptor record.

  arguments:
    rec_ptr [in]   Record that should be initialized
    
  return value: void */
  
void pa21InitDescRecord( tpa21DescRecord *rec_ptr,
                         DescPtr          parentDesc );


/*!*********************************************************************
  Function: pa21PutRec

  description: Put values from user fields to sqlda areas.

  arguments:
    appl_rec_ptr [in]   ARD or APD
    impl_rec_ptr [in]   IRD or IPD
    sqlmode      [in]   for handling empty string as NULL in Oracle
    
  return value: API_OK, API_NOT_OK, API_TRUNCATE, API_DATA_LOSS, ...*/
  
API_RETCODE pa21PutRec( tpa21DescRecord *appl_desc_rec_ptr,
                        tpa21DescRecord *impl_desc_rec_ptr,
                        UWORD            sqlmode );


/*!*********************************************************************
  Function: pa21SetCType

  description: Set C-datatype in the application descriptor.

  For non datetime or interval datatypes the fields conciseType and type
  are set.
  For datetime or interval datatypes the fields datetimeIntervalCode is
  also set as described in ODBC-reference 3.0 p. 927.
  
  arguments:
    tpa21DescRecord *applDescPtr  [in/out]  The ARD/APD-record
    SWORD      cType        [in]      The C-datatype (SQL_C_...)
    
  return value: VOID
  */

VOID pa21SetCType( tpa21DescRecord *applRecPtr, SWORD cType );


/*!*********************************************************************
  Function: pa21SetCType

  description: Set the internal hostvartype use ODBC Types

  arguments:
    desc_rec_ptr      [in/out]  The descriptor-record
    SWORD hostvartype [out]     
    collength         [out]
  return value: VOID */

API_RETCODE pa21SetHostVarType( tpa21DescRecord *desc_rec_ptr,
                                SWORD           *hostvartype,
                                UDWORD          *collength);


/*!*********************************************************************
  Function: pa21SetSQLType

  description: Set SQL-datatype in the application descriptor record.

  For non datetime or interval datatypes the fields conciseType and type
  are set.
  For datetime or interval datatypes the fields datetimeIntervalCode is
  also set as described in ODBC-reference 3.0 p. 927.

  arguments:
    tpa21DescRecord *implRecPtr   [in/out]  The IRD/IPD-record
    SWORD            sqlType      [in]      The SQL-datatype (SQL_...)
    
  return value: VOID
  */

VOID pa21SetSQLType( tpa21DescRecord *implRecPtr, SWORD cType );

#endif
