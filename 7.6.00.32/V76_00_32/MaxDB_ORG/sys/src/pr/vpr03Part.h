/*!================================================================

  module:       vpr03Part.h

  responsible:  BurkhardD

  special area: Functions to modify a Part 


  description:  Header fuer Verwaltung des Auftragsinterfaces

  see also:     

 -------------------------------------------------------------------





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
#ifndef __VPR03_PART_H__
#define __VPR03_PART_H__

/*==================================================================*
 *  INCLUDES                                                        *
 *==================================================================*/

#include "gpr00.h"

/*==================================================================*
 *  DECLARATIONS                                                    *
 *==================================================================*/

/*==================================================================*
 *  PROTOTYPES                                                      *
 *==================================================================*/

externC tsp1_part *pr03PartFind(sqlratype *sqlra, tsp1_part_kind_Param part_kind);
externC tsp1_part *pr03PartFindDirect(sqlratype *sqlra, tsp1_part_kind_Param part_kind);
externC void pr03PartInitCache(sqlratype *sqlra);
externC tsp00_Int4 pr03PartPut(tsp1_part *partPtr, tsp00_Int4 Offset, void *pBuf, tsp00_Int4 cbLen);
externC tsp00_Int4 pr03PartCopy(tsp1_part *partPtr, void *pBuf, tsp00_Int4 cbLen);
externC tsp00_Int4 pr03PartAppend(tsp1_part *partPtr, void *pBuf, tsp00_Int4 cbLen);
externC tsp00_Int4 pr03PartGetPartLength(tsp1_part *partPtr);
externC void pr03PartSetPartLength(tsp1_part *partPtr, tsp00_Int4 cbLen);
externC tsp00_Int4 pr03PartGetFreePartSpace(tsp1_part *partPtr);
externC tpr_runtime_errors_Enum pr03PartConverttoPart(tsp1_part *partPtr, tsp00_Int4 *Offset,tsp00_Int4 *cbPartLen, tpr05_StringEncoding encodingPart, void *pBuf, tsp00_Int4 cbBufLen, tpr05_StringEncoding EncodingBuf);
externC char *pr03PartGetRawPtr(tsp1_part *partPtr);
#endif
