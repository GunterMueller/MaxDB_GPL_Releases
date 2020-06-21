/*!
  @file           SAPDBFields_FieldCollection.hpp
  @author         TorstenS
  @ingroup        Field Container classes
  @brief          Declarations for class SAPDBFields_FieldCollection

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


#ifndef SAPDBFields_FieldCollection_HPP
#define SAPDBFields_FieldCollection_HPP


#include "Container/Container_Vector.hpp"

/*!
@class  SAPDBFields_FieldCollection
@brief  This class implements a collection of fields which are
        located in a related memory chunk.
*/

class SAPDBFields_FieldCollection
{
public:
     
    /// The range for the number of elements in field Containers
    typedef SAPDB_UInt IndexType;
    /// type for specifying sizes
    typedef SAPDB_UInt  SizeType;

public:

    /*!
       @brief  Constructs a field collection
       @param  allocator [in|out] allocator to get the memory to store
                                  the field handler not the fields itself.
    */

    SAPDBFields_FieldCollection( SAPDBMem_IRawAllocator &allocator )
    : m_Fields( allocator )
    {}

    /*!
        @brief  Destruct a field collection and return the memory
    */

    ~SAPDBFields_FieldCollection(){
        m_Fields.Delete();
    }
    
    /*!
        @brief  Initialize field collection, i.e. allocate memory.
        @param  maxFields [in] maximum number of fields in the collection
        @return (bool) true if successful
    */

    bool Initialize( const IndexType  maxFields ){
        return m_Fields.Reserve( maxFields );
    }

    /*!
        @brief  Returns the number of fields stored in the field collection.
        @return (IndexType) count
    */
    IndexType GetFieldCount() const{
        return m_Fields.GetSize();
    }
 
    /*!
        @brief  Returns the maximum number of fields storable in 
                the field collection.
        @return (IndexType) count
    */
    IndexType GetMaxFieldCount() const{
        return m_Fields.GetCapacity();
    }
    /*!
        @brief  Returns the length for the given fieldNo in bytes. 
                If the fieldNo does not exist zero is returned.
        @param  fieldNo [in] index of the field [0...GetFieldCount()-1]
        @return (SizeType) length
    */
    SizeType GetFieldLength( const IndexType fieldNo ) const{
        return m_Fields[ fieldNo ].GetLength();
    }

    /*!
        @brief  Returns the sum of the length of all fields in the 
                field collection in bytes.
        @param  fieldNo [in] The index of the field [0...GetFieldCount()-1]
        @return (SizeType) length
    */
    SizeType GetTotalLength() const
    {
        IndexType fieldNo;
        SizeType  length = 0;

        for( fieldNo = 0; fieldNo < GetFieldCount(); ++fieldNo ){
            length += this->GetFieldLength( fieldNo );
        }
        return length;
    }

    /*!
       @brief  Returns the &lt;N>-th field within the field collection.
       @param  fieldNo [in]  index of the field [0...GetFieldCount()-1]
       @param  field   [out] requested field 
       @return (bool) true if the field is available
    */
    bool GetField(
        const IndexType     fieldNo,
        SAPDBFields_Field   &field ) const
    {
        if( fieldNo >= GetFieldCount()){
           return false;
        }
        field = m_Fields[ fieldNo ];
        return true;
    }
 
    /*!
       @brief  Add a field with the given index to the field collection.
       @param  fieldNo [in] index of the field [0...GetFieldCount()-1]
       @param  field   [in] given field 
       @return (bool) true if field could be stored
    */
    bool Add(
        const IndexType         fieldNo,
        const SAPDBFields_Field &field )
    {
        SAPDBERR_ASSERT_ARGUMENT( fieldNo < m_Fields.GetCapacity());

        if( fieldNo >= m_Fields.GetSize()){
            if( ! m_Fields.Resize( fieldNo + 1)){
                return false; // could not happen, cause memory is still reserved 
            }
        }
        m_Fields[ fieldNo ] = field;
        return true;
    }

private:

    // Assignment operator is not supported
    SAPDBFields_FieldCollection& operator=( const SAPDBFields_FieldCollection& fc );

     // Copy constructor is not supported
    SAPDBFields_FieldCollection( const SAPDBFields_FieldCollection& fc );

private:

    // The content of the field collection
    Container_Vector< SAPDBFields_Field > m_Fields;
};


#endif // SAPDBFields_FieldCollection_HPP

