/*!
  @file           SAPDBFields_Field.hpp
  @author         StefanB, UweH
  @ingroup        Field
  @brief          class declarations for Field

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



#ifndef SAPDBFIELDS_FIELD_HPP
#define SAPDBFIELDS_FIELD_HPP

#include "SAPDBCommon/SAPDB_Types.hpp"
#include "SAPDBCommon/SAPDB_RangeCode.hpp"
/*!
@class SAPDBFields_Field
@brief A handler to a contiguous memory block of certain size.

The memory block is fixed in content and size, that means 
SAPDBFields_Field has only methods for reading the content and no 
methods for changing the content.
*/
class SAPDBFields_Field
{
public:
	/// The range for field lengths
    typedef SAPDB_UInt                  SizeType;
	///The range for positions within fields
	typedef SizeType                    IndexType;
    /// Pointer to field data
	typedef SAPDB_Byte*                 Pointer;
    /// const Pointer to field data
	typedef const SAPDB_Byte*           ConstPointer;

public:
	/// Constructs an empty field
	SAPDBFields_Field()
    : m_DataPtr(0),
      m_DataLen(0)
	{}
    /*!
    @brief          Constructs a field pointing to Src with length Len.
    @param          Src [in] The address of the memory block
    @param          Len [in] The length of the memory block in sizeof(SAPDB_Byte)
    */
    SAPDBFields_Field(void *   source,
                      SizeType length) 
      : m_DataPtr(reinterpret_cast<Pointer>(source)), m_DataLen(length) {}
    /*!
    @brief Assigns values for the memory block pointer and length
    @param          Src [in] Pointer to memory block
    @param          Len [in] Length of memory block
    */
    void Assign(void     *source,
                SizeType  length)
    {
        m_DataPtr = reinterpret_cast<Pointer>(source);
        m_DataLen = length;
    }
    /*!
    @brief Assigns values for the memory block pointer.
    @param          Field [in] Another field
    */
    void Assign(const SAPDBFields_Field& Field)
    {
        m_DataPtr = Field.m_DataPtr;
        m_DataLen = Field.m_DataLen;
    }
	/// Deassigns a Field
    void Deassign()
    {
        m_DataPtr = 0;
        m_DataLen = 0;
	}
    /*!
    @brief   Indicates, if the instance is already assigned to a memory block.
    @return  true if data ptr is assigned
    */
    bool IsAssigned() const
    {
        return m_DataPtr != 0;
	}
	/// Returns the length of the memory block in sizeof(SAPDB_Byte)
    SizeType GetLength() const
    {
        return m_DataLen;
    }
    /// returns true if given field is bytewise equal to current field.
    bool IsEqualTo(const SAPDBFields_Field& Field) const
    {
        return m_DataLen == Field.m_DataLen
               && 
               IsAssigned() && Field.IsAssigned()
               &&
               ! memcmp(m_DataPtr, Field.m_DataPtr, GetLength());
    }
	/// Checks, if two fields point to the same memory block
	bool operator ==(const SAPDBFields_Field& Field) const
    {
        return m_DataPtr == Field.m_DataPtr
               &&
               m_DataLen == Field.m_DataLen;
    }
    /// inverse operation of ==
    bool operator !=(const SAPDBFields_Field& Field) const
    {
        return ! (*this == Field);
    }
	/// Returns the address of the constant memory block
    const Pointer GetDataPtr() const
    {
        return m_DataPtr;
    }
	/// Returns the address of the memory block
    Pointer GetDataPtr()
    {
        return m_DataPtr;
    }
    /*!
    @brief Returns the address of a certain position within the memory block
    @param          offset [in] The position within the memory block
    @param          length [in] The length which is required (optional)
    @return         const Pointer

	The field must be not empty.
    */
    const Pointer GetPointer( IndexType offset,
                              SizeType  length = 0) const;
    /// the non constant variant
    Pointer GetPointer( IndexType offset,
                        SizeType  length = 0);
    /*!
    @brief Returns the the byte from the given offset position.
    @param          offset [in] The position within the memory block
    @return         the byte

	The field must be not empty.
    */
    SAPDB_Byte GetByteAt ( IndexType offset ) const
    {
        return *(GetPointer(offset,sizeof(SAPDB_Byte)));
    }
	/// Returns an iterator which points to the begin of the memory block
    const ConstPointer Begin() const
    {
        return m_DataPtr;
    }
	/// Returns an iterator which points behind the end of the memory block.
    const ConstPointer End() const
    {
        return m_DataPtr + m_DataLen;
    }
    /*!
    @brief Copies  the contents from given field to current field.
    @param field   other field to copy from
    @param errlist in case of an error this is filled
    @return false on any error
    
    If the given Field points to the same memory nothing is done and true returned.
    */
    bool Copy (const SAPDBFields_Field &field,
               SAPDBErr_MessageList    &errlist)
    {
        if ( *this != field )
        {
            if ( ! SAPDB_MemMove( m_DataPtr, field.GetDataPtr(), field.m_DataLen,
                                  "SAPDBFields_Field::Copy", __CONTEXT__, errlist ) )
                return false;
            m_DataLen = field.m_DataLen;
        }
        return true;
    }
    /*!
    @brief  Copies  the contents from given field to current field.
    @param  field   other field to copy from
    @return false on any error
    
    If the given Field points to the same memory nothing is done and true returned.
    If the length of both fields is different nothing is copied and false is returned.
    */
    bool Copy (const SAPDBFields_Field &field)
    {
        if ( *this != field )
        {
            if ( m_DataLen != field.m_DataLen )
                return false;
            SAPDB_MemMoveNoCheck(m_DataPtr, field.GetDataPtr(), field.m_DataLen);
            m_DataLen = field.m_DataLen;
        }
        return true;
    }
    /*!
    @brief writes contents to trace
    @param title may be given for better information
    */
    void WriteToTrace(const SAPDB_Char *title = 0) const;
    /*!
    @brief context info is appended to errlist
    @param errlist this is extended
    @param title may be given for better information
    */
    void AppendContextInfo(SAPDBErr_MessageList &errlist,
                           const SAPDB_Char     *filename,
                           const SAPDB_UInt      linenumber,
                           const SAPDB_Char     *title = 0) const;

protected:

    /// Pointer to memory block
	Pointer   m_DataPtr;
	/// Count of elements of SAPDB_Byte in memory block
    SizeType  m_DataLen;
};
#endif // SAPDBFIELDS_FIELD_HPP

