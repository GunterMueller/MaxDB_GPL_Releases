/*!
  @file           SAPDBFields_AttributedField.hpp
  @author         UweH
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



#ifndef SAPDBFIELDS_FIELD_ATTRIBUTED_HPP
#define SAPDBFIELDS_FIELD_ATTRIBUTED_HPP

#include "SAPDBCommon/Fields/SAPDBFields_Field.hpp"

/*!
@class SAPDBFields_AttributedField
@brief A handler to a contiguous memory block of certain size.

The memory block is fixed in content and size, that means 
SAPDBFields_Field has only methods for reading the content and no 
methods for changing the content.
*/

class SAPDBFields_AttributedField : public SAPDBFields_Field
{
public:

    /// Special field content
    enum Attribute
    {
        ValueAttribute,       ///< pointer and length is used and greater 0
        NullAttribute,        ///< pointer and length is 0
        DefaultAttribute,     ///< pointer and length is 0, the value must come from outside
        BlobAttribute,        ///< pointer and length is 0
        SpecialNullAttribute, ///< pointer and length is 0
        IgnoreAttribute,      ///< pointer and length is 0
        UnusedAttribute,      ///< This must not be used and indicates always an error.
        Attribute_MAX
    };
    /// for output of the Attribute
    static const char *AttributeToString[Attribute_MAX];
    
	/// Constructs a special attributed field. default is NULL
    SAPDBFields_AttributedField(Attribute state = DefaultAttribute)
    : SAPDBFields_Field(0,0),
      m_Attribute      (state)
    {
        if( ! CheckAttribute() )
            CheckAttributeFailed(__FILE__,__LINE__);
    }
    /*!
    @brief          Constructs a field pointing to Src with length Len.
    @param          source [in] The address of the memory block
    @param          length [in] The length of the memory block in sizeof(SAPDB_Byte)
    */
    SAPDBFields_AttributedField ( void        *source,
                                  SizeType     length,
                                  Attribute    attribute = ValueAttribute)
      : SAPDBFields_Field(reinterpret_cast<Pointer>(source),length),
        m_Attribute      (attribute)
    {
        if( ! CheckAttribute() )
            CheckAttributeFailed(__FILE__,__LINE__);
    }
    /*!
    @brief Assigns values for the memory block pointer and length
    @param          source [in] Pointer to memory block
    @param          length [in] Length of memory block
    */
    void Assign( void     *source,
                 SizeType  length)
    {
        SAPDBFields_Field::Assign(reinterpret_cast<Pointer>(source),length);
        m_Attribute = ValueAttribute;
    }
	/// Deassigns a Field
    void Deassign()
    {
        SAPDBFields_Field::Deassign();
        m_Attribute = ValueAttribute;
	}
	/*!
    @brief  Assigns an empty field.
    The field is simply assigned to the this-pointer with length zero.
    */
    void AssignEmpty() 
    {
        Assign(this, 0);
	}
    /// returns true if given field is bytewise equal to current field.
    bool IsEqualTo(const SAPDBFields_AttributedField& Field) const
    {
        return SAPDBFields_Field::IsEqualTo(Field)
               &&
               m_Attribute == Field.m_Attribute;
    }
	/// Checks, if two fields point to the same memory block
	bool operator ==(const SAPDBFields_AttributedField& Field) const
    {
        return SAPDBFields_Field::operator == (Field)
               &&
               m_Attribute == Field.m_Attribute;
    }
	/// Checks the opposite of ==
    bool operator !=(const SAPDBFields_AttributedField& Field) const
    {
        return ! (*this == Field);
    }
    /*!
    @brief Copies  the contents from given field to current field.
    @param field   other field to copy from
    @param errlist in case of an error this is filled
    @return false on any error
    
    If the given Field points to the same memory nothing is done and true returned.
    */
    bool Copy (const SAPDBFields_AttributedField &field,
               SAPDBErr_MessageList              &errlist)
    {
        if(!SAPDBFields_Field::Copy(field,errlist))
            return false;
        m_Attribute   = field.m_Attribute;
        return true;
    }
    /*!
    @brief  Copies  the contents from given field to current field.
    @param  field   other field to copy from
    @return false on any error
    
    If the given Field points to the same memory nothing is done and true returned.
    If the length of both fields is different nothing is copied and false is returned.
    */
    bool Copy (const SAPDBFields_AttributedField &field)
    {
        if(!SAPDBFields_Field::Copy(field))
            return false;
        m_Attribute   = field.m_Attribute;
        return true;
    }
    /// returns the attribute assigned
    Attribute GetAttribute() const
    {
        return Attribute(m_Attribute);
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

    /// This checks the current set attribute value against the given field values
	bool CheckAttribute()
    {
        switch (m_Attribute)
        {
            case ValueAttribute:
            case BlobAttribute:
                return m_DataPtr!=0 && m_DataLen>=0;
            case NullAttribute:
            case DefaultAttribute:
            case SpecialNullAttribute:
            case IgnoreAttribute:
                return m_DataPtr==0 && m_DataLen==0;
        }
        return false;
    }
    /// Abort
    void CheckAttributeFailed (const SAPDB_Char *file,
                               SAPDB_UInt2       line);

protected:

    SAPDB_UInt1 m_Attribute; ///< 1 compressed & persistent form of Attribute
    SAPDB_UInt1 m_Filler1;   ///< 1 not used
    SAPDB_UInt2 m_Filler2;   ///< 2 not used
};

#endif // SAPDBFIELDS_FIELD_ATTRIBUTED_HPP

