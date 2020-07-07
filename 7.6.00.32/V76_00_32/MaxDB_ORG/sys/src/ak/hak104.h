/*!******************************************************************************

  @file         hak104.h

  ------------------------------------------------------------------------------

  @author       <unknown>

  @brief        <unknown topic>

\if EMIT_LICENCE

    ========== licence begin  GPL
    Copyright (c) 2004-2005 SAP AG
 
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
\endif

********************************************************************************/

#ifndef HAK104_H
#define HAK104_H


/*===========================================================================*
 *     RefType: C++                                                          *
 *===========================================================================*/

/*===========================================================================*
 *     MD5: '\x76\x39\x92\xf6\x4d\x8d\x41\x71\xc1\x47\xa4\x31\x96\xe8\xd5\x31'
 *===========================================================================*/


/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#include "gsp00.h"
#include "gak104.h"


/*===========================================================================*
 *  FUNCTION PROTOTYPES                                                      *
 *===========================================================================*/

externPascal pasbool ak104_CreateIncrementalMemorySequence (
    tak104_Allocator         allocator,
    tak104_refcountMode_Param     refMode,
    tak104_IncrementalMemorySequence  &  sequence,
    tak104_MsgList           msglist);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal pasbool ak104_DeleteIncrementalMemorySequence (
    tak104_Allocator         allocator,
    tak104_IncrementalMemorySequence  &  sequence,
    tak104_MsgList           msglist);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal pasbool ak104_DeleteIFieldSequence (
    tak104_Allocator         allocator,
    tak104_IFieldSequence  &  sequence,
    tak104_MsgList           msglist);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal pasbool ak104_CreateNullField_IMS (
    tak104_IncrementalMemorySequence     sequence,
    tak104_MsgList           msglist);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal pasbool ak104_CreateFixedField_IMS (
    tak104_IncrementalMemorySequence     sequence,
    tsp00_Int4               size,
    tsp00_MoveObjPtr      &  buf,
    tak104_MsgList           msglist);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal pasbool ak104_CreateVariableField_IMS (
    tak104_IncrementalMemorySequence     sequence,
    tsp00_Int4               size,
    tsp00_MoveObjPtr      &  buf,
    tak104_MsgList           msglist);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal pasbool ak104_CloseCurrentField_IMS (
    tak104_IncrementalMemorySequence     sequence,
    tsp00_Int4               size,
    tak104_MsgList           msglist);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void ak104_TruncateField_IMS (
    tak104_IncrementalMemorySequence     sequence,
    tsp00_Int4               index);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal tsp00_Int4 ak104_CalculateSlack_IMS (
    tak104_IncrementalMemorySequence     sequence);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal tsp00_Int4 ak104_GetFieldCount_IMS (
    tak104_IncrementalMemorySequence     sequence);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal pasbool ak104_GetField_IMS (
    tak104_IncrementalMemorySequence     sequence,
    tsp00_Int4               index,
    tsp00_MoveObjPtr      &  fieldbuf,
    tsp00_Int4            &  fieldlen,
    pasbool               &  isnull,
    tak104_MsgList           msglist);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal tsp00_Int4 ak104_GetFieldCount_IFS (
    tak104_IFieldSequence     sequence);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal pasbool ak104_GetField_IFS (
    tak104_IFieldSequence     sequence,
    tsp00_Int4               index,
    tsp00_MoveObjPtr      &  fieldbuf,
    tsp00_Int4            &  fieldlen,
    pasbool               &  isnull,
    tak104_MsgList           msgList);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal tak104_MemorySequence ak104_GetReference_MS (
    tak104_MemorySequence     sequence);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void ak104_DropReference_MS (
    tak104_MemorySequence  &  sequence);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void ak104_SetRefcountMode_MS (
    tak104_MemorySequence     sequence,
    tak104_refcountMode_Param     refMode);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal integer ak104_GetCurrentRefcount_MS (
    tak104_MemorySequence     sequence);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal pasbool ak104_Store_IFS (
    tak104_IFieldSequence     sequence,
    tak104_IPersistenceStream     stream,
    tak104_MsgList           msgList);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal pasbool ak104_RestoreAsIncrementalMemorySequence (
    tak104_IPersistenceStream     stream,
    tak104_Allocator         allocator,
    tak104_refcountMode_Param     refMode,
    tak104_IncrementalMemorySequence  &  sequence,
    tak104_MsgList           msgList);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal pasbool ak104_RestoreAsSequenceMemoryCopy (
    tak104_IPersistenceStream     stream,
    tak104_Allocator         allocator,
    tak104_refcountMode_Param     refMode,
    tak104_SequenceMemoryCopy  &  sequence,
    tak104_MsgList           msgList);

/* Don't edit this file by hand, it is generated by ptocProto.py */


#endif

