/*!================================================================
 module:    gpr11.h

 responsible: BurkhardD
 
 special area:   Callinterface | Client | Prototypes fuer vpr11
 
 description: Call_statement_proceduren
 
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

#ifndef __GPR11_H__
#define __GPR11_H__

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
void p11shortfieldparameterput(sqlcatype *sqlca, sqlgaentry *gaen, sqldatype *sqlda, boolean again);
#ifdef __cplusplus
}
#endif

#endif
