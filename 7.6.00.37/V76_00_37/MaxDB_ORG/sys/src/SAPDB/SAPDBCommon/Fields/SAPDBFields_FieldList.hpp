/*!
  @file           SAPDBFields_FieldList.hpp
  @author         UweH
  @ingroup        Field
  @brief          Declarations for class SAPDBFields_FieldList.

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
#ifndef SAPDBFields_FieldList_HPP
#define SAPDBFields_FieldList_HPP

#include "SAPDBCommon/Fields/SAPDBFields_IFieldSequences.hpp"
/*!
@class SAPDBFields_FieldList
@brief A handler to a contiguous memory block of certain size and with certain structure.

The structure consists of a list of offsets followed by a list of fields.
Each offset gives the position of the corresponding field.
The last offset points behind the last field.
*/
class SAPDBFields_FieldList : public SAPDBFields_IFieldSequence
{
public:
    /*!
    @brief Type definition of type IndexType.

	Both base classes provide a type IndexType.
    The following type definition forces this class to use the 
    type IndexType from base class 'SADBFields_IFieldSequence'.
    */
    typedef SAPDBFields_IFieldSequence::IndexType      IndexType;
    /*!
    @brief type for specifying sizes

	Both base classes provide a type IndexType.
    The following type definition forces this class to use the 
    type IndexType from base class 'SADBFields_IFieldSequence'.
    */
    typedef SAPDBFields_IFieldSequence::SizeType       SizeType;
    /// result Of GetField()
    typedef SAPDBFields_IFieldSequence::GetFieldResult GetFieldResult;

public:
	/// Constructs sequence that is not assigned.
    SAPDBFields_FieldList()
    : m_FieldCount      (0),
      m_FirstFreeOffset (0)
    {}
    /// returns true if a persistent format exists
    bool IsAssigned() const
    {
        return m_Buffer.IsAssigned();
    }
    /// Deassign internal buffer
    void Deassign()
    {
        m_Buffer.Deassign();
    	m_FieldCount      = 0;
        m_FirstFreeOffset = 0;
    }
    /// the given field is assigned with the persistent representation
    const SAPDB_Byte* GetConstDataPtr() const
    {
        return m_Buffer.GetDataPtr();
    }
    /// the non const version
    SAPDB_Byte* GetDataPtr()
    {
        return m_Buffer.GetDataPtr();
    }
    /*!
    @brief  Assigns a new buffer which is used to build a new fieldlist
    @param  source [in]  The address of the memory block
    @param  length [in]  The length of the memory block
    */
    void AssignNewBuffer (SAPDB_Byte           *source,
                          SizeType              length)
	{
        m_Buffer.Assign(source,length);
    	m_FieldCount      = 0;
        m_FirstFreeOffset = 0;
    }
    /*!
    @param field [in] the new field which should be appended to the fieldlist
    @param errlist [out] error description in case of an error
    @return false if any error occured, normally the the size of the give space for the
            fieldlist will be too small to add another field
    */
    bool AppendField ( const SAPDBFields_AttributedField &field,
                       SAPDBErr_MessageList              &errlist );

    /*!
    @brief  Assigns a new buffer which is interpreted as a fieldlist
    @param  source [in]  The address of the memory block
    @param  length [in]  The length of the memory block
    @param  errlist [out] error description in case of an error
    @param  firstFreeOffset [in] optional
    @return false if any error occured
    
    If firstFreeOffset is given (>0) then this is used as the end of interpretation.
    */
    bool Assign (SAPDB_Byte           *source,
                 SizeType              length,
                 SAPDBErr_MessageList &errlist,
                 SizeType              firstFreeOffset = 0);
    /// This function is for testing the class
    static bool Test (SAPDBErr_MessageList &errlist);
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

public: // interface of SAPDBFields_IFieldSequence

    virtual IndexType      GetFieldCount  () const;
    virtual GetFieldResult GetField       ( IndexType                    fieldno,
                                            SAPDBFields_AttributedField &field,
                                            SAPDBErr_MessageList        &errlist ) const;
    virtual SizeType       GetFieldLength ( IndexType                    index ) const;
    virtual SizeType       GetTotalLength () const;

protected:

    /// the used buffer
    SAPDBFields_Field m_Buffer;
    /// the numbers of fields in the list
    SAPDB_UInt2       m_FieldCount;
    /// the first free offset where a new field may be appended
    SAPDB_UInt2       m_FirstFreeOffset;
};
#endif // SAPDBFields_FieldList_HPP

