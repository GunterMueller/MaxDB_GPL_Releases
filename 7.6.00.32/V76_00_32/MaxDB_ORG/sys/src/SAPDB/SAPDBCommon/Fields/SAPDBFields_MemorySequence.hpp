/*!*********************************************************************

    @file         SAPDBFields_MemorySequence.hpp

    @author       DanielD

    @brief        A SAPDBFields_IFieldSequence implemented as
                  an actual array of SAPDBFields_Field.

    The storage location of the actual field content is unspecified.

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

#ifndef SAPDBFIELDS_MEMORYSEQUENCE_HPP
#define SAPDBFIELDS_MEMORYSEQUENCE_HPP

#include "SAPDBCommon/Fields/SAPDBFields_Field.hpp"
#include "SAPDBCommon/Fields/SAPDBFields_IFieldSequences.hpp"

class SAPDBMem_IRawAllocator;

/*!

  @class SAPDBFields_MemorySequence

 */
class SAPDBFields_MemorySequence : public SAPDBFields_IFieldSequence
{
public:
    enum RefcountMode {invalidCount, noCount, sessionCount, sharedCount};

    /*!
        @brief          Returns the number of fields in the field sequence.
        @return         IndexType
    */
    virtual IndexType GetFieldCount() const;

    /*!
        @brief          Returns the &lt;N>-th field within the field sequence.
        @param          FieldNo [in] The index of the field [0...GetFieldCount()-1]
        @param          Field [in/out] The field to be assigned to the &lt;N>-th component
        @return         SAPDBFields_Field
    */
    virtual SAPDBFields_Field& GetField ( IndexType          FieldNo,
                                          SAPDBFields_Field &Field ) const;

    /*!
        @brief          Returns the &lt;N>-th field within the field sequence.
        @param          fieldno [in] The index of the field [0...GetFieldCount()-1]
        @param          field   [in/out] The field to be assigned to the &lt;N>-th component
        @param          errlist [out] if fieldError is returned this contains additional info
        @return         GetFieldResult
    */
    virtual SAPDBFields_IFieldSequence::GetFieldResult GetField (
                                      IndexType                    fieldno,
                                      SAPDBFields_AttributedField &field,
                                      SAPDBErr_MessageList        &errlist) const;

    /*!
        @brief          Returns the length of the field with index &lt;N>.
        @return         SAPDBFields_Field::SizeType:
    */
    virtual SAPDBFields_Field::SizeType GetFieldLength(IndexType FieldNo) const;

    /*!
    @brief          Returns the sum of the length of all fields in the field sequence in bytes.
    @return         SAPDBFields_Field::SizeType:
    */
    virtual SAPDBFields_Field::SizeType GetTotalLength() const;

    /*!
       @brief          Writes all fields from Begin up to End sequentially to
                       Destination End is not included.
       @param          Destination [in] The buffer into which the fields should be written.
       @param          Offset [in] The offset withion the buffer where the writing starts.
       @param          Begin [in] The index of the fields from where the write starts.
       @param          End [in] The index of the field up to which the write works.
                       End is not included in writing.
       @return         SAPDBFields_Field::SizeType: The number of bytes copied.
    */
    virtual SAPDBFields_Field::SizeType Write(SAPDBFields_Field            &Destination,
                                              SAPDBFields_Field::IndexType  Offset,
                                              IndexType                     Begin,
                                              IndexType                     End) const;

    /*!
        @brief return a new reference to this

        This will increase the internal reference counter. Objects are
        created with a reference count of 1, it is therefor not necessary
        to call this method after object creation.
    */
    SAPDBFields_MemorySequence * GetReference ();

    /*!
        @brief set parameter sequence to NULL

        This will decrease the reference counter of sequence. If the
        reference count drops to 0, sequence will be destroyed.
    */
    static void DropReference (SAPDBFields_MemorySequence * & sequence);

    /*!
        @brief set reference counting to off/session local/shared
     */
    void SetRefcountMode (RefcountMode refMode);

    /*!
        @brief return the current number of references
     */
    int GetCurrentReferenceCount () const;

protected:
    /*!
        @brief create a new empty SAPDBFields_MemorySequence

        @param refMode set reference counting to off/session local/shared
    */
    SAPDBFields_MemorySequence (RefcountMode refMode);

    /*!
        @brief create a new SAPDBFields_MemorySequence with fieldCount fields
    */
    SAPDBFields_MemorySequence (
            SAPDBFields_Field * fields,
            int                  fieldCount);

    /*!
        @brief release resources
    */
    virtual ~SAPDBFields_MemorySequence ();

    /*!
        @brief return the allocator needed to destroy the object
    */
    virtual SAPDBMem_IRawAllocator * GetAllocator () const;

protected:
    int                      refCount;
    int                      count;
    SAPDBFields_Field      * fields;
    RefcountMode             refMode;
};

#endif

