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
#include "SAPDBCommon/Fields/SAPDBFields_FieldMap.hpp"
#include "SAPDBCommon/Fields/SAPDBFields_Exceptions.hpp"

//--------------------------------------------------------------------------------------------
SAPDBFields_FieldMap::SAPDBFields_FieldMap(SAPDBMem_IRawAllocator                &allocator, 
                                           SAPDBFields_IFieldSequence::IndexType  max)
 : m_Content(allocator)
{
    m_Content.Reserve(max);
}

//--------------------------------------------------------------------------------------------
SAPDBFields_IFieldSequence::IndexType
SAPDBFields_FieldMap::GetFieldCount() const
{
    return m_Content.GetSize();
}

//--------------------------------------------------------------------------------------------
SAPDBFields_IFieldSequence::SizeType
SAPDBFields_FieldMap::GetFieldLength(SAPDBFields_IFieldSequence::IndexType index) const
{
    return m_Content[index].m_field.GetLength();
}

//--------------------------------------------------------------------------------------------
SAPDBFields_IFieldSequence::GetFieldResult
SAPDBFields_FieldMap::GetField ( SAPDBFields_IFieldSequence::IndexType  fieldno,
                                 SAPDBFields_AttributedField           &field,
                                 SAPDBErr_MessageList                  &errlist) const
{
    if ( fieldno >= GetFieldCount() )
    {
        errlist = Fields_Exception(__CONTEXT__, FIELD_INVALID_FIELDNO,
                                   SAPDB_ToString(fieldno),
                                   SAPDB_ToString(GetFieldCount()));
        return SAPDBFields_IFieldSequence::fieldError;
    }
    field = m_Content[fieldno].m_field;
    return SAPDBFields_IFieldSequence::fieldFound;
}

//--------------------------------------------------------------------------------------------
void SAPDBFields_FieldMap::Fill(SAPDBFields_IFieldSequence::IndexType  pos,
                                SAPDBFields_IFieldSequence::IndexType  index,
                                const SAPDBFields_AttributedField     &field)
{
    SAPDBERR_ASSERT_ARGUMENT(pos < m_Content.GetCapacity());
    if (!(pos < m_Content.GetSize()))
        m_Content.Resize(pos+1);
    m_Content[pos] = EntryType(index, field);
}

//--------------------------------------------------------------------------------------------
SAPDBFields_IFieldSequence::SizeType
SAPDBFields_FieldMap::GetTotalLength() const
{
    SAPDBFields_IFieldSequence::IndexType pos;
    SAPDBFields_AttributedField           field;
    SAPDBFields_IFieldSequence::SizeType  length = 0;
    for (pos=0; pos<GetFieldCount(); ++pos)
	{
        length += m_Content[pos].m_field.GetLength();
    }
    return length;
}

//--------------------------------------------------------------------------------------------
void SAPDBFields_FieldMap::Sort()
{
    EntryType Entry;
    IndexType i, j;
    for (i=0; i<GetFieldCount(); ++i) {
        for (j=i+1; j<GetFieldCount(); ++j) {
            if (GetIndex(i) > GetIndex(j)) {
                Entry = m_Content[i];
                m_Content[i] = m_Content[j];
                m_Content[j] = Entry;
            }
        }
    }
}
