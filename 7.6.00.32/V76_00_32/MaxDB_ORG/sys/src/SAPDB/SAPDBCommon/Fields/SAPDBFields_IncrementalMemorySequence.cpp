/*!*********************************************************************

    @file         SAPDBFields_IncrementalMemorySequence.cpp

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

#include "SAPDBCommon/Fields/SAPDBFields_IncrementalMemorySequence.hpp"

#include "SAPDBCommon/MemoryManagement/SAPDBMem_IRawAllocator.hpp"
#include "SAPDBCommon/MemoryManagement/SAPDBMem_NewDestroy.hpp"
#include "SAPDBCommon/ErrorsAndMessages/SAPDBErr_Contracts.hpp"

typedef struct MemPageT {
    MemPageT     * next;
    unsigned int   size;
    unsigned int   used;
    union {
        void * alignment;
        char   data [1];
    };
} MemPageT;


class IncrementalAllocator : public SAPDBMem_IRawAllocator
{
public:
    static IncrementalAllocator * create (SAPDBMem_IRawAllocator * innerAllocator,
                                        int                      initialSize);
    static void destroy (IncrementalAllocator * allocator);
    virtual void* Allocate(SAPDB_ULong ByteCount);
    virtual void* Allocate(SAPDB_ULong ByteCount, const void* Hint);
    virtual void  Deallocate(void* p);
    virtual void  GetBaseAllocatorCallStatistics(SAPDB_ULong &,SAPDB_ULong &) const;
    virtual void  GetCallStatistics(SAPDB_ULong &,SAPDB_ULong &) const;
    void popBytes (int count);
    bool addPage (SAPDB_ULong minAvailable);
    int  calculateSlack (bool includeAdminOverhead = false) const;

private:
    bool findSlackPage (SAPDB_ULong bytesRequired);
    SAPDBMem_IRawAllocator * innerAllocator;
    MemPageT *               mem;
};


/*----------------------------------------*/

SAPDBFields_IncrementalMemorySequence::
SAPDBFields_IncrementalMemorySequence (
    SAPDBMem_IRawAllocator & allocator_in,
    RefcountMode             refMode,
    int                      fieldCountEstimate,
    int                      bufsizeEstimate)
: SAPDBFields_MemorySequence (refMode)
{
    this->allocator = &allocator_in;
    newarray (this->fields, fieldCountEstimate, allocator_in);
    this->count = 0;
    this->maxFieldCount = fieldCountEstimate;
    this->fieldIsOpen = false;
    this->dataAllocator = IncrementalAllocator::create (&allocator_in, bufsizeEstimate);
}

/*----------------------------------------*/

SAPDBFields_IncrementalMemorySequence::
~SAPDBFields_IncrementalMemorySequence ()
{
    if (this->dataAllocator != NULL) {
        IncrementalAllocator::destroy ((IncrementalAllocator*) this->dataAllocator);
    }
    if (this->fields) {
        this->allocator->Deallocate (this->fields);
    }
}

/*----------------------------------------*/

bool SAPDBFields_IncrementalMemorySequence::
CreateNullField ()
{
    tsp00_Byte * buf = this->CreateFixedField (0);
    if (buf == NULL) {
        return false;
    }
    this->fields [this->count - 1].Assign (NULL, 0);
    return true;
}

/*----------------------------------------*/

tsp00_Byte * SAPDBFields_IncrementalMemorySequence::
CreateFixedField (
    int size)
{
    SAPDBERR_PRECONDITION (!this->fieldIsOpen);
    /*
     * grow number of fields
     */
    if (this->count == this->maxFieldCount) {
        int newMaxFieldCount = this->maxFieldCount * 2;
        // allocate new array
        SAPDBFields_Field * newfields;
        newarray (newfields, newMaxFieldCount, *this->allocator);
        if (newfields == NULL) {
            return NULL;
        }
        // copy existing fields
        for (int i = 0; i < this->count; ++i) {
            newfields [i] = this->fields [i];
        }
        // delete old array
        this->allocator->Deallocate (this->fields);
        // set instance variables to new values
        this->fields = newfields;
        this->maxFieldCount = newMaxFieldCount;
    }
    /*
     * allocate data
     */
    tsp00_Byte * data = (tsp00_Byte*) this->dataAllocator->Allocate (size);
    if (data == NULL) {
        return NULL;
    }
    SAPDBFields_Field * field = &this->fields [this->count];
    field->Assign (data, size);
    ++this->count;
    /*
     * return
     */
    return data;
}

/*----------------------------------------*/

tsp00_Byte * SAPDBFields_IncrementalMemorySequence::
CreateVariableField (
    int maxSize)
{
    tsp00_Byte * result = this->CreateFixedField (maxSize);
    this->fieldIsOpen = true;
    return result;
}

/*----------------------------------------*/

void SAPDBFields_IncrementalMemorySequence::
CloseCurrentField (
    int actualLength)
{
    SAPDBERR_PRECONDITION (this->fieldIsOpen);
    SAPDBFields_Field * current = &this->fields [this->count - 1];
    int reservedLength = current->GetLength ();
    tsp00_Byte * fieldStart = current->GetDataPtr ();
    current->Assign (fieldStart, actualLength);
    ((IncrementalAllocator*)this->dataAllocator)
        ->popBytes (reservedLength - actualLength);
    this->fieldIsOpen = false;
}

/*----------------------------------------*/

void SAPDBFields_IncrementalMemorySequence::
TruncateField (
    int index)
{
    SAPDBERR_PRECONDITION ((0 <= index) && (index < this->count));
    SAPDBFields_Field * current = &this->fields [index];
    // possible todo: check if memory is at end of chunc
    current->Assign (NULL, 0);
}

/*----------------------------------------*/

int  SAPDBFields_IncrementalMemorySequence::
CalculateSlack (
    bool includeAdminOverhead) const
{
    IncrementalAllocator * inc = (IncrementalAllocator*) this->dataAllocator;
    return inc->calculateSlack (includeAdminOverhead);
}

/*----------------------------------------*/

SAPDBMem_IRawAllocator * SAPDBFields_IncrementalMemorySequence::
GetAllocator () const
{
    return this->allocator;
}

/*----------------------------------------*/

IncrementalAllocator * IncrementalAllocator::
create (
    SAPDBMem_IRawAllocator * innerAllocator,
    int                      initialSize)
{
    IncrementalAllocator tmpAlloc;
    tmpAlloc.innerAllocator = innerAllocator;
    tmpAlloc.mem = NULL;
    if (!tmpAlloc.addPage (initialSize)) {
        return NULL;
    }
    IncrementalAllocator * result = new (tmpAlloc) IncrementalAllocator;
    *result = tmpAlloc;
    return result;
}

/*----------------------------------------*/

void IncrementalAllocator::
destroy (
    IncrementalAllocator * allocator)
{
    SAPDBMem_IRawAllocator * innerAllocator = allocator->innerAllocator;
    MemPageT * current = allocator->mem;
    while (current != NULL) {
        MemPageT * next = current->next;
        innerAllocator->Deallocate (current);
        current = next;
    }
}

/*----------------------------------------*/

void* IncrementalAllocator::
Allocate(
    SAPDB_ULong ByteCount)
{
    if (this->mem->size - this->mem->used < ByteCount) {
        bool slackFound = this->findSlackPage (ByteCount);
        if (!slackFound) {
            if (!this->addPage (ByteCount)) {
                return NULL;
            }
        }
    }
    void * result = this->mem->data + this->mem->used;
    this->mem->used += ByteCount;
    return result;
}

/*----------------------------------------*/

void* IncrementalAllocator::
Allocate(
    SAPDB_ULong ByteCount,
    const void* Hint)
{
    return this->Allocate (ByteCount);
}

/*----------------------------------------*/

void  IncrementalAllocator::
Deallocate(
    void* p)
{
    SAPDBERR_PRECONDITION (!"Can't deallocate from incremental allocator");
}

/*----------------------------------------*/

void IncrementalAllocator::
popBytes (
    int count)
{
    SAPDBERR_PRECONDITION (count >= 0);
    SAPDBERR_PRECONDITION ((int) this->mem->used >= count);
    this->mem->used -= count;
}

/*----------------------------------------*/

bool IncrementalAllocator::
addPage(
    SAPDB_ULong minAvailable)
{
    SAPDB_ULong allocSize = 1000;
    if (minAvailable > allocSize) {
        allocSize = minAvailable;
    }
    MemPageT * newpage = (MemPageT*) this->innerAllocator->Allocate (
        allocSize + offsetof (MemPageT, data));
    if (newpage == NULL) {
        return false;
    }
    newpage->next = this->mem;
    this->mem = newpage;
    newpage->size = allocSize;
    newpage->used = 0;
    return true;
}


/*----------------------------------------*/

void IncrementalAllocator::
GetBaseAllocatorCallStatistics(
    SAPDB_ULong &,
    SAPDB_ULong &) const
{
}

/*----------------------------------------*/

void IncrementalAllocator::
GetCallStatistics(
    SAPDB_ULong &,
    SAPDB_ULong &) const
{
}

/*----------------------------------------*/

int IncrementalAllocator::
calculateSlack (
    bool includeAdminOverhead) const
{
    int result = 0;
    if (includeAdminOverhead) {
        result += sizeof (IncrementalAllocator);
    }
    MemPageT * current = this->mem;
    while (current != NULL) {
        result += current->size - current->used;
        if (includeAdminOverhead) {
            result += offsetof (MemPageT, data);
        }
        current = current->next;
    }
    return result;
}

/*----------------------------------------*/

bool IncrementalAllocator::
findSlackPage (
    SAPDB_ULong bytesRequired)
{
    MemPageT * current = this->mem;
    MemPageT ** previousAddr = &this->mem;
    while (current != NULL) {
        if (current->size - current->used >= bytesRequired) {
            *previousAddr = current->next;
            this->mem = current;
            return true;
        }
        previousAddr = &current->next;
        current = current->next;
    }
    return false;
}

/*----------------------------------------*/

