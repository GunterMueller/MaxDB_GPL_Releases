/*!================================================================

  module:       vpr03Segement.h

  responsible:  BurkhardD

  special area: Verwaltung Auftrags Segment

  description:

  see also:

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
#ifndef __VPR03_SEGMENT_H__
#define __VPR03_SEGMENT_H__

/*==================================================================*
 *  INCLUDES                                                        *
 *==================================================================*/

#include "vsp001.h"
#include "gpr00.h"

/*==================================================================*
 *  DECLARATIONS                                                    *
 *==================================================================*/
#if !defined(externC)
# if defined (__cplusplus)
#  define  externC      extern "C"
# else
#  define  externC
# endif
#endif

typedef struct {
  tsp1_packet *packetPtr;
  tsp00_C8 TermId;
  tsp1_segment *segmPtr;
  tsp1_part *firstPartPtr;
  tsp1_part *actPartPtr;
  tpr00_PartCache *rasegmpartptr;
} tpr03_SegmDesc;

typedef enum tpr03_SegmentAttributeEnum {
  Commit_Immediately_epr03, Ignore_Costwarning_epr03, Prepare_epr03, With_Info_ep03, Mass_Cmd_epr03, Parsing_Again_epr03
} tpr03_SegmentAttributeEnum;

/*==================================================================*
 *  PROTOTYPES                                                      *
 *==================================================================*/

externC void pr03SegmentSetAttribute(tsp1_segment *segm, tpr03_SegmentAttributeEnum Attribute, BOOLEAN Val);
externC BOOLEAN pr03SegmentGetAttribute(tsp1_segment *segm, tpr03_SegmentAttributeEnum Attribute);
externC void pr03SegmentSetMessCode(tsp1_segment *segm, tsp1_cmd_mess_type MessType);
externC BOOLEAN pr03SegmentSetSqlMode(tsp1_segment *segm, tsp1_sqlmode SqlMode);
externC tpr03_SegmDesc *pr03SegmentNew(tsp1_packet *pck, tpr00_PartCache *rasegmpartptr, tsp00_C8 VAR_ARRAY_REF  pTermId);
externC void pr03SegmentDelete(tpr03_SegmDesc *SegmDesc);

externC tsp1_segment *pr03SegmentInit(tpr03_SegmDesc *SegmDesc);
externC tsp1_segment *pr03SegmentAdd(tpr03_SegmDesc *SegmDesc);
externC tsp1_part * pr03SegmentAddPart(tpr03_SegmDesc *SegmDesc, tsp1_part_kind_Param part_kind);
externC void pr03SegmentFinishPart(tpr03_SegmDesc *SegmDesc);
#endif
