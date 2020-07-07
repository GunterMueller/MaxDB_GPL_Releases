/*!================================================================
  module:    gpr01s.h

  responsible:  BurkhardD

  special area:   Header for Oldstyle PASCAL and C Modules (vpr01s, vpr03sc)

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

#ifndef __GPR01S_H__
#define __GPR01S_H__

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

tsp00_Int2 pr01sIsArrayCommand(sqlxatype* sqlxa, sqlkaentry *ka);
void pr01sPrepareExecuteAtFetch(struct tpr01_SQLDesc *SQLDesc);
void pr01sCuNameSearch(sqlxatype* sqlxa, sqlkaentry *ka, tsp00_KnlIdentifier cuname);
tsp00_Int2 pr01sFindKaOpenIndex(sqlxatype* sqlxa, tsp00_KnlIdentifier cuname);
tsp00_Int2 pr01sGetKaOpenIndex(sqlcatype* sqlca, sqlxatype* sqlxa);
void pr01sExecuteAtFetch(struct tpr01_SQLDesc *SQLDesc);
tsp00_Int2 pr01sGetFunctionCode(sqlratype *sqlra);
tsp00_Bool pr01sExecuteAtFetchPossible(sqlxatype* sqlxa, sqlkaentry *ka);
sqlkaentry *pr01sIsExecuteAtFetch(sqlxatype* sqlxa);
void pr01sDisableExecuteAtFetch(sqlratype *sqlrap);
void p01sbparsen (sqlcatype* sqlca, sqlxatype* sqlxa, sqlgaentry *ga, sqlkaentry *ka);
void p01saparsen (sqlcatype* sqlca, sqlxatype* sqlxa, sqlgaentry *ga, sqlkaentry *ka);
#endif
