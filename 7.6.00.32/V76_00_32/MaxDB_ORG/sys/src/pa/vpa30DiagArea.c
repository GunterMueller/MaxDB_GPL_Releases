/*!**********************************************************************

  module: vpa30DiagArea.c

  -----------------------------------------------------------------------

  responsible:  

  special area: ODBC-Driver

  description:  diagnostic-Area

  see also:     http://www.microsoft.com/data/odbc/

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

#include "vpa30DiagArea.h"


/*!**********************************************************************

  prototypes and constants for internal functions

************************************************************************/

/*!
  Function: pa30_OrderFunction

  description: Defines the order of diagnistic records.

  arguments:
    void *diagRec1  [in]  First diagnostic record to compare
    void *diagRec2  [in]  Second diagnostic record to compare
    
  return value:
    - -1 if diagRec1<diagRec2
    - 0 if diagRec1==diagRec2
    - 1 if diagRec1>diagRec2

 This function is used to order the diagnostic records as described in
 the ODBC refernce manual:

 "If two or more status records are returned, the Driver Manager and
 driver rank them according to the following rules. The record with
 the highest rank is the first record. The source of a record (Driver
 Manager, driver, gateway, and so on) is not considered when ranking
 records.

   · Errors. Status records that describe errors have the highest
     rank. Among error records, records that indicate a transaction
     failure or possible transaction failure outrank all other
     records. If two or more records describe the same error
     condition, then SQLSTATEs defined by the X/Open CLI specification
     (classes 03 through HZ) outrank ODBC- and driver-defined
     SQLSTATEs.

   · Implementation-defined No Data values. Status records that
     describe driver-defined No Data values (class 02) have the second
     highest rank.

   · Warnings. Status records that describe warnings (class 01) have
     the lowest rank. If two or more records describe the same warning
     condition, then warning SQLSTATEs defined by the X/Open CLI
     specification outrank ODBC- and driver-defined SQLSTATEs.

  If there are two or more records with the highest rank, it is
  undefined which record is the first record. The order of all other
  records is undefined. In particular, because warnings may appear
  before errors, applications should check all status records when a
  function returns a value other than SQL_SUCCESS."  */

int pa30_OrderFunction( const void *diagRec1,
                        const void *diagRec2 );


/*!**********************************************************************

  Chapter: exported functions

************************************************************************/

tpa31DiagRec* pa30AddDiagRec( tpa30DiagArea *diagArea )
{
    tpa31DiagRec *newDiagRec;
    
    API_TRACE( API_TR_ENTRY, "pa30AddDiagRec", 0);
    API_TRACE( API_TR_PTR, "diagArea", &diagArea );

    newDiagRec = pr09AddItem( *diagArea );
    /*
    API_MEMCPY( newDiagRec, &diagRec, sizeof(tpa31DiagRec));
    */          
    API_TRACE( API_TR_EXIT, "pa30AddDiagRec", 0 );
    
    return newDiagRec;
} /* pa30AddDiagRec */


API_RETCODE pa30DeleteDiagArea( tpa30DiagArea *diagArea )
{
    API_RETCODE apiRetcode = API_OK;
    
    API_TRACE( API_TR_ENTRY, "pa30DeleteDiagArea", 0);
    API_TRACE( API_TR_PTR, "diagArea", &diagArea );

    if (*diagArea!=NULL) {
        pr09DeleteDescriptor( *diagArea );
        *diagArea = NULL;
    }; /* if */
    
    API_TRACE( API_TR_EXIT, "pa30DeleteDiagArea", 0 );
    API_TRACE( API_TR_RETCODE, "apiRetcode", &apiRetcode );
    
    return apiRetcode;
} /* pa30DeleteDiagArea */


API_RETCODE pa30FreeDiagArea( tpa30DiagArea *diagArea )
{
    API_RETCODE apiRetcode = API_OK;
    
    API_TRACE( API_TR_ENTRY, "pa30FreeDiagArea", 0);
    API_TRACE( API_TR_PTR, "diagArea", &diagArea );

    if (*diagArea!=NULL && pr09GetItemCount(*diagArea) != 0 ) {
        pr09FreeDescriptor( *diagArea );
    }; /* if */
    
    API_TRACE( API_TR_EXIT, "pa30FreeDiagArea", 0 );
    API_TRACE( API_TR_API_RETCODE, "apiRetcode", &apiRetcode );
    
    return apiRetcode;
} /* pa30FreeDiagArea */


SQLRETURN pa30GetDiagField( tpa30DiagArea *diagArea,
                            UDWORD         odbcVersion,
                            SWORD          recNo,
                            SWORD          diagId,
                            PTR            valuePtr,
                            SWORD          bufferLength,
                            SWORD         *strLengthPtr,
                            const tsp77encoding *nativeEncoding)
{
    SQLRETURN retcode = SQL_SUCCESS;
    tpa31DiagRec *diagRec;
    
    API_TRACE( API_TR_ENTRY, "pa31GetDiagField", 0);
    API_TRACE( API_TR_PTR, "diagRec", &diagRec );
    API_TRACE( API_TR_UDWORD, "odbcVersion", &odbcVersion );
    API_TRACE( API_TR_SWORD, "recNo", &recNo );
    API_TRACE( API_TR_SWORD, "diagId", &diagId );
    API_TRACE( API_TR_PTR, "valuePtr", &valuePtr );
    API_TRACE( API_TR_SWORD, "bufferLength", &bufferLength );
    API_TRACE( API_TR_PTR, "strLengthPtr", &strLengthPtr );

    switch (diagId) {
    case SQL_DIAG_NUMBER:
        *((SDWORD*) valuePtr) = pr09GetItemCount( *diagArea );
        break;
    default:
        if (recNo<=0) {
            retcode = SQL_ERROR;
        } else {
            diagRec = pa30GetDiagRecEx( diagArea, recNo );
            if (diagRec==NULL) {
                retcode = SQL_NO_DATA;
            } else {
                retcode = pa31GetDiagField( diagRec,
                                            odbcVersion,
                                            diagId,
                                            valuePtr,
                                            bufferLength,
                                            strLengthPtr,
                                            nativeEncoding);
            }; /* else */
        }; /* else */
    }; /* switch */
    
    API_TRACE( API_TR_EXIT, "pa31GetDiagField", 0 );
    API_TRACE( API_TR_RETCODE, "retcode", &retcode );
    
    return retcode;
} /* pa30GetDiagField */


tpa31DiagRec* pa30GetDiagRecEx( tpa30DiagArea *diagArea, SWORD index )
{
    tpa31DiagRec *diagRec;
    
    API_TRACE( API_TR_ENTRY, "pa30GetDiagRecEx", 0);
    API_TRACE( API_TR_PTR, "diagArea", &diagArea );
    API_TRACE( API_TR_SWORD, "index", &index );

    diagRec = pr09GetItemEx( *diagArea, index-1 );
    
    API_TRACE( API_TR_EXIT, "pa30GetDiagRecEx", 0 );
    
    return diagRec;
} /* pa30GetDiagRecEx */


API_RETCODE pa30InitDiagArea( tpa30DiagArea *diagArea )
{
    API_RETCODE apiRetcode = API_OK;
    
    API_TRACE( API_TR_ENTRY, "pa30InitDiagArea", 0);
    API_TRACE( API_TR_PTR, "diagArea", &diagArea );

    *diagArea = pr09NewDescriptor( 1, sizeof( tpa31DiagRec ) );
    
    API_TRACE( API_TR_EXIT, "pa30InitDiagArea", 0 );
    API_TRACE( API_TR_API_RETCODE, "apiRetcode", &apiRetcode );
    
    return apiRetcode;
} /* pa30InitDiagArea */


API_RETCODE pa30OrderDiagArea( tpa30DiagArea *diagArea )
{
    API_RETCODE apiRetcode = API_OK;
    
    API_TRACE( API_TR_ENTRY, "pa30OrderDiagArea", 0);
    API_TRACE( API_TR_PTR, "diagArea", &diagArea );

    pr09OrderItems( *diagArea, pa30_OrderFunction );
    
    API_TRACE( API_TR_EXIT, "pa30OrderDiagArea", 0 );
    API_TRACE( API_TR_RETCODE, "apiRetcode", &apiRetcode );
    
    return apiRetcode;
} /* pa30OrderDiagArea */


API_RETCODE pa30PutError( tpa30DiagArea *diagArea,
                          UWORD          sqlState,
                          tpa41ESQBlock *esqblkPtr)
{
    API_RETCODE apiRetcode = API_OK;
    tpa31DiagRec *newDiagRec;
    
    API_TRACE( API_TR_ENTRY, "pa30PutError", 0);

    newDiagRec = pa30AddDiagRec( diagArea );
    if (newDiagRec!=NULL) {
        if (esqblkPtr) {
            pa31InitDiagRec( newDiagRec,
                             sqlState,
                             esqblkPtr->esq_sql_code,
                             esqblkPtr->sql_err_text,
                             0, 0 ); /* ... row and col */
        } else {
            pa31InitDiagRec( newDiagRec, sqlState, 0, (UCHAR*) "", 0, 0 );
        }; /* else */
    } else {
        apiRetcode = API_NOT_OK;
    }; /* else */
    
    API_TRACE( API_TR_EXIT, "pa30PutError", 0);
    API_TRACE( API_TR_API_RETCODE, "apiRetcode", &apiRetcode);

    return apiRetcode;
} /* pa30PutError */

/*!**********************************************************************

  EndChapter: exported functions

************************************************************************/

/*!**********************************************************************

  Chapter: internal functions

************************************************************************/

int pa30_OrderFunction( const void *diagRec1,
                        const void *diagRec2 )
{
    /* ... */
    return 0;
} /* pa30OrderFunction */

/*!**********************************************************************

  EndChapter: internal functions

************************************************************************/

