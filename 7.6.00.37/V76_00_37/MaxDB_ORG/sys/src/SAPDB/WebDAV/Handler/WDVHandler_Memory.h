/*!**********************************************************************

  module: WDVHandler_Memory.h

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

#ifndef WDVHandler_MEMORY_H
#define WDVHandler_MEMORY_H

/***********************************************************************

	Includes

 ***********************************************************************/

/*	#include "SAPDB/WebDAV/Handler/WDVHandler_CommonInclude.h"
*/
/***********************************************************************

	Defines

 ***********************************************************************/

/* To enable memory leak searching */
/* #define WDVHANDLER_MEMCHECK */

#ifdef WDVHANDLER_MEMCHECK

#define WDVHandler_Allocat         Memory_Allocat
#define WDVHandler_ReAllocat       Memory_ReAllocat
#define WDVHandler_Free            Memory_Free

#else

#define WDVHandler_Allocat         sqlallocat
#define WDVHandler_ReAllocat       sqlreallocat
#define WDVHandler_Free            sqlfree

#endif

/***********************************************************************

	Types

 ***********************************************************************/

/***********************************************************************

	Prototypes

 ***********************************************************************/

#ifdef WDVHANDLER_MEMCHECK

void Memory_Allocat ( WDV_Int4    lSize,
                      WDV_UInt1 **ppucObjPtr,
                      WDV_Bool   *pbOk		);
    
void Memory_ReAllocat ( WDV_Int4    lSize,
                        WDV_UInt1 **ppucObjPtr,
                        WDV_Bool   *pbOk );

void Memory_Free ( WDV_UInt1 *pucObjPtr );

#endif

/***********************************************************************

	End

 ***********************************************************************/

#endif
