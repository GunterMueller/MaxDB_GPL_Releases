/*!*********************************************************************

    @file         SAPDBFields_IncrementalMemorySequence.hpp

    @author       DanielD

    @brief        A SAPDBFields_IFieldSequence than can be filled
                  incrementally without knowing the number of fields
                  on object creation.

    One use case: create a IncrementalMemorySequence to hold all
    the SQL literals (because neither the number of literals
    nor the accumulated size of all the literals is known). This
    Sequence uses the task local allocator. Once all the literals have
    been read, copy them to a SAPDBFields_SequenceMemoryCopy using the
    Shared SQL allocator. This combines fast storing of literals
    during parsing and the most memory efficient representation
    in the Shared SQL cache.

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

#ifndef SAPDBFIELDS_INCREMENTALMEMORYSEQUENCE_HPP
#define SAPDBFIELDS_INCREMENTALMEMORYSEQUENCE_HPP

#include "SAPDBCommon/Fields/SAPDBFields_MemorySequence.hpp"

class SAPDBMem_IRawAllocator;
class SAPDB_OStream;
/*!

  @class SAPDBFields_IncrementalMemorySequence

  @brief both the number of fields and the memory buffer
         can grow

 */
class SAPDBFields_IncrementalMemorySequence : public SAPDBFields_MemorySequence
{
public:
    /*!
        @brief create a new SAPDBFields_IncrementalMemorySequence
    */
    SAPDBFields_IncrementalMemorySequence (
            SAPDBMem_IRawAllocator & allocator,
            RefcountMode             refMode,
            int                      fieldCountEstimate = 1,
            int                      bufsizeEstimate = 0);

    /*!
        @brief release resources
    */
    ~SAPDBFields_IncrementalMemorySequence ();

    /*!
        @brief reserve a new field of size bytes
        @returns false if no new field could be allocated
    */
    bool CreateNullField ();

    /*!
        @brief reserve a new field of size bytes
        @returns a pointer to the field memory
         or NULL if no new field could be allocated
    */
    tsp00_Byte * CreateFixedField (int size);

    /*!
        @brief reserve a new field of an unknown size

        CloseCurrentField must be called before the next call to
        CreateFixedField or CreateVariableField.

        @returns a pointer to the field memory
         or NULL if no new field could be allocated
    */
    tsp00_Byte * CreateVariableField (int maxSize);

    /*!
        @brief set the actual size of the last field
    */
    void CloseCurrentField (int actualLength);

    /*!
        @brief set the length of field index to zero

        This doesn't release the memory. But a copy of this sequence
        will copy only an empty field.
    */
    void TruncateField (int index);

    /*!
        @brief calculate the numbers of bytes allocated, but not used
    */
    int CalculateSlack (bool includeAdminOverhead = false) const;

    /*!
        @brief not implemented
    */
    void Print(SAPDB_OStream &) const {};

protected:
    /*!
        @brief return the allocator needed to destroy the object
    */
    virtual SAPDBMem_IRawAllocator * GetAllocator () const;


private:
    SAPDBMem_IRawAllocator * allocator;
    int                      maxFieldCount;
    bool                     fieldIsOpen;
    SAPDBMem_IRawAllocator * dataAllocator;
};


#endif
