/*!**********************************************************************

  module: vwd11strseq.c

  -----------------------------------------------------------------------

  responsible:  Holger Bischoff
				Markus Oezgen

  special area: WebAgent

  description:  String sequence

  see also:

  -----------------------------------------------------------------------

  copyright:               Copyright (c) 1999-2005 SAP AG




    ========== licence begin  GPL
    Copyright (c) 1999-2005 SAP AG

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

#include "hwd11strseq.h"
#include "vpr09DynaDesc.h"

void wd11_FreeEntries( sapdbwa_StringSeqP stringSeq );

/*!**********************************************************************

  Chapter: sapdbwa_StringSeqP

  description:
  
************************************************************************/

struct st_wa_stringseq {
    tpr09DynamicDescriptor  *stringDesc;
}; /* st_stringseq */


sapdbwa_StringSeqP WINAPI sapdbwa_CreateStringSeq()
{
    tsp00_Bool allocatOK      = true;
    sapdbwa_StringSeqP newStringSeq = NULL;
    
    sapdbwa_SQLALLOCAT( sizeof( struct st_wa_stringseq ),
                   (sapdbwa_UInt1**) &newStringSeq, &allocatOK );
    if (allocatOK) {
        newStringSeq->stringDesc = pr09NewDescriptor( 2, sizeof( char *) );
        if ( newStringSeq->stringDesc == NULL ) {
            /* memory allocation failure */
            sapdbwa_SQLFREE( (sapdbwa_UInt1*) newStringSeq );
            newStringSeq = NULL;
        }; /* if */
    }; /* if */
    
    return newStringSeq;
} /* WACreateStringSeq */

    
void WINAPI sapdbwa_DestroyStringSeq( sapdbwa_StringSeqP seq )
{
    /*wd11_FreeEntries( sapdbwa_StringSeqP seq );*/
    pr09DeleteDescriptor( seq->stringDesc );
    sapdbwa_SQLFREE( (sapdbwa_UInt1*) seq );
} /* WADestroyStringSeq */


sapdbwa_Int4 WINAPI sapdbwa_GetNumElem( sapdbwa_StringSeqP seq )
{
    return pr09GetItemCount( seq->stringDesc );
} /* sapdbwa_GetNumElem */


const char* WINAPI sapdbwa_GetStringByIndex( sapdbwa_StringSeqP  strSeq,
                                        sapdbwa_Int4     index )
{
    const char** entry = NULL;
    
    entry = pr09GetItemEx( strSeq->stringDesc, index );
    if (entry != NULL) {
        return *entry;
    } else {
        return NULL;
    }; /* if */
} /* sapdbwa_GetElemByIndex */


tsp00_Bool wd11ContainsString( sapdbwa_StringSeqP strSeq, const char *string )
{
    sapdbwa_Int4   numElems = sapdbwa_GetNumElem( strSeq );
    char       **entry = NULL;
    sapdbwa_Int4   i;
    
    for (i = 0; i < numElems; i++ ) {
        entry = pr09GetItemEx( strSeq->stringDesc, i );
        if (strcmp( *entry, string ) == 0) {
            return true;
        }; /* if */
    }; /* for */

    return false;
} /* wd11ContainsString */


void wd11FreeElems( sapdbwa_StringSeqP strSeq )
{
    pr09FreeDescriptor( strSeq->stringDesc ); 
} /* wd11FreeElems */


tsp00_Bool wd11InsertString( sapdbwa_StringSeqP strSeq, const char *string )
{
    const char **newEntry = NULL;
    tsp00_Bool   insertOK = true;
    
    newEntry = pr09AddItem( strSeq->stringDesc );
    if (newEntry != NULL) {
        *newEntry = string;
    } else {
        insertOK = false;
    }; /* else */

    return insertOK;
} /* wd11InsertString */

/*!**********************************************************************

  EndChapter: sapdbwa_StringSeqP
  
************************************************************************/

void wd11_FreeEntries( sapdbwa_StringSeqP stringSeq )
{
    sapdbwa_Int4   numElems = sapdbwa_GetNumElem( stringSeq );
    char       **entry = NULL;
    sapdbwa_Int4   i;
    
    for (i = 0; i < numElems; i++ ) {
        entry = pr09GetItemEx( stringSeq->stringDesc, i );
        pr09CloseItemEx( stringSeq->stringDesc, i );
        sapdbwa_SQLFREE( (sapdbwa_UInt1*) *entry );
    }; /* for */
} /* wd11_FreeEntries */

