/*!*********************************************************************

    @file         vak104.cpp

    @author       DanielD

    @brief        Pascal wrapper around fields and sequences

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
***********************************************************************/

#include "hak104.h"

#include "SAPDBCommon/Fields/SAPDBFields_IncrementalMemorySequence.hpp"
#include "SAPDBCommon/Fields/SAPDBFields_Persistence.hpp"
#include "SAPDBCommon/Fields/SAPDBFields_AttributedField.hpp"
#include "SAPDBCommon/MemoryManagement/SAPDBMem_IRawAllocator.hpp"
#include "SAPDBCommon/MemoryManagement/SAPDBMem_NewDestroy.hpp"

/*----------------------------------------*/

inline static SAPDBFields_MemorySequence::RefcountMode pasMode2cppMode (
    tak104_refcountMode_Param refMode)
{
    SAPDBFields_MemorySequence::RefcountMode result;

    switch (refMode) {
    case noCount_eak104:
        result = SAPDBFields_MemorySequence::noCount;
        break;
    case sessionCount_eak104:
        result = SAPDBFields_MemorySequence::sessionCount;
        break;
    case sharedCount_eak104:
        result = SAPDBFields_MemorySequence::sharedCount;
        break;
    default:
        result = SAPDBFields_MemorySequence::invalidCount;
        break;
    }
    return result;
}

/*----------------------------------------*/

externPascal pasbool ak104_CreateIncrementalMemorySequence (
    tak104_Allocator                     allocator,
    tak104_refcountMode_Param            refMode,
    tak104_IncrementalMemorySequence  &  sequence,
    tak104_MsgList                       msgList)
{
    SAPDBFields_IncrementalMemorySequence * result =
        new (*allocator) SAPDBFields_IncrementalMemorySequence (*allocator, 
                                                              pasMode2cppMode (refMode));
    if (result == NULL) {
        return false;
    }
    sequence = reinterpret_cast<tak104_IncrementalMemorySequence> (result);
    return true;
}

/*----------------------------------------*/

externPascal pasbool ak104_DeleteIncrementalMemorySequence (
    tak104_Allocator         allocator,
    tak104_IncrementalMemorySequence  &  sequence,
    tak104_MsgList                       msgList)
{
    destroy (sequence, *allocator);
    return true;
}

/*----------------------------------------*/

externPascal pasbool ak104_DeleteIFieldSequence (
    tak104_Allocator         allocator,
    tak104_IFieldSequence &  sequence,
    tak104_MsgList           msgList)
{
    destroy (sequence, *allocator);
    return true;
}

/*----------------------------------------*/

externPascal pasbool ak104_CreateNullField_IMS (
    tak104_IncrementalMemorySequence     sequence,
    tak104_MsgList                       msgList)
{
    bool ok = sequence->CreateNullField ();
    return ok;
}

/*----------------------------------------*/

externPascal pasbool ak104_CreateFixedField_IMS (
    tak104_IncrementalMemorySequence     sequence,
    tsp00_Int4               size,
    tsp00_MoveObjPtr      &  buf,
    tak104_MsgList           msgList)
{
    buf = reinterpret_cast<tsp00_MoveObjPtr>(sequence->CreateFixedField (size));
    if (buf == NULL) {
        return false;
    }
    else {
        return true;
    }
}

/*----------------------------------------*/

externPascal pasbool ak104_CreateVariableField_IMS (
    tak104_IncrementalMemorySequence     sequence,
    tsp00_Int4               size,
    tsp00_MoveObjPtr      &  buf,
    tak104_MsgList           msgList)
{
    buf = reinterpret_cast<tsp00_MoveObjPtr>(sequence->CreateVariableField (size));
    if (buf == NULL) {
        return false;
    }
    else {
        return true;
    }
}

/*----------------------------------------*/

externPascal pasbool ak104_CloseCurrentField_IMS (
    tak104_IncrementalMemorySequence     sequence,
    tsp00_Int4               size,
    tak104_MsgList           msgList)
{
    sequence->CloseCurrentField (size);
    return true;
}

/*----------------------------------------*/

externPascal void ak104_TruncateField_IMS (
    tak104_IncrementalMemorySequence     sequence,
    tsp00_Int4               index)
{
    sequence->TruncateField (index);
}

/*----------------------------------------*/

externPascal tsp00_Int4 ak104_CalculateSlack_IMS (
    tak104_IncrementalMemorySequence     sequence)
{
    return sequence->CalculateSlack ();
}

/*----------------------------------------*/

externPascal tsp00_Int4 ak104_GetFieldCount_IMS (
    tak104_IncrementalMemorySequence     sequence)
{
    return sequence->GetFieldCount ();
}

/*----------------------------------------*/

externPascal pasbool ak104_GetField_IMS (
    tak104_IncrementalMemorySequence     sequence,
    tsp00_Int4               index,
    tsp00_MoveObjPtr      &  fieldbuf,
    tsp00_Int4            &  fieldlen,
    pasbool               &  isnull,
    tak104_MsgList           msgList)
{
    SAPDBFields_Field field;
    sequence->GetField (index, field);
    if (field.GetDataPtr () == NULL) {
        isnull = true;
    }
    else {
        fieldbuf = reinterpret_cast<tsp00_MoveObjPtr>(field.GetDataPtr ());
        fieldlen = field.GetLength ();
        isnull = ((char)csp_undef_byte == (char)(*fieldbuf)[0]) ? true : false;
    }
    return true;
}

/*----------------------------------------*/

externPascal tsp00_Int4 ak104_GetFieldCount_IFS (
    tak104_IFieldSequence     sequence)
{
    return sequence->GetFieldCount ();
}

/*----------------------------------------*/

externPascal pasbool ak104_GetField_IFS (
    tak104_IFieldSequence     sequence,
    tsp00_Int4               index,
    tsp00_MoveObjPtr      &  fieldbuf,
    tsp00_Int4            &  fieldlen,
    pasbool               &  isnull,
    tak104_MsgList           msgList)
{
    SAPDBFields_AttributedField field;
    SAPDBErr_MessageList * realMsgList = reinterpret_cast<SAPDBErr_MessageList *>(msgList);
    if (sequence->GetField (index, field, *realMsgList) != SAPDBFields_IFieldSequence::fieldFound) {
        return false;
    }
    if (field.GetAttribute () == SAPDBFields_AttributedField::NullAttribute) {
        isnull = true;
    }
    else {
        fieldbuf = reinterpret_cast<tsp00_MoveObjPtr>(field.GetDataPtr ());
        fieldlen = field.GetLength ();
        isnull = ((char)csp_undef_byte == (char)(*fieldbuf)[0]) ? true : false;
    }
    return true;
}

/*----------------------------------------*/

externPascal tak104_MemorySequence ak104_GetReference_MS (
    tak104_MemorySequence     sequence)
{
    return sequence->GetReference ();
}

/*----------------------------------------*/

externPascal void ak104_DropReference_MS (
    tak104_MemorySequence  &  sequence)
{
    SAPDBFields_MemorySequence::DropReference (sequence);
}

/*----------------------------------------*/

externPascal void ak104_SetRefcountMode_MS (
    tak104_MemorySequence     sequence,
    tak104_refcountMode_Param     refMode)
{
    sequence->SetRefcountMode (pasMode2cppMode (refMode));
}

/*----------------------------------------*/

externPascal integer ak104_GetCurrentRefcount_MS (
    tak104_MemorySequence     sequence)
{
    return sequence->GetCurrentReferenceCount ();
}

/*----------------------------------------*/

externPascal pasbool ak104_Store_IFS (
    tak104_IFieldSequence     sequence,
    tak104_IPersistenceStream     stream,
    tak104_MsgList           msgList)
{
    SAPDBErr_MessageList * realMsgList = reinterpret_cast<SAPDBErr_MessageList *>(msgList);
    return SAPDBFields_Persistence::Store (*sequence, *stream, *realMsgList);
}

/*----------------------------------------*/

externPascal pasbool ak104_RestoreAsIncrementalMemorySequence (
    tak104_IPersistenceStream     stream,
    tak104_Allocator                     allocator,
    tak104_refcountMode_Param            refMode,
    tak104_IncrementalMemorySequence  &  sequence,
    tak104_MsgList                       msgList)
{
    SAPDBErr_MessageList * realMsgList = reinterpret_cast<SAPDBErr_MessageList *>(msgList);
    tak104_IncrementalMemorySequence result =
        SAPDBFields_Persistence::RestoreAsIncrementalMemorySequence (
            *stream, *allocator, pasMode2cppMode (refMode), *realMsgList);
    return result != NULL;
}

/*----------------------------------------*/

externPascal pasbool ak104_RestoreAsSequenceMemoryCopy (
    tak104_IPersistenceStream     stream,
    tak104_Allocator              allocator,
    tak104_refcountMode_Param     refMode,
    tak104_SequenceMemoryCopy  &  sequence,
    tak104_MsgList                msgList)
{
    SAPDBErr_MessageList * realMsgList = reinterpret_cast<SAPDBErr_MessageList *>(msgList);
    tak104_SequenceMemoryCopy result =
        SAPDBFields_Persistence::RestoreAsSequenceMemoryCopy (
            *stream, *allocator, pasMode2cppMode (refMode), *realMsgList);
    return result != NULL;
}

/*----------------------------------------*/


