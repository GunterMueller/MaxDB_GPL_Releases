/*!*********************************************************************

    @file         SAPDBFields_SequenceMemoryCopy.cpp

    @author       DanielD

    @brief

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

#include "SAPDBCommon/Fields/SAPDBFields_SequenceMemoryCopy.hpp"

#include "gsp00.h"
#include "SAPDBCommon/MemoryManagement/SAPDBMem_IRawAllocator.hpp"
#include "SAPDBCommon/MemoryManagement/SAPDBMem_NewDestroy.hpp"
#include "SAPDBCommon/Fields/SAPDBFields_AttributedField.hpp"
#include "SAPDBCommon/SAPDB_IPersistenceStream.hpp"

/*----------------------------------------*/

SAPDBFields_SequenceMemoryCopy::
SAPDBFields_SequenceMemoryCopy (
    SAPDBFields_MemorySequence::RefcountMode refMode)
: SAPDBFields_MemorySequence (refMode)
{
    this->mem = NULL;
}

/*----------------------------------------*/

SAPDBFields_SequenceMemoryCopy::
SAPDBFields_SequenceMemoryCopy (
    SAPDBMem_IRawAllocator     & allocator_in,
    SAPDBFields_IFieldSequence * source,
    SAPDBFields_MemorySequence::RefcountMode refMode)
: SAPDBFields_MemorySequence (refMode)
{
    Msg_List msgList;
    this->initFromSequence (allocator_in, source, msgList);
}

/*----------------------------------------*/

SAPDBFields_SequenceMemoryCopy * SAPDBFields_SequenceMemoryCopy::
Create (
    SAPDBMem_IRawAllocator     & allocator_in,
    SAPDBFields_IFieldSequence * source,
    SAPDBFields_MemorySequence::RefcountMode refMode,
    Msg_List                   & msgList)
{
    SAPDBFields_SequenceMemoryCopy * result;
    result = new (allocator_in) SAPDBFields_SequenceMemoryCopy (refMode);
    if (result == NULL) {
        return NULL;
    }
    if (!result->initFromSequence (allocator_in, source, msgList)) {
        destroy (result, allocator_in);
        return NULL;
    }
    return result;
}

/*----------------------------------------*/

SAPDBFields_SequenceMemoryCopy::
SAPDBFields_SequenceMemoryCopy (
    SAPDBMem_IRawAllocator   & allocator_in,
    SAPDB_IPersistenceStream & instream,
    int                        fieldCount,
    int                        allDataLength,
    SAPDBFields_MemorySequence::RefcountMode refMode,
    Msg_List                 & msgList)
: SAPDBFields_MemorySequence (refMode)
{
    this->initFromStream (allocator_in, instream, fieldCount, allDataLength, msgList);
}

/*----------------------------------------*/

SAPDBFields_SequenceMemoryCopy * SAPDBFields_SequenceMemoryCopy::
Create (
    SAPDBMem_IRawAllocator   & allocator_in,
    SAPDB_IPersistenceStream & instream,
    int                        fieldCount,
    int                        allDataLength,
    SAPDBFields_MemorySequence::RefcountMode refMode,
    Msg_List                 & msgList)
{
    SAPDBFields_SequenceMemoryCopy * result;
    result = new (allocator_in) SAPDBFields_SequenceMemoryCopy (refMode);
    if (result == NULL) {
        return NULL;
    }
    if (!result->initFromStream (allocator_in, instream, fieldCount, allDataLength, msgList)) {
        destroy (result, allocator_in);
    }
    return result;
}

/*----------------------------------------*/

SAPDBFields_SequenceMemoryCopy::
~SAPDBFields_SequenceMemoryCopy ()
{
    if (this->mem) {
        this->allocator->Deallocate (this->mem);
    }
}

/*----------------------------------------*/

SAPDBMem_IRawAllocator * SAPDBFields_SequenceMemoryCopy::
GetAllocator () const
{
    return this->allocator;
}

/*----------------------------------------*/

bool SAPDBFields_SequenceMemoryCopy::
initFromSequence (
        SAPDBMem_IRawAllocator     & allocator_in,
        SAPDBFields_IFieldSequence * source,
        Msg_List                   & msgList)
{
    /*
     * alloc memory
     */
    this->allocator = &allocator_in;
    int fieldCount = source->GetFieldCount ();
    int dataLength = source->GetTotalLength ();
    int fieldsSize = fieldCount * sizeof (SAPDBFields_Field);
    int requiredLength = fieldsSize + dataLength;
    this->count = fieldCount;
    this->mem = static_cast<tsp00_Byte*> (this->allocator->Allocate (requiredLength));
    if (this->mem == NULL) {
        return false;
    }
    this->fields = reinterpret_cast<SAPDBFields_Field *>(this->mem);
    /*
     * copy fields
     */
    tsp00_Byte * writePosition = this->mem + fieldsSize;
    SAPDBFields_AttributedField sourceField;
    for (int i = 0; i < fieldCount; ++i) {
        source->GetField (i, sourceField, msgList);
        int fieldLen = sourceField.GetLength ();
        memcpy (writePosition, sourceField.GetDataPtr (), fieldLen);
        this->fields[i].Assign (writePosition, fieldLen);
        writePosition += fieldLen;
    }
    return true;
}

/*----------------------------------------*/

bool SAPDBFields_SequenceMemoryCopy::
initFromStream (
        SAPDBMem_IRawAllocator   & allocator_in,
        SAPDB_IPersistenceStream & instream,
        int                        fieldCount,
        int                        allDataLength,
        Msg_List                 & msgList)
{
    /*
     * alloc memory
     */
    this->allocator = &allocator_in;
    int fieldsSize = fieldCount * sizeof (SAPDBFields_Field);
    int requiredLength = fieldsSize + allDataLength;
    this->count = fieldCount;
    this->mem = static_cast<tsp00_Byte*> (this->allocator->Allocate (requiredLength));
    if (this->mem == NULL) {
        return false;
    }
    this->fields = reinterpret_cast<SAPDBFields_Field *>(this->mem);
    /*
     * read fields
     */
    tsp00_Byte * writePosition = this->mem + fieldsSize;
    for (int i = 0; i < fieldCount; ++i) {
        tsp00_Uint2 fieldLen;
        if (!instream.ReadBytes (&fieldLen, sizeof (fieldLen), msgList)) {
            return false;
        }
        if (!instream.ReadBytes (writePosition, fieldLen, msgList)) {
            return false;
        }
        this->fields[i].Assign (writePosition, fieldLen);
        writePosition += fieldLen;
    }
    return true;
}

/*----------------------------------------*/

