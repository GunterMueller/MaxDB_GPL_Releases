/*!================================================================
 module:    gpr01e.h

 responsible: BurkhardD
 
 special area:   CPCDrv | Client | Prototypes fuer vpr01e
 
 description: 
 
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

#ifndef __GPR01E_H__
#define __GPR01E_H__

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
extern "C" {
#endif
void p01xtracefilecheck(sqlcatype *sqlca, sqlxatype *sqlxa);
void p01xpagethostvariable(sqlcatype *sqlca, sqlxatype *sqlxa, sqlgaentry *gae, tsp00_Int2 paind, tsp00_Int4 *len);
void p01xprofinit (sqlcatype *sqlca, sqlxatype *sqlxa, sqlgaentry *gae);
void p01xtimetrace (sqlcatype *sqlca, sqlxatype *sqlxa, sqlgaentry *gae);
void p01xcmdclose (sqlcatype *sqlca, sqlxatype *sqlxa, sqlgaentry *gae);
void p01xhalt (tpr00_ResultTypeEnum result);
void p01xballocate  (sqlcatype *sqlca, sqlgaentry *gae, int index);
#ifdef __cplusplus
}
#endif
#endif
