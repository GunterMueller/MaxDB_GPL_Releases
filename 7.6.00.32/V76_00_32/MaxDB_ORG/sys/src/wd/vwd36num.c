/*!**********************************************************************

  module: vwd36num.c

  -----------------------------------------------------------------------

  responsible:  Holger Bischoff
				Markus Oezgen

  special area: WebAgent

  description:  Get "exclusive" numbers.

  see also:

  -----------------------------------------------------------------------

  copyright:              Copyright (c) 1999-2005 SAP AG




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

#include "vpr09DynaDesc.h"
#include "hwd27excl.h"
#include "hwd36num.h"

/*!**********************************************************************

  Chapter: twd36NumPoolP

************************************************************************/

typedef struct st_wa_num_elem
{
    sapdbwa_Bool available;
} twd36NumElem;


struct st_wa_num_pool
{
    twd27ExclP              poolExcl;

    sapdbwa_Int4              maxNum;
    sapdbwa_Int4              lowestFreeElem;
    tpr09DynamicDescriptor *elems;
}; /* struct st_wa_num_pool */


void wd36_SearchNextFreeItem( twd36NumPoolP pool );


twd36NumPoolP wd36CreateNumPool( sapdbwa_Int4  maxNum )
{
    sapdbwa_Bool     allocatOk = true;
    twd36NumPoolP  newPool = NULL;

    sapdbwa_SQLALLOCAT( sizeof( struct st_wa_num_pool ),
                   (sapdbwa_UInt1**) &newPool,
                   &allocatOk );
    if (allocatOk) {
        newPool->maxNum         = (maxNum >= 0) ? maxNum : MAX_INT4_SP00;
        newPool->lowestFreeElem = 0;
        if (wd27InitExcl( &newPool->poolExcl )) {
            newPool->elems = pr09NewDescriptor( 5, sizeof( twd36NumElem ) );
            if (newPool->elems == NULL) {
                wd27ExitExcl( newPool->poolExcl );
                sapdbwa_SQLFREE( (sapdbwa_UInt1*) newPool );
                newPool = NULL;
            }; /* if */
        } else {
            sapdbwa_SQLFREE( (sapdbwa_UInt1*) newPool );
            newPool = NULL;
        }; /* else */
    }; /* if */

    return newPool;
} /* wd36CreateNumPool */


void wd36DestroyNumPool( twd36NumPoolP  pool )
{
    wd27BegExcl( pool->poolExcl );
    pr09DeleteDescriptor( pool->elems );
    wd27EndExcl( pool->poolExcl );
    wd27ExitExcl( pool->poolExcl );    
    sapdbwa_SQLFREE( (sapdbwa_UInt1*) pool );    
} /* wd36DestroyNumPool */


sapdbwa_Bool wd36GetNumber( twd36NumPoolP  pool,
                          sapdbwa_Int4    *id )
{
    twd36NumElem *numElem = NULL;
    
    wd27BegExcl( pool->poolExcl );
    if (pool->lowestFreeElem < pool->maxNum) {
        /* we have a free number */
        if (pr09GetItemCount( pool->elems ) == pool->lowestFreeElem) {
            numElem = pr09AddItem( pool->elems );
        } else {
            numElem = pr09GetItemEx( pool->elems, pool->lowestFreeElem );
        }; /* else */
    }; /* if */
    if (numElem != NULL) {
        numElem->available = false;
        *id = pool->lowestFreeElem;
        wd36_SearchNextFreeItem( pool );
	}; /* if */
    wd27EndExcl( pool->poolExcl );

    return (numElem != NULL);
} /* wd36GetNumber */


sapdbwa_Bool wd36PutNumber( twd36NumPoolP  pool,
                          sapdbwa_Int4     id )
{
    twd36NumElem *numElem = NULL;
    sapdbwa_Bool retcode    = true;
    
    wd27BegExcl( pool->poolExcl );
    numElem = pr09GetItemEx( pool->elems, id );
     if (numElem != NULL) {
        numElem->available = true;
        if (id < pool->lowestFreeElem) {
            pool->lowestFreeElem = id;
        }; /* if */
    } else {
        retcode = false;
    }; /* else */
    wd27EndExcl( pool->poolExcl );

    return retcode;
} /* wd36PutNumber */


/*!**********************************************************************

  EndChapter: twd36NumPoolP

************************************************************************/

void wd36_SearchNextFreeItem( twd36NumPoolP pool )
{
    twd36NumElem *numElem = NULL;

    pool->lowestFreeElem++;
    numElem = pr09GetItemEx( pool->elems,
                             pool->lowestFreeElem );
    while ((pool->lowestFreeElem < pool->maxNum)
           && (numElem != NULL)
           && (numElem->available == false) ) {
        pool->lowestFreeElem++;
        numElem = pr09GetItemEx( pool->elems, pool->lowestFreeElem );
    }; /* while */
} /* wd36_SearchNextFreeItem */
