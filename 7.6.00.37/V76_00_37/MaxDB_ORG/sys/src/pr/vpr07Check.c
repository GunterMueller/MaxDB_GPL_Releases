/*!================================================================
 module:    vpr07Check.c

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

#include "gpr00.h"
#include "vpr07Check.h"

/*==================================================================*
 *  DECLARATIONS                                                    *
 *==================================================================*/

#define PR07_NEVER (-1)

/*==================================================================*
 *  EXTERNAL PROTOTYPES                                             *
 *==================================================================*/

/*==================================================================*
 *  EXTERNAL VARIABLES                                              *
 *==================================================================*/

/*==================================================================*
 *  FUNCTION PROTOTYPES                                             *
 *==================================================================*/

/*==================================================================*
 *  CODE                                                            *
 *==================================================================*/

/* must return int in case of definition of PR07_CHECKDESC */
int pr07CheckAssert(int Expression)
{
  assert(Expression);
  return(PR07_NEVER);
}

#ifdef SAPDB_QUICK
void pr07CheckHeap(tsp00_Uint1 *Buffer)
{
  BOOLEAN bOk;
  sqlvalidatealloc ( Buffer, &bOk );
  if (!bOk)
    pr07CheckAssert(FALSE);
}
#endif
