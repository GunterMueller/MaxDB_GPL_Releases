/*!================================================================

  module:       vpr06ParseId.h

  responsible:  BurkhardD

  special area:

  description:

  see also:

  change history :

        D025086 03-12-2001 New function pr06ParseIdDropDirect.

 -------------------------------------------------------------------





    ========== licence begin  GPL
    Copyright (c) 1998-2005 SAP AG

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
#ifndef __VPR06_PARSEID_H__
#define __VPR06_PARSEID_H__

/*==================================================================*
 *  INCLUDES                                                        *
 *==================================================================*/

#include "gpr00.h"
#include "vpr09DynaDesc.h"

/*==================================================================*
 *  DECLARATIONS                                                    *
 *==================================================================*/
typedef struct {
  tpr09DynamicDescriptor *ParseIDDesc;
  struct tpr01_ConDesc *ConDesc;
  tsp00_Int2 MaxParseIDs;
} tpr06_ParseIDCache;
/*==================================================================*
 *  PROTOTYPES                                                      *
 *==================================================================*/


externC BOOLEAN pr06ParseIdIsEqual(tpr00_ParseId *p1, tpr00_ParseId *p2);

externC void pr06ParseIdCopy(tpr00_ParseId *p1, tpr00_ParseId *p2);
externC void pr06ParseInfoCopy(tpr00_ParseInfo *p1, tpr00_ParseInfo *p2);
externC tpr06_ParseIDCache *pr06ParseIDNewCache(struct tpr01_ConDesc *ConDesc, tsp00_Int4 Size);
externC void pr06ParseIDDeleteCache(tpr06_ParseIDCache *Cache);
externC void pr06ParseIdDrop(sqlcatype *sqlca, sqlxatype *sqlxa, sqlgaentry *ga, tpr06_ParseIDCache *ParseIDCache, tpr00_ParseId *ParseID);
externC void pr06ParseIdDropIntoCache(sqlcatype *sqlca, sqlxatype *sqlxa, sqlgaentry *ga, tpr06_ParseIDCache *ParseIDCache, tpr00_ParseId *ParseID);
externC void pr06ParseIdDropDirect(struct tpr01_ConDesc *ConDesc, tpr00_ParseId *ParseID);
externC BOOLEAN pr06ParseIdCheckKnlSessionID(tpr00_SessionID *SessionID, tpr00_ParseId *aparseid);
externC tsp00_Int4 pr06ParseIdKnlSessionIDCompare(tpr00_SessionID *SessionID_1, tpr00_SessionID *SessionID_2);
externC void pr06ParseIdKnlSessionIDCopy(tpr00_SessionID *Dest, tpr00_SessionID *Src);
externC void pr06ParseIdSessionID2ParseID(tpr00_ParseId *ParseID, tpr00_SessionID *SessionID);
#endif
