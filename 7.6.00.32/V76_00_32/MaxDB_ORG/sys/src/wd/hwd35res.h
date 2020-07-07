/*!**********************************************************************

  module: hwd35res.h

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

#ifndef HWD35RES_H
#define HWD35RES_H

/*=======================================================================
 *  INCLUDES
 *=====================================================================*/

#include "gwd00.h"

#include "vpr09DynaDesc.h"
#include "hwd27excl.h"
#include "heo07.h"

/*!**********************************************************************

  Chapter: tws35ResourcePoolP

  description:
  
************************************************************************/

typedef struct st_wa_resource_pool *twd35ResourcePoolP;
typedef struct st_semaphore        *twd35SemaphoreP;

struct st_wa_resource_pool
{
    tpr09DynamicDescriptor *resources;      /* "array" of twd35Resource     */

    sapdbwa_Int4            maxRes;         /* max number of resources      */
    sapdbwa_Int4            lowestFreeRes;  /* the next free resource       */
    sapdbwa_Int4            openConnections;/* Already opened connections   */
    
    twd27ExclP              poolExcl;
    twd35SemaphoreP         waitForConnectionQueue;
}; /* struct st_wa_resource_pool */

typedef sapdbwa_Int4 twd35_ResourceId;

twd35ResourcePoolP wd35CreateResourcePool( twd35_ResourceId  maxSize );


void wd35DestroyResourcePool( twd35ResourcePoolP  pool );


sapdbwa_Bool wd35LockElem( twd35ResourcePoolP   pool,
                         void               **res,
                         twd35_ResourceId    *id );


sapdbwa_Bool wd35GetElemByIndex( twd35ResourcePoolP   pool,
                               twd35_ResourceId     id,
                               void               **res );


sapdbwa_Bool wd35SetElemByIndex( twd35ResourcePoolP   pool,
                               void                *res,
                               twd35_ResourceId     id );

                              
sapdbwa_UInt4 wd35GetNumAllocElems( twd35ResourcePoolP   pool );


void wd35UnlockElem( twd35ResourcePoolP  pool,
                     twd35_ResourceId    id );

/*!**********************************************************************

  EndChapter: twd35ResourcePoolP

************************************************************************/

#endif
