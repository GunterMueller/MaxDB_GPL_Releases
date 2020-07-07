/*!================================================================
  module:    gpr01b.h

  responsible:  BurkhardD

  special area:   Header for Oldstyle PASCAL and C Modules (vpr01b)

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

#ifndef __GPR01B_H__
#define __GPR01B_H__

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
#ifdef __cplusplus
extern "C" {                         /* Assume C declarations for C++ */
#endif  /* __cplusplus */
  void p01bdrop_parsid (sqlcatype * sqlca, sqlxatype *sqlxa, tpr00_ParseId VAR_ARRAY_REF parsid);
  void p01bgetmfetch  (sqlcatype * sqlca, sqlxatype *sqlxa, sqlgaentry *ga, sqlkaentry *ka, tsp00_Int4 sqcrowcount);
  void p01bexistdatapart (sqlcatype * sqlca, sqlxatype *sqlxa, sqlgaentry *ga, sqlkaentry *ka, sqlorentry *ore, int mfindex);
  void p01bafterexecute (sqlcatype * sqlca, sqlxatype *sqlxa, sqlgaentry *ga, sqlkaentry *ka, sqlorentry *ore, sqlparsid *parsid);
  void p01baexecute (sqlcatype * sqlca, sqlxatype *sqlxa, sqlgaentry *ga, sqlkaentry *ka, sqlorentry *ore);
void p01baparsen(sqlcatype * sqlca, sqlxatype *sqlxa, sqlgaentry *ga, sqlkaentry *kae, tsp00_Int2 *cmdfetch);
void p01bbparsen(sqlcatype * sqlca, sqlxatype *sqlxa, sqlgaentry *ga, sqlkaentry *kae, tsp00_Int2 *cmdfetch);
tsp00_Int2 p01bmfentryget(sqlcatype * sqlca, sqlxatype *sqlxa, tpr00_ParseId VAR_ARRAY_REF parsid);
#ifdef __cplusplus
}
#endif  /* __cplusplus */
#endif
