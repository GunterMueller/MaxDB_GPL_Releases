/*!**********************************************************************

  module: vwd12hpool.c

  -----------------------------------------------------------------------

  responsible:  Holger Bischoff
				Markus Oezgen

  special area: WebAgent

  description:  Handle pool

  see also:     WebAgent user manual

  -----------------------------------------------------------------------

  copyright:               Copyright (c) 2000-2005 SAP AG




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




************************************************************************/

/*=======================================================================
 *  INCLUDES
 *=====================================================================*/

#include "vpr09DynaDesc.h"

#include "hwd12hpool.h"
#include "hwd27excl.h"

/*!**********************************************************************

  Chapter: twd12HandlePool

************************************************************************/

#define ALLOC_INCREMENT 5

#define NO_POOLING

struct st_wa_handle_pool
{
    twd12Constructor  constructor;
    twd12Destructor   destructor;
    
    tpr09DynamicDescriptor *handles;
    
    twd27ExclP              excl;	
}; /* struct st_wa_handle_pool */


twd12HandlePoolP wd12CreateHandlePool( twd12Constructor  constructor,
                                       twd12Destructor   destructor )
{
    sapdbwa_Bool        allocatOK = true;
    twd12HandlePoolP  newPool = NULL;

    sapdbwa_SQLALLOCAT( sizeof( struct st_wa_handle_pool),
                   (sapdbwa_UInt1**) &newPool,
                   &allocatOK );
    if (allocatOK) {
        newPool->constructor  = constructor;
        newPool->destructor   = destructor;
        if (!wd27InitExcl( &newPool->excl )) {
            sapdbwa_SQLFREE( (sapdbwa_UInt1*) newPool );
            newPool = NULL;
        } else {
            newPool->handles      = pr09NewDescriptor( ALLOC_INCREMENT,
                                                   sizeof(void*) );
            if (newPool->handles == NULL) {
                /* memory allocation failed */
                wd27ExitExcl( newPool->excl );
                sapdbwa_SQLFREE( (sapdbwa_UInt1*) newPool );
                newPool = NULL;
            };
        };
    }; /* if allocatOK */

    return newPool;
} /* wd12CreateHandlePool */


void wd12DestroyHandlePool( twd12HandlePoolP hpool )
{
    void **actHandle = NULL;

    wd27BegExcl( hpool->excl );
    pr09SetIndex( hpool->handles, Start_epr09 );
    while ( (actHandle = pr09GetNextItem( hpool->handles )) != NULL) {
        hpool->destructor( *actHandle );
        *actHandle = NULL;
    };
    pr09DeleteDescriptor( hpool->handles );
    wd27EndExcl( hpool->excl );
    wd27ExitExcl( hpool->excl );
    sapdbwa_SQLFREE( (sapdbwa_UInt1*) hpool );
} /* wd12DestroyHandlePool */


void *wd12GetHandle( twd12HandlePoolP hpool )
{
    void **phandle = NULL;
    void  *handle  = NULL;
    
#ifdef NO_POOLING
    handle = hpool->constructor();
#else    
    wd27BegExcl( hpool->excl );
    phandle = pr09GetItemEx( hpool->handles, 0 );
    if (phandle == NULL) {
        /* No handles available. Create new one. */
        handle = hpool->constructor();
    } else {
        handle = *phandle;
        pr09CloseItemEx( hpool->handles, 0 );
        phandle = NULL;
    };
    wd27EndExcl( hpool->excl );
#endif
    
    return handle;
} /* wd12GetHandle */


void wd12PutHandle( twd12HandlePoolP hpool, void *handle )
{
    void **phandle = NULL;
    
#ifdef NO_POOLING    
    hpool->destructor( handle );
#else
    wd27BegExcl( hpool->excl );
    phandle = pr09AddItem( hpool->handles );
    wd27EndExcl( hpool->excl );
    *phandle = handle;
#endif    
} /* wd12PutHandle */
    
/*!**********************************************************************

  EndChapter: twd12HandlePool

************************************************************************/
