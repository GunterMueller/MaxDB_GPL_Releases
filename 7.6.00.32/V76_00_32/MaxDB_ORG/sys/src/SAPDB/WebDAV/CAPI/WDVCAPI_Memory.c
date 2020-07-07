/*!**********************************************************************

  module: WDVCAPI_Memory.c

  -----------------------------------------------------------------------

  responsible:  Markus Oezgen

  special area: WebDAV

  description:  Memory Management

  see also:

  -----------------------------------------------------------------------

                         Copyright (c) 2000-2005 SAP AG




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

/***********************************************************************

	Includes

 ***********************************************************************/

#include <stdlib.h>

#ifdef WIN32
	#define _DEBUG
	#include <crtdbg.h>
#endif

#include <WDVCAPI_Definition.h>

#include "WebDAV/CAPI/WDVCAPI_Memory.h"

/***********************************************************************

	Defines

 ***********************************************************************/

/***********************************************************************

	Structs & Types

 ***********************************************************************/

/***********************************************************************

	Prototypes

 ***********************************************************************/

/***********************************************************************

	API Internal Functions

 ***********************************************************************/

#ifdef WDVCAPI_MEMCHECK

static allocCount = 0;

void Memory_Allocat( WDV_Int4    lSize,
					 WDV_UInt1 **ppucObjPtr,
					 WDV_Bool   *pbOk )
{

    allocCount++;

    *ppucObjPtr = malloc( lSize );
    *pbOk = (WDV_Bool) (ppucObjPtr != NULL);

}


void Memory_ReAllocat( WDV_Int4    lSize,
                       WDV_UInt1 **ppucObjPtr,
                       WDV_Bool   *pbOk )
{

    *ppucObjPtr = realloc( *ppucObjPtr, lSize );
    *pbOk = (WDV_Bool) (ppucObjPtr != NULL);

}


void Memory_Free( WDV_UInt1 *pucObjPtr )
{

    WDV_Bool dump = (pucObjPtr == (WDV_UInt1*) 17);
    
    allocCount--;

    free( pucObjPtr );

    if (dump) {
#ifdef WIN32
/*
        _CrtDumpMemoryLeaks();
*/
#endif        
    };

}

#endif  /* WDVCAPI_MEMCHECK */

/***********************************************************************

	End

 ***********************************************************************/
