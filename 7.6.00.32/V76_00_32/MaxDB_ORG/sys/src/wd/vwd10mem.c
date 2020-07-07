/*!**********************************************************************

  module: vwd10mem.c

  -----------------------------------------------------------------------

  
  responsible:  Holger Bischoff
				Markus Oezgen

  special area: WebAgent

  description:  Memory routines for WebAgent

  see also:

  -----------------------------------------------------------------------

  copyright:                Copyright (c) 1999-2005 SAP AG




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

#include <stdlib.h>

#ifdef WIN32
#define _DEBUG
#include <crtdbg.h>
#endif

#include "geo102.h"
#include "hwd10mem.h"

static wd11AllocCount = 0;

void sapdbwa_allocat( size_t              lSize,
					  sapdbwa_UInt1		**ppucObjPtr,
					  tsp00_BoolAddr      pbOk		)
{
    wd11AllocCount++;
    *ppucObjPtr = malloc( lSize );
    *pbOk = (tsp00_Bool) (ppucObjPtr != NULL);
}


void sapdbwa_reallocat( size_t            lSize,
						sapdbwa_UInt1	**ppucObjPtr,
						tsp00_BoolAddr    pbOk		)
{
    *ppucObjPtr = realloc( *ppucObjPtr, lSize );
    *pbOk = (tsp00_Bool) (ppucObjPtr != NULL);
}


void sapdbwa_free( sapdbwa_UInt1*  pucObjPtr )
{
    tsp00_Bool dump = (pucObjPtr == (sapdbwa_UInt1*) 17);
    
    wd11AllocCount--;
    free( pucObjPtr );
    if (dump) {
#ifdef sapdbwa_MEMCHECK
        _CrtDumpMemoryLeaks();
#endif        
    }; /* if */
}
