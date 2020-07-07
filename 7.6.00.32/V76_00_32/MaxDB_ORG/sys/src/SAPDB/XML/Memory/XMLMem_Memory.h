/*!**********************************************************************

  module: XMLMem_Memory.h

  -----------------------------------------------------------------------

  responsible:  Markus Oezgen

  special area: XML Index Engine

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

#ifndef XMLMEM_MEMORY_H
#define XMLMEM_MEMORY_H

/***********************************************************************

	Includes

 ***********************************************************************/

/***********************************************************************

	Defines

 ***********************************************************************/

/* To enable memory leak searching */
/*#define XMLMEM_MEMCHECK
*/
#ifdef XMLMEM_MEMCHECK

#define XMLMem_Allocat         Memory_Allocat
#define XMLMem_ReAllocat       Memory_ReAllocat
#define XMLMem_Free            Memory_Free

#else

#define XMLMem_Allocat         sqlallocat
#define XMLMem_ReAllocat       sqlreallocat
#define XMLMem_Free            sqlfree

#endif

/***********************************************************************

	Types

 ***********************************************************************/

/***********************************************************************

	Prototypes

 ***********************************************************************/

#ifdef XMLMEM_MEMCHECK

void Memory_Allocat ( SAPDB_Int4    lSize,
                      SAPDB_UInt1 **ppucObjPtr,
                      SAPDB_Bool   *pbOk		);
    
void Memory_ReAllocat ( SAPDB_Int4    lSize,
                        SAPDB_UInt1 **ppucObjPtr,
                        SAPDB_Bool   *pbOk );

void Memory_Free ( SAPDB_UInt1 *pucObjPtr );

#endif

/***********************************************************************

	End

 ***********************************************************************/

#endif
