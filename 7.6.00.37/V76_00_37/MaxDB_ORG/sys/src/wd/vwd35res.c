/*!**********************************************************************

  module: vwd35res.c

  -----------------------------------------------------------------------

  responsible:  Holger Bischoff
				Markus Oezgen

  special area: WebAgent

  description:  Resource Pool

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

#include "hwd35res.h"

/*!**********************************************************************

  Chapter: tws35ResourcePoolP

************************************************************************/

struct st_semaphore {
    teo07_ThreadSemaphore	semaphore;
};

typedef struct
{
    void       *data;
    tsp00_Bool  available;
} twd35Resource;

/*===========================================================================*/

void wd35_SearchNextFreeElem( twd35ResourcePoolP pool );

tsp00_Bool wd35_SearchNextFreeElemNotNull( twd35ResourcePoolP  pool,
                                           twd35_ResourceId   *resId );

/*---------------------------------------------------------------------------*/

sapdbwa_Bool Semaphore_Create( twd35SemaphoreP *hSemaphore );

sapdbwa_Bool Semaphore_Destroy( twd35SemaphoreP hSemaphore );

sapdbwa_Bool Semaphore_Begin( twd35SemaphoreP hSemaphore );

sapdbwa_Bool Semaphore_End( twd35SemaphoreP hSemaphore );

/*===========================================================================*/

sapdbwa_Bool Semaphore_Create( twd35SemaphoreP *hSemaphore )
{

    tsp00_ErrTextc	   errorText;
    teo07_ThreadErr	   error;
    tsp00_Bool		   bOk;
    twd35SemaphoreP    hNewSemaphore = NULL;

    sapdbwa_allocat(sizeof(struct st_semaphore), (sapdbwa_UInt1	**)&hNewSemaphore, &bOk);
    if (!bOk) {
        return sapdbwa_False;
    }

    memset(hNewSemaphore, 0, sizeof(struct st_semaphore));

    sqlcreatesem(&hNewSemaphore->semaphore, 0, errorText, &error);
    if (error != THR_OK_EO07) {
        sapdbwa_free((sapdbwa_UInt1	*)hNewSemaphore);

        return sapdbwa_False;
    }

    *hSemaphore = hNewSemaphore;

    return sapdbwa_True;

}

/*===========================================================================*/

sapdbwa_Bool Semaphore_Destroy( twd35SemaphoreP hSemaphore )
{

    sqldestroysem(hSemaphore->semaphore);
    sapdbwa_free((sapdbwa_UInt1	*)hSemaphore);

    return sapdbwa_True;

}

/*===========================================================================*/

sapdbwa_Bool Semaphore_Begin( twd35SemaphoreP hSemaphore )
{

    sqlwaitsem(hSemaphore->semaphore);

    return sapdbwa_True;

}

/*===========================================================================*/

sapdbwa_Bool Semaphore_End( twd35SemaphoreP hSemaphore )
{

    sqlsignalsem(hSemaphore->semaphore);

    return sapdbwa_True;

}

/*===========================================================================*/

twd35ResourcePoolP wd35CreateResourcePool( twd35_ResourceId size )
{

    tsp00_Bool          allocatOK = true;
    twd35ResourcePoolP  newPool = NULL;

    sapdbwa_SQLALLOCAT(sizeof( struct st_wa_resource_pool),
                      (sapdbwa_UInt1**) &newPool, &allocatOK);
    if (allocatOK) {
        if (!wd27InitExcl( &newPool->poolExcl )) {
            sapdbwa_SQLFREE( (sapdbwa_UInt1*) newPool );
            return NULL;
        }

        if (!Semaphore_Create(&newPool->waitForConnectionQueue)) {
            sapdbwa_SQLFREE( (sapdbwa_UInt1*) newPool );
            return NULL;
        }

        newPool->openConnections = 0;
        newPool->maxRes    = (size >= 0) ? size : MAX_INT4_SP00;
        newPool->lowestFreeRes = 0;
        newPool->resources = pr09NewDescriptor( 5, sizeof( twd35Resource ) );
        if (newPool->resources == NULL) {
            /* memory allocation failure */
            wd35DestroyResourcePool( newPool );
            sapdbwa_SQLFREE( (sapdbwa_UInt1*) newPool );
            return NULL;
        };
    };

    return newPool;

}

/*===========================================================================*/

void wd35DestroyResourcePool( twd35ResourcePoolP  pool )
{

    if (pool != NULL) {
        wd27BegExcl( pool->poolExcl );

        if (pool->resources != NULL) {
            pr09DeleteDescriptor( pool->resources );
        };

        Semaphore_Destroy(pool->waitForConnectionQueue);

        wd27EndExcl( pool->poolExcl );    
        wd27ExitExcl( pool->poolExcl );

        sapdbwa_SQLFREE( (sapdbwa_UInt1*) pool );
    };

}

/*===========================================================================*/

tsp00_Bool wd35LockElem( twd35ResourcePoolP   pool,
                         void               **res,
                         twd35_ResourceId    *id )
{
    tsp00_Bool        retcode = true;
    twd35_ResourceId  freeId  = 0;
    twd35Resource    *freeRes = NULL;
    
    wd27BegExcl(pool->poolExcl);

    if (pool->openConnections == pool->maxRes) {
        /* Go into the queue          */
        wd27EndExcl(pool->poolExcl);
        if (!Semaphore_Begin(pool->waitForConnectionQueue)) {
            return sapdbwa_False;
        }
    
        wd27BegExcl( pool->poolExcl );
    }

    if (pool->lowestFreeRes < pool->maxRes) {
		freeId = pool->lowestFreeRes;
        if (pr09GetItemCount( pool->resources ) == freeId) {
            /* create new resource */
            freeRes = pr09AddItem( pool->resources );
            if (freeRes != NULL) {
                freeRes->data = NULL;
                wd35_SearchNextFreeElem( pool );
            }; /* if */
        } else {
            freeRes = pr09GetItemEx( pool->resources, freeId );
            /* find free elem for the next LockElem */
            wd35_SearchNextFreeElem( pool );
            if (freeRes->data == NULL) {
                /* try to find elem with data != NULL */
                twd35_ResourceId  resId = 0;
                
                if (wd35_SearchNextFreeElemNotNull( pool, &resId )) {
                    freeRes = pr09GetItemEx( pool->resources, resId );
					freeId = resId;
                }; /* if */
            }; /* if */
        }; /* else */
        retcode = (freeRes != NULL);
        if (retcode) {
            freeRes->available = false;
            *res = freeRes->data;
            *id  = freeId;
        }; /* if */
    } else {
        /* no free resource available */
        retcode = false;
    }; /* else */
    wd27EndExcl( pool->poolExcl );
    
    return retcode;
} /* wd35LockElem */


tsp00_Bool wd35GetElemByIndex( twd35ResourcePoolP   pool,
                               twd35_ResourceId     id,
                               void               **res )
{
    tsp00_Bool retcode = true;
    twd35Resource *resElem = NULL;

    wd27BegExcl( pool->poolExcl );
    resElem = pr09GetItemEx( pool->resources,
                             (sapdbwa_Int4) id );
    wd27EndExcl( pool->poolExcl );
    if (resElem != NULL) {
        *res = resElem->data;
    } else {
        retcode = false;
        *res = NULL;
    }; /* else */

    return retcode;
} /* wd35GetElemByIndex */


tsp00_Bool wd35SetElemByIndex( twd35ResourcePoolP   pool,
                               void                *res,
                               twd35_ResourceId     id )
{
    tsp00_Bool retcode = true;
    twd35Resource *resElem = NULL;

    wd27BegExcl( pool->poolExcl );
    resElem = pr09GetItemEx( pool->resources,
                             (sapdbwa_Int4) id );
    wd27EndExcl( pool->poolExcl );
    if (resElem != NULL) {
        resElem->data = res;
    } else {
        retcode = false;
    }; /* else */

    return retcode;
} /* wd35SetElemByIndex */


sapdbwa_UInt4 wd35GetNumAllocElems( twd35ResourcePoolP   pool )
{
    return pr09GetItemCount( pool->resources );
} /* wd35GetNumAllocElems */


void wd35UnlockElem( twd35ResourcePoolP  pool,
                     twd35_ResourceId    id )
{
    twd35Resource *resElem = NULL;
    
    wd27BegExcl( pool->poolExcl );
    resElem = pr09GetItemEx( pool->resources, id );
    if (resElem != NULL) {
        resElem->available = true;
        if (id < pool->lowestFreeRes) {
            pool->lowestFreeRes = id;
        }; /* if */

        if (pool->openConnections == pool->maxRes) {
            /* Signal free connection */
            Semaphore_End(pool->waitForConnectionQueue);
        }
    }; /* if */
    wd27EndExcl( pool->poolExcl );    
} /* wd35UnlockElem */

/*!**********************************************************************

  EndChapter: twd35ResourcePoolP

************************************************************************/

void wd35_SearchNextFreeElem( twd35ResourcePoolP pool )
{
    twd35Resource *resElem = NULL;

    pool->lowestFreeRes++;
    resElem = pr09GetItemEx( pool->resources,
                             pool->lowestFreeRes );
    /* find the next free elem with the lowest index */
    while ((pool->lowestFreeRes < pool->maxRes)
           && (resElem != NULL)
           && (resElem->available == false) ) {
        pool->lowestFreeRes++;
        resElem = pr09GetItemEx( pool->resources,
                                 pool->lowestFreeRes );    
    }; /* while */
} /* wd35_SearchNextFreeElem */


tsp00_Bool wd35_SearchNextFreeElemNotNull( twd35ResourcePoolP  pool,
                                           twd35_ResourceId   *resId )
{
    tsp00_Bool        retcode = false;
    twd35Resource    *resElem = pr09GetItemEx( pool->resources,
                                               pool->lowestFreeRes );
    twd35_ResourceId  freeElemNotNull = pool->lowestFreeRes;
    
    /* find the next free elem with the lowest index and with data != NULL */
    while ((freeElemNotNull < pr09GetItemCount( pool->resources ))
           && (resElem != NULL)
           && (resElem->available == false || resElem->data == NULL)) {
        freeElemNotNull++;
        resElem = pr09GetItemEx( pool->resources,
                                 freeElemNotNull );    
    }; /* while */
    if (resElem != NULL && resElem->available && resElem->data != NULL) {
        retcode = true;
        *resId  = freeElemNotNull;
    }; /* if */
        
    return retcode;
} /* wd35_SearchNextFreeElem */

