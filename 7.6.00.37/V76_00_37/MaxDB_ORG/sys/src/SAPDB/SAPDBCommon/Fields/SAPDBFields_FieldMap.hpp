/*!
  @file           SAPDBFields_FieldMap.hpp
  @author         StefanB,TillW,UweH
  @ingroup        Field Container classes
  @brief          Declarations for class SAPDBFields_FieldMap

\if EMIT_LICENCE
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

\endif
*/


#ifndef SAPDBFields_FieldMap_HPP
#define SAPDBFields_FieldMap_HPP


#include "SAPDBCommon/Fields/SAPDBFields_IFieldSequences.hpp"
#include "Container/Container_Vector.hpp"

/*!
@class          SAPDBFields_FieldMap
@brief          A map of (index,field) entries with fields scattered in memory.
*/

class SAPDBFields_FieldMap : public SAPDBFields_IFieldSequence
{
public:
    /*!
       @brief  Constructs a field map with a amximum of &lt;N> elements.
       @param  RawAllocator [in] The underlying raw allocator, where the entries are located.
               Note, that only the handler to the fields are located in the
               memory of the raw allocator, not the fields themself.
       @param  Max [in] The maximum number of entries in the field map. The actual 
               size of the field map depends at which positions entries have
               been filled into the field map.
       @example
       SAPDBFields_FieldMap MyMap(Alloc, 10);
       // Now the size of MyMap is zero.
       MyMap.Fill(7, 10, field);
       // The size now is 8. ATTENTION! Fields number 0 through 6
       // are not assigned at this stage.
       MyMap.Fill(10, 17, field); // not possible

        The map initially contains no field handler.
        To fill fields into the map the method 'Fill()' must be used
    */
    SAPDBFields_FieldMap ( SAPDBMem_IRawAllocator                &allocator,
                           SAPDBFields_IFieldSequence::IndexType  max);
    /// missing comment
    SAPDBFields_IFieldSequence::IndexType GetIndex ( SAPDBFields_IFieldSequence::IndexType fieldno)
    {
        SAPDBERR_ASSERT_ARGUMENT(fieldno < GetFieldCount());
        return m_Content[fieldno].m_index;
    }
    /// missing omment
    void Fill(      SAPDBFields_IFieldSequence::IndexType  pos,
                    SAPDBFields_IFieldSequence::IndexType  index,
              const SAPDBFields_AttributedField           &field);
    /// Sorts the map by the index (ascending).
    void Sort();

    virtual GetFieldResult GetField ( IndexType                    fieldno,
                                      SAPDBFields_AttributedField &field,
                                      SAPDBErr_MessageList        &errlist) const;
    virtual IndexType GetFieldCount() const;
    virtual SizeType  GetFieldLength(IndexType  fieldno) const;
    virtual SizeType  GetTotalLength() const;

private:
    /// no copy-construction (standard copy-constructor would call
    /// copy-constructor of Container_Vector which is not working at
    /// this time)
    SAPDBFields_FieldMap (const SAPDBFields_FieldMap&);

    struct EntryType
	{
        SAPDBFields_IFieldSequence::IndexType m_index;
        SAPDBFields_AttributedField           m_field;
        EntryType()
        : m_index(0)
        {}
        EntryType(SAPDBFields_IFieldSequence::IndexType  index,
                  const SAPDBFields_AttributedField     &field)
        : m_index(index),
          m_field(field)
        {}
        bool operator==(const EntryType& entry) const
        {
            return (m_index == entry.m_index) && (m_field == entry.m_field);
        }
    };
    /// The content of the field map
    Container_Vector<EntryType> m_Content;
};



#endif // SAPDBFields_FieldMap_HPP

