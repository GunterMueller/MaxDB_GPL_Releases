/*!================================================================
  module:    gpr05.h

  responsible:  BurkhardD

  special area:   Header for Oldstyle PASCAL and C Modules (vpr05, vpr05c)

  description:

  see:

 -------------------------------------------------------------------





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




 ===================================================================*/

#ifndef __GPR05_H__
#define __GPR05_H__

/*==================================================================*
 *  INCLUDES                                                        *
 *==================================================================*/

#include "gpr00.h"

/*==================================================================*
 *  DECLARATIONS                                                    *
 *==================================================================*/

/*==================================================================*
 *  FUNCTION PROTOTYPES                                             *
 *==================================================================*/
BOOLEAN p05eq (tsp00_Sname VAR_ARRAY_REF a, char *b, tsp00_Int4 b_pos, tsp00_Int4 length);
void p05nextsymbol(char *buf, tsp00_Int4 buflen, tsp00_Int4 posanf,
		   tsp00_Int4 *sympos, tsp00_Int4 *symlen, tpr00_Symbol *symb);
int p05up1casebuf(tsp00_MoveObj VAR_ARRAY_REF buf, tsp00_Int4 lwb, tsp00_Int4 upb);
tsp00_Int4 p05chrtoint2 (tsp00_MoveObj VAR_ARRAY_REF buf, int pos, int len);
#endif
