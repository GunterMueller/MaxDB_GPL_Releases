/*!================================================================
 module:    vpr08c.h

 responsible: BurkhardD
 
 special area:   CPCDrv | Client | Prototypes fuer vpr08c.c
 
 description: SQL Runtime System
 
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

#ifndef __VPR08C_H__
#define __VPR08C_H__

/*==================================================================*
 *  INCLUDES                                                        *
 *==================================================================*/

#include "gpr00.h"
#include "vpr01Con.h"
#include "vpr05AnalyzeSQL.h"

/*==================================================================*
 *  DECLARATIONS                                                    *
 *==================================================================*/

/*==================================================================*
 *  FUNCTION PROTOTYPES                                             *
 *==================================================================*/
#ifdef __cplusplus
extern "C" {
#endif
void pr08cSearchFetch(sqlcatype *sqlca, sqlxatype *sqlxa, sqlgaentry *ga, tsp00_KnlIdentifier VAR_ARRAY_REF ergn, BOOLEAN *descr, int *intopos);
void pr08cSearchCommand (sqlcatype*, sqlxatype*, sqlgaentry*, BOOLEAN*, 
			 int*, char*);
void pr08cSearchClose(struct tpr01_SQLDesc *SQLDesc);
int  pr08cputStatementtoPart(struct tpr01_SQLDesc *SQLDesc);
tpr_runtime_errors_epr00 pr08cPutCursorNametoPart(tpr05_String *CursorName, tsp1_part *part_Ptr, tsp00_Int4 *offset, tsp00_Int4 *free_part_space, tpr05_StringEncoding PacketEncoding);
  tpr00_FetchSpecEnum pr08MapFetchSpec(tpr05_StmtAnalysisDesc *StmtTypeDesc);
#ifdef __cplusplus
}
#endif
#endif
