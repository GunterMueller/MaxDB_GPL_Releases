/*!*********************************************************************

    @file         SAPDBFields_MemorySequence.cpp

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

#include "SAPDBCommon/Fields/SAPDBFields_MemorySequence.hpp"
#include "SAPDBCommon/MemoryManagement/SAPDBMem_IRawAllocator.hpp"
#include "SAPDBCommon/Tracing/SAPDBTrace_Usage.hpp"
#include "SAPDBCommon/Tracing/SAPDBTrace_Topic.hpp"
#include "SAPDBCommon/SAPDB_ToString.hpp"
#include "hgg01_1.h"    // g01abort
#include "gsp03_1.h"
#include "RunTime/System/RTESys_AtomicOperation.hpp"

#if defined (SAPDB_SLOW)
extern SAPDBTrace_Topic Memory_Trace;
#endif

static const char * const modeNamesC [] = {
    "invalidCount", 
    "noCount", 
    "sessionCount", 
    "sharedCount",
    "<invalid>"
};
/*----------------------------------------*/

SAPDBFields_MemorySequence::
SAPDBFields_MemorySequence (
    RefcountMode refMode_init)

{
    this->refCount = 1;
    this->fields = NULL;
    this->count = 0;
    this->refMode = refMode_init;
    SAPDBTRACE_WRITELN (Memory_Trace, 8, 
        modeNamesC [this->refMode] << " MemorySequence at "
        << SAPDB_ToStringClass ((const void *)this) << " created with 1 reference");
}

/*----------------------------------------*/

SAPDBFields_MemorySequence::
SAPDBFields_MemorySequence (
    SAPDBFields_Field * fields,
    int                 fieldCount)
{
    this->refCount = 1;
    this->fields = fields;
    this->count = fieldCount;
    this->refMode = noCount;
    SAPDBTRACE_WRITELN (Memory_Trace, 8, "MemorySequence at "
        << SAPDB_ToStringClass ((const void *)this) << " created with 1 reference");
}

/*----------------------------------------*/

SAPDBFields_MemorySequence::
~SAPDBFields_MemorySequence ()
{
}

/*----------------------------------------*/

SAPDBMem_IRawAllocator * SAPDBFields_MemorySequence::
GetAllocator () const
{
    return NULL;
}

/*----------------------------------------*/

SAPDBFields_IFieldSequence::IndexType SAPDBFields_MemorySequence::
GetFieldCount() const
{
    return this->count;
};

/*----------------------------------------*/

SAPDBFields_Field& SAPDBFields_MemorySequence::
GetField (
    IndexType          FieldNo,
    SAPDBFields_Field &Field ) const
{
    Field = this->fields [FieldNo];
    return Field;
}

/*----------------------------------------*/

SAPDBFields_IFieldSequence::GetFieldResult SAPDBFields_MemorySequence::
GetField (
    IndexType                    fieldno,
    SAPDBFields_AttributedField &field,
    SAPDBErr_MessageList        &errlist) const
{
    SAPDBFields_Field & localField = this->fields [fieldno];
    SAPDBFields_AttributedField::Attribute attr;

    if (localField.GetDataPtr () == NULL) {
        attr = SAPDBFields_AttributedField::NullAttribute;
    }
    else {
        attr = SAPDBFields_AttributedField::ValueAttribute;
    }
    field = SAPDBFields_AttributedField (
        localField.GetDataPtr (),
        localField.GetLength (),
        attr);
    return SAPDBFields_IFieldSequence::fieldFound;
}

/*----------------------------------------*/

SAPDBFields_Field::SizeType SAPDBFields_MemorySequence::
GetFieldLength(
    IndexType FieldNo) const
{
    return this->fields [FieldNo].GetLength ();
}

/*----------------------------------------*/

SAPDBFields_Field::SizeType SAPDBFields_MemorySequence::
GetTotalLength() const
{
    SAPDBFields_Field::SizeType sum = 0;
    for (int i = 0; i < this->count; ++i) {
        sum += this->fields [i].GetLength ();
    }
    return sum;
};

/*----------------------------------------*/

SAPDBFields_Field::SizeType SAPDBFields_MemorySequence::
Write(
    SAPDBFields_Field            &Destination,
    SAPDBFields_Field::IndexType  Offset,
    IndexType                     Begin,
    IndexType                     End) const
{
    return 0; // not implemented
}

/*----------------------------------------*/

SAPDBFields_MemorySequence * SAPDBFields_MemorySequence::
GetReference ()
{
    switch (this->refMode) {
    case noCount:
        break;
    case sessionCount:
        ++this->refCount;
        break;
    case sharedCount:
        RTESys_AtomicModify(this->refCount,1);  // PTS 1139076
        break;
    default:
        // need assertion
        break;
    }
    SAPDBTRACE_WRITELN (Memory_Trace, 8, "MemorySequence at "
        << SAPDB_ToStringClass ((const void *)this) << " incremented to "
        <<  this->refCount << " references");
    return this;
}

/*----------------------------------------*/

void SAPDBFields_MemorySequence::
DropReference (
    SAPDBFields_MemorySequence * & sequence)
{
    switch (sequence->refMode) {
    case noCount:
        break;
    case sessionCount:
        --sequence->refCount;
        break;
    case sharedCount:
        RTESys_AtomicModify(sequence->refCount,-1);  // PTS 1139076
        break;
    default:
        // need assertion
        break;
    }
    SAPDBTRACE_WRITELN (Memory_Trace, 8, "MemorySequence at "
        << SAPDB_ToStringClass ((const void *)sequence) << " decremented to "
        <<  sequence->refCount << " references");
    if (sequence->refCount == 0) {
        SAPDBMem_IRawAllocator * allocator = sequence->GetAllocator ();
        if (allocator != NULL) {
            SAPDBTRACE_WRITELN (Memory_Trace, 8, "MemorySequence at "
                << SAPDB_ToStringClass ((const void *)sequence) << " deallocated");
            sequence->~SAPDBFields_MemorySequence ();
            allocator->Deallocate (sequence);
        }
        else {
            SAPDBTRACE_WRITELN (Memory_Trace, 8, "MemorySequence at "
                << SAPDB_ToStringClass ((const void *)sequence) << " kept on stack with 0 references");
        }
    }
    else {
        if (sequence->refCount < 0) {
            g01abort (csp3_a06_fieldlist_reference, csp3_n_fieldlist,
                "reference cnt < 0       ", 0);
        }
    }

    sequence = NULL;
}

/*----------------------------------------*/

void SAPDBFields_MemorySequence::
SetRefcountMode (
    RefcountMode refMode_new)
{
    this->refMode = refMode_new;
}

/*----------------------------------------*/

int SAPDBFields_MemorySequence::
GetCurrentReferenceCount () const
{
    return this->refCount;
}

