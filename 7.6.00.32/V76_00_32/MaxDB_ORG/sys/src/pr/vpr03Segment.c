
/*!================================================================

  module:       vpr03Segment.c

 -------------------------------------------------------------------

  responsible:  BurkhardD

  special area: Verwaltung Auftrags Segment

  description:  

  see also:

  -------------------------------------------------------------------------





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


/*==================================================================*
 *  INCLUDES                                                        *
 *==================================================================*/

#include "vpr03Segment.h"
#include "gpr03m.h"
#include "hsp26.h"
/*==================================================================*
 *  DECLARATIONS                                                    *
 *==================================================================*/

/*==================================================================*
 *  EXTERNAL PROTOTYPES                                             *
 *==================================================================*/

/*==================================================================*
 *  FUNCTION PROTOTYPES                                             *
 *==================================================================*/

static void pr03SegmentResetPartCache(tpr00_PartCache *rasegmpartptr);
static void pr03SegmentSetCache(tpr00_PartCache *rasegmpartptr, tsp1_part *partPtr, tsp1_part_kind_Param part_kind);
/*==================================================================*
 *  CODE                                                            *
 *==================================================================*/

void pr03SegmentSetAttribute(tsp1_segment *segm, tpr03_SegmentAttributeEnum Attribute, BOOLEAN Val)
{
  tsp1_segment_header *segh = &segm->variant.C_1.sp1s_segm_header_F;
  switch(Attribute) {
  case (Commit_Immediately_epr03) : {
    segh->variant.C_1.sp1c_commit_immediately_F = Val;
    break;
  }
  case (Ignore_Costwarning_epr03) : {
    segh->variant.C_1.sp1c_ignore_costwarning_F = Val;
    break;
  }
  case (Prepare_epr03) : {
    segh->variant.C_1.sp1c_prepare_F = Val;
    break;
  }
  case (With_Info_ep03) : {
    segh->variant.C_1.sp1c_with_info_F = Val;
    break;
  }
  case (Mass_Cmd_epr03) : {
    segh->variant.C_1.sp1c_mass_cmd_F = Val;
    break;
  }
  case (Parsing_Again_epr03) : {
    segh->variant.C_1.sp1c_parsing_again_F = Val;
    break;
  }
  }
}

void pr03SegmentSetMessCode(tsp1_segment *segm, tsp1_cmd_mess_type MessType)
{
  tsp1_segment_header *segh = &segm->variant.C_1.sp1s_segm_header_F;
  segh->variant.C_1.sp1c_mess_type_F = MessType;
}

/*!
  Function:     pr03SegmentSetSqlMode

  see also: 
  
  Description:  Puts the given SqlMode into packet header
    
  Arguments:
    pck           [in] pointer to a packet
    SqlMode       [in] sqlmode of this packet
    
  return value: BOOLEAN
    <UL>
        <LI>TRUE: processing succeded
        <LI>FALSE: Error
    </UL>
 */
externC BOOLEAN 
pr03SegmentSetSqlMode( tsp1_segment *segm, tsp1_sqlmode SqlMode )
{
  if (segm) {
    tsp1_segment_header *segh = &segm->variant.C_1.sp1s_segm_header_F;
    segh->sp1c_sqlmode = SqlMode;
    return (TRUE);
  }
  return(FALSE);
}


BOOLEAN pr03SegmentGetAttribute(tsp1_segment *segm, tpr03_SegmentAttributeEnum Attribute)
{
  tsp1_segment_header *segh = &segm->variant.C_1.sp1s_segm_header_F;
  switch(Attribute) {
  case (Commit_Immediately_epr03) : {
    return segh->variant.C_1.sp1c_commit_immediately_F;
    break;
  }
  case (Ignore_Costwarning_epr03) : {
    return segh->variant.C_1.sp1c_ignore_costwarning_F;
    break;
  }
  case (Prepare_epr03) : {
    return segh->variant.C_1.sp1c_prepare_F;
    break;
  }
  case (With_Info_ep03) : {
    return segh->variant.C_1.sp1c_with_info_F;
    break;
  }
  case (Mass_Cmd_epr03) : {
    return segh->variant.C_1.sp1c_mass_cmd_F;
    break;
  }
  case (Parsing_Again_epr03) : {
    return segh->variant.C_1.sp1c_parsing_again_F;
    break;
  }
  }
  return false;
}

externC tpr03_SegmDesc *pr03SegmentNew(tsp1_packet *pck, tpr00_PartCache *rasegmpartptr, tsp00_C8 VAR_ARRAY_REF  pTermId)
{
  tpr03_SegmDesc *SegmDesc = pr03mAllocat(sizeof(tpr03_SegmDesc), "SegmDesc");
  if (SegmDesc) {
    SegmDesc->packetPtr = pck;
    SegmDesc->rasegmpartptr = rasegmpartptr;
    memcpy(SegmDesc->TermId, pTermId, sizeof(SegmDesc->TermId));
    SegmDesc->segmPtr = NULL;
    SegmDesc->firstPartPtr = NULL;
    SegmDesc->actPartPtr = NULL;
  }
  return SegmDesc;
}

static void pr03SegmentResetPartCache(tpr00_PartCache *rasegmpartptr)
{
  memset(rasegmpartptr, 0, sizeof(tpr00_PartCache));  
}

static void pr03SegmentSetCache(tpr00_PartCache *rasegmpartptr, tsp1_part *partPtr, tsp1_part_kind_Param part_kind)
{
  (*rasegmpartptr)[part_kind] = partPtr;
}

externC void pr03SegmentDelete(tpr03_SegmDesc *SegmDesc)
{
  pr03mFree(SegmDesc, "SegmDesc");
  return;
}

externC tsp1_segment *pr03SegmentInit(tpr03_SegmDesc *SegmDesc)
{
  if (SegmDesc && SegmDesc->packetPtr) {
    tsp1_packet * pck = SegmDesc->packetPtr;
    s26init_cmd_packet(pck, &SegmDesc->segmPtr);
    memcpy(pck->sp1_header.sp1h_appl_version, SegmDesc->TermId, sizeof(tsp00_C8));
    SegmDesc->actPartPtr = NULL;
    SegmDesc->firstPartPtr =  NULL;
    pr03SegmentResetPartCache(SegmDesc->rasegmpartptr);
    return SegmDesc->segmPtr;
  }
  return NULL;
}

externC tsp1_segment * pr03SegmentAdd(tpr03_SegmDesc *SegmDesc)
{
  tsp1_packet * pck = SegmDesc->packetPtr;
  if (SegmDesc->segmPtr == NULL) {
    pr03SegmentInit(SegmDesc);
  }
  else {
    s26new_segment_init (pck, sp1sk_cmd, &SegmDesc->segmPtr, &SegmDesc->firstPartPtr);
    SegmDesc->actPartPtr = NULL;
  }
  return SegmDesc->segmPtr;
}

externC tsp1_part * pr03SegmentAddPart(tpr03_SegmDesc *SegmDesc, tsp1_part_kind_Param part_kind)
{
  if (SegmDesc && SegmDesc->packetPtr) {
    s26new_part_init (SegmDesc->packetPtr, SegmDesc->segmPtr, &SegmDesc->actPartPtr);
    if (!SegmDesc->firstPartPtr)
      SegmDesc->firstPartPtr = SegmDesc->actPartPtr;
    if (SegmDesc->actPartPtr)
      SegmDesc->actPartPtr->variant.C_1.sp1p_part_header_F.sp1p_part_kind = part_kind;
    if (SegmDesc->segmPtr->variant.C_1.sp1s_segm_header_F.variant.C_1.sp1s_own_index_F == 1) {
      pr03SegmentSetCache(SegmDesc->rasegmpartptr, SegmDesc->actPartPtr, part_kind);
    }
    return SegmDesc->actPartPtr;
  }
  return NULL;
}

externC void pr03SegmentFinishPart(tpr03_SegmDesc *SegmDesc)
{
  if (SegmDesc && SegmDesc->actPartPtr)
    s26finish_part(SegmDesc->packetPtr, SegmDesc->actPartPtr);  
}
