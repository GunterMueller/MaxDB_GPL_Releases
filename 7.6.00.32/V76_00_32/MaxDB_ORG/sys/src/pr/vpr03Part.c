/*!================================================================

  module:       vpr03Part.c

 -------------------------------------------------------------------

  responsible:  BurkhardD

  special area: Functions to modify a Part

  description:  

  see also:
  
  last changed: 2000-08-29  11:01 Marco Paskamp     get/setPartLength()

  -------------------------------------------------------------------------





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

/*==================================================================*
 *  INCLUDES                                                        *
 *==================================================================*/

#include "vpr03Part.h"
#include "vpr03Part.h"
#include "hsp26.h"
#include "hsp78_0.h"
#include "vpr07Macro.h"

/*==================================================================*
 *  DEFINES                                                    *
 *==================================================================*/


/*==================================================================*
 *  EXTERNAL PROTOTYPES                                             *
 *==================================================================*/

/*==================================================================*
 *  FUNCTION PROTOTYPES                                             *
 *==================================================================*/

/*==================================================================*
 *  CODE                                                            *
 *==================================================================*/

/*!
  Function: pr03PartFind

  description: Liefert die Adresse des in part_kind angegebenen Parts.
               Optimierte Version, die mit einem Part Cache arbeitet.
	       Dieser PartCache muss nach abholen des Auftrages mit 
	       pr03PartInitCache initialisiert worden sein.
               
	       
  arguments:
      sqlra        [in]  Request Area.
      part_kind    [in]  Im Auftrag (PartCache) zu suchender Part.

  return:          Pointer auf den Part oder NULL fuer nicht gefunden.
 */

tsp1_part *pr03PartFind(sqlratype *sqlra, tsp1_part_kind_Param part_kind)
{
  tsp1_segment   *segp= (sqlra) ? (tsp1_segment*)sqlra->rasegptr : NULL;

  tsp1_part *part_ptr = NULL;

  if (!segp)
    return NULL;

  if (segp->variant.C_1.sp1s_segm_header_F.variant.C_1.sp1s_no_of_parts_F > 0) {
    if (part_kind < PartCache_mxpr00) { 
      part_ptr = sqlra->rasegmpartptr[part_kind];
    }
    else {
      /* part_kind is to big to fit in the cache */
      /* try to find directly */
      s26find_part(segp, part_kind, &part_ptr);
    }
  }
  return(part_ptr);
}

/*!
  Function: pr03PartFindDirect 

  description: Liefert die Adresse des in part_kind angegebenen Parts.
               Der Part wird im Gegensatz zu pr03PartFind direkt
	       im Auftrag gesucht.

  arguments:
      sqlra        [in]  Request Area.
      part_kind    [in]  Im Auftrag zu suchender Part.

  return:          Pointer auf den Part oder NULL fuer nicht gefunden.
 */

tsp1_part *pr03PartFindDirect(sqlratype *sqlra, tsp1_part_kind_Param part_kind)
{
  tsp1_segment   *segp= (sqlra) ? (tsp1_segment*)sqlra->rasegptr : NULL;
  tsp1_part* part_ptr = NULL;

  if (!segp)
    return NULL;

  if (segp->variant.C_1.sp1s_segm_header_F.variant.C_1.sp1s_no_of_parts_F > 0) {
    s26find_part(segp, part_kind, &part_ptr);
  }
  return(part_ptr);
}

/*!
  Function: pr03PartInitCache 

  description: Initialisiert den PartCache.
               Der PartCache ist ein Array von Pointern, mit Anfangsaddressen
	       der Parts im Auftrag.
	       sp1p_part_kind wird als Index in den PartCache benutzt.

  arguments:
      sqlra        [in]  Request Area. Nach dem Receive vom Kern.

  return:          ---
 */

void pr03PartInitCache(sqlratype *sqlra)
{
  tsp1_segment   *segp= (sqlra) ? (tsp1_segment*)sqlra->rasegptr : NULL;
  tsp1_part* part_ptr;
  tsp00_Int2 part_no;

  memset(sqlra->rasegmpartptr, 0, sizeof(sqlra->rasegmpartptr));  

  if (!segp)
    return;

  part_ptr = &segp->variant.C_1.sp1p_part_F;

  part_no=segp->variant.C_1.sp1s_segm_header_F.variant.C_1.sp1s_no_of_parts_F;
  if (part_no == 0)
    return;

  while(part_ptr) {
    tsp00_Int2 part_kind = part_ptr->variant.C_2.sp1p_part_kind_F;
    if (part_kind < PartCache_mxpr00 ) {
      sqlra->rasegmpartptr[part_kind] = part_ptr;
    }    
    if (part_no > 1) {
      /* s26nextpart crashes if there is no nextpart available */
      s26nextpart(&part_ptr);
      part_no--;
    }
    else
      break;    
  }
}

/*!
  Function: pr03PartPut 

  description: Puts buffer pBuf into Part on position Offset and updates the length of the Part.
               
  arguments:
      partPtr      [in|out]  pointer to a part
      pBuf         [in]      pointer to buffer which contains data
      Offset       [in]      offset up to starting put data into part
      cbLen        [in]      max length of part
      
  return:          the number of bytes used in the part.---
*/

tsp00_Int4  pr03PartPut(tsp1_part *partPtr, tsp00_Int4 Offset,void *pBuf, tsp00_Int4 cbLen)
{
  memcpy(&partPtr->variant.C_1.sp1p_buf_F[Offset], pBuf, cbLen);
  partPtr->sp1p_part_header.sp1p_buf_len = Offset + cbLen;
  return partPtr->sp1p_part_header.sp1p_buf_len;
}

/*!
  Function: pr03PartCopy

  description: Puts buffer pBuf into Part and updates the length of the Part.
               
  arguments:
      partPtr      [in|out]  pointer to a part
      pBuf         [in]      pointer to buffer which contains data
      cbLen        [in]      max length of part
      
  return:          the number of bytes used in the part.---
*/

tsp00_Int4 pr03PartCopy(tsp1_part *partPtr, void *pBuf, tsp00_Int4 cbLen)
{
  return pr03PartPut(partPtr, 0, pBuf, cbLen);
}

/*!
  Function: pr03PartAppend

  description: Appends the Part with buffer pBuf and updates the length 
  of the Part.
               
  arguments:
      partPtr      [in|out]  pointer to a part
      pBuf         [in]      pointer to buffer which contains data
      cbLen        [in]      max length of part
      
  return:          the number of bytes used in the part.---
*/

tsp00_Int4  pr03PartAppend(tsp1_part *partPtr, void *pBuf, tsp00_Int4 cbLen)
{
  return pr03PartPut(partPtr, partPtr->sp1p_part_header.sp1p_buf_len, pBuf, cbLen);
}

/*!
  Function: pr03PartConverttoPart 

  description: Converts from buffer to part depending on encoding type.
               
  arguments:
      partPtr      [in|out]  pointer to a part
      Offset       [in]      offset up to starting put data into part
      cbLen        [in]      max length of part
      encodingPart [in]      encoding type of part
      pBuf         [in]      pointer to buffer which contains data
      cbBufLen     [in]      length of pBuf in bytes
      encodingBuf  [in]      encoding type of buffer
      
  return:    0 : processing successed
          -777 : error can not convert from unicode
          -808 : Not yet implemented
          -706 : SQL statement too long (buffer overflow)
 */

tpr_runtime_errors_Enum pr03PartConverttoPart(tsp1_part *partPtr, tsp00_Int4 *Offset,tsp00_Int4 *cbPartLen, tpr05_StringEncoding encodingPart, void *pBuf, tsp00_Int4 cbBufLen, tpr05_StringEncoding encodingBuf)
{
  tsp00_Uint4 cbWrite = 0;
  tsp00_Uint4 cbParsed;
  tsp78ConversionResult rc;
  tpr_runtime_errors_Enum Err = cpr_p_ok;

  rc = sp78convertString(encodingPart, 
                         (void*)&partPtr->variant.C_1.sp1p_buf_F[*Offset], 
                         (tsp00_Uint4)*cbPartLen, 
                         &cbWrite, 
                         FALSE, 
                         encodingBuf, 
                         (const void*) pBuf, 
                         (tsp00_Uint4) cbBufLen, 
                         &cbParsed);

  if (rc == sp78_TargetExhausted) {
    Err = cpr_inp_string_truncated;
  }
  else
    if (rc != sp78_Ok)
      Err = cpr_conv_to_unicode_impossible;
  partPtr->sp1p_part_header.sp1p_buf_len = *Offset + cbWrite; /*set part size*/
  *cbPartLen -= cbWrite;
  *Offset    += cbWrite;
  return Err;
}


/*!
  Function: pr03getPartLength 

  description: Gets current length of part.
               
  arguments:
      partPtr [in|out]  pointer to a part
      
  return:  length of part in bytes
 */

tsp00_Int4 pr03PartGetPartLength(tsp1_part *partPtr)
{
  return (partPtr->sp1p_part_header.sp1p_buf_len);
}

/*!
  Function: pr03SetPartLength 

  description: Sets current length of part.
               
  arguments:
      partPtr [in|out]  pointer to a part
      cbLen   [in]      current length of part
      
  return:          ---
 */

void pr03PartSetPartLength(tsp1_part *partPtr, tsp00_Int4 cbLen)
{
  partPtr->sp1p_part_header.sp1p_buf_len = cbLen;
}

/*!
  Function: pr03getFreePartSpace 

  description: Gets the free space in bytes of a order packet.
               
  arguments:
      partPtr [in|out]  pointer to a part
      
  return:  free space in bytes
 */

tsp00_Int4 pr03PartGetFreePartSpace(tsp1_part *partPtr)
{
  return (partPtr->sp1p_part_header.sp1p_buf_size );
}

char *pr03PartGetRawPtr(tsp1_part *partPtr)
{
  return (char*)((partPtr) ? partPtr->variant.C_1.sp1p_buf_F : NULL);
}
