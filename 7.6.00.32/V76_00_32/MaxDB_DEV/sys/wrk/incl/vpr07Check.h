/*!================================================================
 module:    vpr07Check.h

 responsible: BurkhardD

 special area:   CPCDrv | Client | Rangcheck

 description: Check auf Konsistenz von Datenstrukturen und
              Gueltigkeit von Pointern

 see:




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




 ===================================================================*/

/*==================================================================*
 *  INCLUDES                                                        *
 *==================================================================*/
#include <assert.h>

/*==================================================================*
 *  DECLARATIONS                                                    *
 *==================================================================*/

#define PR07_CHECKDESC(Desc, DescTypeEnum) \
{int _d##Desc = (!Desc || Desc->DescType != DescTypeEnum) ? pr07CheckAssert(FALSE) : 0;}
#ifdef SAPDB_QUICK
#define PR07_HEAPCHECK(Buffer) pr07CheckHeap(Buffer)
#define PR07_ASSERT(a) if (a==NULL) pr07CheckAssert(TRUE)
#else
#define PR07_HEAPCHECK(Buffer)
#define PR07_ASSERT(a) a
#endif
/*==================================================================*
 *  EXTERNAL PROTOTYPES                                             *
 *==================================================================*/

/*==================================================================*
 *  EXTERNAL VARIABLES                                              *
 *==================================================================*/

/*==================================================================*
 *  FUNCTION PROTOTYPES                                             *
 *==================================================================*/

int pr07CheckAssert(int Expression);
#ifdef SAPDB_QUICK
void pr07CheckHeap(tsp00_Uint1 *Buffer);
#endif

/*==================================================================*
 *  CODE                                                            *
 *==================================================================*/

