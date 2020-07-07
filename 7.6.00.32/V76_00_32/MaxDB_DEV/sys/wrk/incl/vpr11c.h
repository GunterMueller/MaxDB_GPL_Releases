/*!================================================================
 module:    vpr11c.h

 responsible: BurkhardD

 special area:   CPCDrv | Client | ODBC Interface

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

#ifndef __VPR11C_H__
#define __VPR11C_H__
#include "gpr00.h"
#include "vpr01Con.h"

void pr11cGetsqcstm(sqlcatype *sqlca, struct tpr01_ConDesc *ConDesc,                    tpr05_StringEncoding PacketEncoding,
tsp1_cmd_mess_type cmdtyp, tsp00_Int4 cbBufLen, tsp00_MoveObj sqcstmbuf, tpr05_StringEncoding encodingType, int *intopos);
void pr11cSyntax  (sqlcatype *sqlca,
                   struct tpr01_ConDesc *ConDesc,
                   /*                   sqldatype *sqlda,*/
                   tpr05_String *sqlCmd);
#endif
