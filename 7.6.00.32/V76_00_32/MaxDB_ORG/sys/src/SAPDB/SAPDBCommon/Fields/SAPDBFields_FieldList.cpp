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
#include "SAPDBCommon/Fields/SAPDBFields_FieldList.hpp"
#include "SAPDBCommon/Fields/SAPDBFields_Exceptions.hpp"
#include "SAPDBCommon/SAPDB_MemCopyMove.hpp"
#include "KernelCommon/Kernel_VTrace.hpp"
//--------------------------------------------------------------------------------------------
/// the maximum length of the persistent format is three
#define MAX_PERSISTENT_FIELD_LENGTH 3
//--------------------------------------------------------------------------------------------
class LengthField
{
    public:
    /// ctor with usage of given buffer
    LengthField(SAPDB_Byte                  *source,
                SAPDBFields_Field::SizeType  length)
    : m_Field(source,length)
    {}
    /// returns the used bytes for the persistent length information
    SAPDB_UInt2 GetPersistentLength()
    {
        SAPDB_Byte firstByte = m_Field.GetByteAt(0);
        return firstByte < 255 ? 1 : 3;
    }
    /// return true if the persistent length could be written
    bool IsAssigned()
    {
        return m_Field.IsAssigned();
    }
    /// the length is returned, bytes must have a length of 1or3
    SAPDB_UInt2 GetLength()
    {
        /// The routines used here include many checks
        /// m_Field.GetDataPtr() + offset would be faster
        SAPDB_Byte firstByte = m_Field.GetByteAt(0);
        if ( firstByte < 246 )
            return firstByte-1;
        if ( firstByte == 255 )
        {
            SAPDB_UInt2 length1 = m_Field.GetByteAt(1);
            SAPDB_UInt2 length2 = m_Field.GetByteAt(2);
            length1 = (length1 << 8) | length2;
            return length1-3;
        }
        return 1; // special attributed value
    }
    /// the coded attribute is returned, bytes must have a length of 1or3
    SAPDBFields_AttributedField::Attribute GetAttribute()
    {
        SAPDB_Byte firstByte = m_Field.GetByteAt(0);
        if ( firstByte < 246 || firstByte == 255 )
            return SAPDBFields_AttributedField::ValueAttribute;
        switch (firstByte)
        {
            case 254 : return SAPDBFields_AttributedField::NullAttribute;
            case 253 : return SAPDBFields_AttributedField::DefaultAttribute;
            case 252 : return SAPDBFields_AttributedField::BlobAttribute;
            case 251 : return SAPDBFields_AttributedField::SpecialNullAttribute;
            case 250 : return SAPDBFields_AttributedField::IgnoreAttribute;
            default  : return SAPDBFields_AttributedField::UnusedAttribute;
        }
    }
    /// This is used to convert a new length and attribute information into the persistent format
    bool WriteLengthAndAttribute ( SAPDB_UInt2                             length,
                                   SAPDBFields_AttributedField::Attribute  attribute,
                                   SAPDBErr_MessageList                   &errlist )
    {
        if ( ! IsAssigned() )
        {
            errlist = Fields_Exception(__CONTEXT__,LENGTHFIELD_NOT_ASSIGNED);
            return false;
        }
        
        SAPDB_Byte persistentAttribute;
        
        switch (attribute)
        {
            case SAPDBFields_AttributedField::ValueAttribute:
                persistentAttribute = length+1 < 246 ? length+1 : 255;
                break;
            case SAPDBFields_AttributedField::NullAttribute:
                persistentAttribute = 254;
                break;
            case SAPDBFields_AttributedField::DefaultAttribute:
                persistentAttribute = 253;
                break;
            case SAPDBFields_AttributedField::BlobAttribute:
                persistentAttribute = 252;
                break;
            case SAPDBFields_AttributedField::SpecialNullAttribute:
                persistentAttribute = 251;
                break;
            case SAPDBFields_AttributedField::IgnoreAttribute:
                persistentAttribute = 250;
                break;
            default:
                errlist = Fields_Exception(__CONTEXT__,LENGTHFIELD_INVALID_ATTRIBUTE,
                                           SAPDBFields_AttributedField::AttributeToString[attribute]);
                return false;
        }
        *(m_Field.GetDataPtr()) = persistentAttribute;
        if ( persistentAttribute < 255 )
            return true;
        *(m_Field.GetDataPtr()+1) = (length+3 & 0xFF00)>>8;
        *(m_Field.GetDataPtr()+2) = (length+3 & 0x00FF);
        return true;
    }
    private:
    /// the internal used field for writing and interpreting the persistent length
    SAPDBFields_AttributedField m_Field;
};
//--------------------------------------------------------------------------------------------
bool SAPDBFields_FieldList::Assign (SAPDB_Byte           *source,
                                    SizeType              length,
                                    SAPDBErr_MessageList &errlist,
                                    SizeType              firstFreeOffset)
{
    m_Buffer.Assign(source,length);
   	m_FieldCount      = 0;
    m_FirstFreeOffset = firstFreeOffset != 0 ? firstFreeOffset : m_Buffer.GetLength();
    
    SizeType currentOffset = 0;
    
    while ( currentOffset < m_FirstFreeOffset )
    {
        LengthField lengthField ( m_Buffer.GetDataPtr() + currentOffset,
                                  m_Buffer.GetLength()  - currentOffset);
        
        if ( currentOffset + lengthField.GetPersistentLength()
             >
             m_Buffer.GetLength()
             ||
             currentOffset + lengthField.GetPersistentLength() + lengthField.GetLength()
             >
             m_Buffer.GetLength() )
        {
            errlist = Fields_Exception (__CONTEXT__, FIELDLIST_IS_CORRUPTED,
                                        SAPDB_ToString(m_FieldCount),
                                        SAPDB_ToString(currentOffset),
                                        SAPDB_ToString(lengthField.GetPersistentLength()),
                                        SAPDB_ToString(lengthField.GetLength()));
            AppendContextInfo(errlist,__CONTEXT__,"SAPDBFields_FieldList::Assign error");
            return false;
        }
        ++m_FieldCount;
        currentOffset = currentOffset + lengthField.GetPersistentLength() + lengthField.GetLength();
    }
    if ( currentOffset != m_FirstFreeOffset )
    {
        AppendContextInfo(errlist,__CONTEXT__,"SAPDBFields_FieldList::Assign error");
        return false;
    }
    return true;
}
//--------------------------------------------------------------------------------------------
bool SAPDBFields_FieldList::AppendField ( const SAPDBFields_AttributedField &field,
                                          SAPDBErr_MessageList              &errlist )
{
    if ( ! field.IsAssigned() )
    {
        errlist.ClearMessageList();
        field.AppendContextInfo(errlist,__CONTEXT__,"SAPDBFields_FieldList::AppendField given field not assigned");
        AppendContextInfo(errlist,__CONTEXT__,"SAPDBFields_FieldList::AppendField given field not assigned");
        return false;
    }
    // Use the LengthField handler to convert length and attribute of the given field
    // to the reserved space.
    SAPDB_Byte  *pLengthBuffer =
        m_Buffer.GetPointer(m_FirstFreeOffset, MAX_PERSISTENT_FIELD_LENGTH);
    LengthField  lengthField (pLengthBuffer, MAX_PERSISTENT_FIELD_LENGTH);
    // do conversion
    if ( lengthField.GetPersistentLength() + field.GetLength()
         >
         m_Buffer.GetLength() - m_FirstFreeOffset )
	{
        errlist.ClearMessageList();
        field.AppendContextInfo(errlist,__CONTEXT__,"SAPDBFields_FieldList::AppendField too small");
        AppendContextInfo(errlist,__CONTEXT__,"SAPDBFields_FieldList::AppendField too small");
        return false;
	}
    if ( ! lengthField.WriteLengthAndAttribute(field.GetLength(), field.GetAttribute(), errlist) )
        return false;
    // set firstFree to append given field body    
    m_FirstFreeOffset += lengthField.GetPersistentLength();
    
    if ( ! SAPDB_MemCopy( m_Buffer.GetPointer(m_FirstFreeOffset, field.GetLength()),
                          field.GetDataPtr(), field.GetLength(),
                          "Fields", __FILE__, __LINE__,
                          errlist ) )
        return false;
    m_FirstFreeOffset += field.GetLength();
    ++m_FieldCount;
    return true;
}

//--------------------------------------------------------------------------------------------
SAPDBFields_FieldList::IndexType SAPDBFields_FieldList::GetFieldCount() const
{
	return m_FieldCount;
}
//--------------------------------------------------------------------------------------------
SAPDBFields_FieldList::GetFieldResult
SAPDBFields_FieldList::GetField ( IndexType                    fieldno,
                                  SAPDBFields_AttributedField &field,
                                  SAPDBErr_MessageList        &errlist) const
{
    if ( ! IsAssigned() )
    {
        errlist = Fields_Exception(__CONTEXT__,FIELDLIST_IS_NOT_ASSIGNED);
        AppendContextInfo(errlist,__CONTEXT__,"SAPDBFields_FieldList::GetField");
        return fieldError;
    }
    if ( fieldno > m_FieldCount )
    {
        errlist = Fields_Exception(__CONTEXT__,FIELD_INVALID_FIELDNO,
                                   SAPDB_ToString(fieldno),
                                   SAPDB_ToString(m_FieldCount));
        AppendContextInfo(errlist,__CONTEXT__,"SAPDBFields_FieldList::GetField");
        return fieldError;
    }

    IndexType currentFieldno = 1;
    SizeType  currentOffset  = 0;
    
    while ( currentOffset < m_Buffer.GetLength() )
    {
        LengthField lengthField ( m_Buffer.GetDataPtr() + currentOffset,
                                  m_Buffer.GetLength()  - currentOffset);
        
        if ( currentOffset + lengthField.GetPersistentLength()
             >
             m_Buffer.GetLength()
             ||
             currentOffset + lengthField.GetPersistentLength() + lengthField.GetLength()
             >
             m_Buffer.GetLength() )
        {
            errlist = Fields_Exception (__CONTEXT__, FIELDLIST_IS_CORRUPTED,
                                        SAPDB_ToString(currentFieldno),
                                        SAPDB_ToString(currentOffset),
                                        SAPDB_ToString(lengthField.GetPersistentLength()),
                                        SAPDB_ToString(lengthField.GetLength()));
            AppendContextInfo(errlist,__CONTEXT__,"SAPDBFields_FieldList::GetField error");
            return fieldError;
        }
        if ( currentFieldno == fieldno )
        {
            field.Assign( m_Buffer.GetDataPtr() + currentOffset + lengthField.GetPersistentLength(),
                          lengthField.GetLength() );
            return fieldFound;
        }
        ++currentFieldno;
        currentOffset = currentOffset + lengthField.GetPersistentLength() + lengthField.GetLength();
    }
    errlist = Fields_Exception (__CONTEXT__, FIELDLIST_HANDLE_IS_CORRUPT,
                                SAPDB_ToString(fieldno),
                                SAPDB_ToString(m_FieldCount));
    AppendContextInfo(errlist,__CONTEXT__,"SAPDBFields_FieldList::GetField");
    return fieldError;
}
//--------------------------------------------------------------------------------------------
SAPDBFields_FieldList::SizeType
SAPDBFields_FieldList::GetFieldLength(IndexType index) const
{
    SAPDBFields_AttributedField field;
    SAPDBErr_MessageList        errlist;
    
    if ( GetField (index,field,errlist) != fieldFound )
        RTE_Crash (errlist);
        
    return field.GetLength();
}
//--------------------------------------------------------------------------------------------
SAPDBFields_FieldList::SizeType
SAPDBFields_FieldList::GetTotalLength() const
{
	return m_FirstFreeOffset;
}
//--------------------------------------------------------------------------------------------
void SAPDBFields_FieldList::WriteToTrace(const SAPDB_Char *title) const
{
    m_Buffer.WriteToTrace((title==0?"Fieldlist":title));
    Kernel_VTrace() << "FieldCount: " << m_FieldCount << ", FirstFreeOffset: " << m_FirstFreeOffset;
}
//--------------------------------------------------------------------------------------------
void SAPDBFields_FieldList::AppendContextInfo(SAPDBErr_MessageList &errlist,
                                              const SAPDB_Char     *filename,
                                              const SAPDB_UInt      linenumber,
                                              const SAPDB_Char     *title) const
{
    errlist = errlist + Fields_Exception(filename,linenumber,FIELDLIST_CONTEXT_INFO,
                                         (title==0?"Fieldlist":title),
                                         SAPDB_ToString(m_FieldCount),
                                         SAPDB_ToString(m_FirstFreeOffset));
    m_Buffer.AppendContextInfo(errlist,filename,linenumber,(title==0?"Fieldlist":title));
}
//--------------------------------------------------------------------------------------------
bool SAPDBFields_FieldList::Test (SAPDBErr_MessageList &errlist)
{
    const SAPDB_UInt                  FieldListLength = 350;
    const SAPDB_UInt                  MaxFieldLength  = 300;
          SAPDB_Byte                  persistentFields [FieldListLength];
          SAPDB_Byte                  persistentField  [MaxFieldLength];
          SAPDBFields_AttributedField field;
          SAPDBFields_FieldList       fields;
          SAPDB_UInt                  fieldListLength = 0;

    if ( ! SAPDB_MemFillNoCheck(persistentFields,0x00,FieldListLength) )
    {
        fields.WriteToTrace("SAPDB_MemFillNoCheck failed");
        errlist = Fields_Exception(__CONTEXT__,FIELDLIST_TEST_FAILED,
                                   "SAPDB_MemFillNoCheck failed");
        return false;
    }
    if ( fields.IsAssigned() )
    {
        fields.WriteToTrace("fields must not be assigned after creation");
        errlist = Fields_Exception(__CONTEXT__,FIELDLIST_TEST_FAILED,
                                   "fields must not be assigned after creation");
        return false;
    }
    fields.AssignNewBuffer (persistentFields,FieldListLength);
    if ( ! fields.IsAssigned() )
    {
        fields.WriteToTrace("fields must be assigned after AssignNewBuffer()");
        errlist = Fields_Exception(__CONTEXT__,FIELDLIST_TEST_FAILED,
                                   "fields must be assigned after AssignNewBuffer()");
        return false;
    }
    if ( fields.GetFieldCount() > 0 )
    {
        fields.WriteToTrace("Before appending any new fields GetFieldCount() must return 0");
        errlist = Fields_Exception(__CONTEXT__,FIELDLIST_TEST_FAILED,
                                   "Before appending any new fields GetFieldCount() must return 0");
        return false;
    }
    if ( fields.GetField (1, field, errlist) != SAPDBFields_IFieldSequence::fieldError )
    {
        fields.WriteToTrace("GetField() must return fieldError because it is not assigned");
        errlist = errlist + Fields_Exception(__CONTEXT__,FIELDLIST_TEST_FAILED,
                                            "GetField() must return fieldError because it is not assigned");
        return false;
    }
    errlist.ClearMessageList();
    if ( field.IsAssigned() )
    {
        fields.WriteToTrace("field from empty fieldlist must not be assigned");
        errlist = Fields_Exception(__CONTEXT__,FIELDLIST_TEST_FAILED,
                                   "field from empty fieldlist must not be assigned");
        return false;
    }
    if ( fields.GetTotalLength() > 0 )
    {
        fields.WriteToTrace("emtpy fieldlist must not return a totallength > 0");
        errlist = Fields_Exception(__CONTEXT__,FIELDLIST_TEST_FAILED,
                                   "emtpy fieldlist must not return a totallength > 0");
        return false;
    }
    field.Assign(persistentField,10);
    if ( ! SAPDB_MemFillNoCheck(persistentField,0xAA,10) )
    {
        fields.WriteToTrace("SAPDB_MemFillNoCheck failed");
        errlist = Fields_Exception(__CONTEXT__,FIELDLIST_TEST_FAILED,
                                   "SAPDB_MemFillNoCheck failed");
        return false;
    }
    if ( ! field.IsAssigned() )
    {
        fields.WriteToTrace("field must be assigned");
        field.WriteToTrace("field must be assigned");
        errlist = Fields_Exception(__CONTEXT__,FIELDLIST_TEST_FAILED,
                                   "field must be assigned");
        return false;
    }
    if ( ! fields.AppendField (field,errlist) )
    {
        fields.WriteToTrace("fields.AppendField failed");
        field.WriteToTrace("fields.AppendField failed");
        errlist = Fields_Exception(__CONTEXT__,FIELDLIST_TEST_FAILED,
                                   "fields.AppendField failed");
        return false;
    }
	fields.WriteToTrace("fieldlist after append of one field");
    if ( fields.GetFieldLength(1) != 10 )
    {
        fields.WriteToTrace("invalid fieldlength for field 1 returned, 10 is expected");
        errlist = Fields_Exception(__CONTEXT__,FIELDLIST_TEST_FAILED2,
                                   "invalid fieldlength for field 1 returned, 10 is expected",
                                   SAPDB_ToString(fields.GetFieldLength(1)));
        return false;
    }
    fieldListLength = fields.GetTotalLength();
    if ( fieldListLength > 11 )
    {
        fields.WriteToTrace("total length of 11 is expected but found was");
        errlist = Fields_Exception(__CONTEXT__,FIELDLIST_TEST_FAILED2,
                                   "total length of 11 is expected but found was",
                                   SAPDB_ToString(fieldListLength));
        return false;
    }
    field.Deassign();
    if ( fields.GetField (1, field, errlist) != SAPDBFields_IFieldSequence::fieldFound )
    {
        fields.WriteToTrace("fields, GetField() must return fieldFound");
        field.WriteToTrace("field GetField() must return fieldFound");
        errlist = errlist + Fields_Exception(__CONTEXT__,FIELDLIST_TEST_FAILED,
                                            "GetField() must return fieldFound");
        return false;
    }
    if ( ! field.IsAssigned() )
    {
        fields.WriteToTrace("field must be assigned");
        field.WriteToTrace("field must be assigned");
        errlist = Fields_Exception(__CONTEXT__,FIELDLIST_TEST_FAILED,
                                   "field must be assigned");
        return false;
    }
    if ( field.GetLength() != 10 )
    {
        fields.WriteToTrace("field must have length 10");
        field.WriteToTrace("field must have length 10");
        errlist = Fields_Exception(__CONTEXT__,FIELDLIST_TEST_FAILED,
                                   "field must have length 10");
        return false;
    }
    fields.Deassign();
    if ( ! fields.Assign (persistentFields,FieldListLength,errlist,fieldListLength) )
    {
        fields.WriteToTrace("fields.Assign failed");
        errlist = Fields_Exception(__CONTEXT__,FIELDLIST_TEST_FAILED,
                                   "fields.Assign failed");
        return false;
    }
    field.Deassign();
    if ( fields.GetField (1, field, errlist) != SAPDBFields_IFieldSequence::fieldFound )
    {
        fields.WriteToTrace("fields, GetField() must return fieldFound");
        field.WriteToTrace("field GetField() must return fieldFound");
        errlist = errlist + Fields_Exception(__CONTEXT__,FIELDLIST_TEST_FAILED,
                                            "fields, GetField() must return fieldFound");
        return false;
    }
    if ( ! field.IsAssigned() )
    {
        fields.WriteToTrace("field must be assigned");
        field.WriteToTrace("field must be assigned");
        errlist = Fields_Exception(__CONTEXT__,FIELDLIST_TEST_FAILED,
                                   "field must be assigned");
        return false;
    }
    if ( field.GetLength() != 10 )
    {
        fields.WriteToTrace("field must have length 10");
        field.WriteToTrace("field must have length 10");
        errlist = Fields_Exception(__CONTEXT__,FIELDLIST_TEST_FAILED,
                                   "field must have length 10");
        return false;
    }
    if ( ! SAPDB_MemFillNoCheck(persistentField,0xBB,5) )
    {
        fields.WriteToTrace("SAPDB_MemFillNoCheck failed");
        errlist = Fields_Exception(__CONTEXT__,FIELDLIST_TEST_FAILED,
                                   "SAPDB_MemFillNoCheck failed");
        return false;
    }
    field.Assign(persistentField,5);
    if ( ! fields.AppendField (field,errlist) )
    {
        fields.WriteToTrace("fields.AppendField failed");
        field.WriteToTrace("fields.AppendField failed");
        errlist = Fields_Exception(__CONTEXT__,FIELDLIST_TEST_FAILED,
                                   "fields.AppendField failed");
        return false;
    }
	fields.WriteToTrace("fieldlist after append of second field");
    if ( fields.GetFieldLength(2) != 5 )
    {
        fields.WriteToTrace("invalid fieldlength for field 1 returned, 10 is expected");
        errlist = Fields_Exception(__CONTEXT__,FIELDLIST_TEST_FAILED2,
                                   "invalid fieldlength for field 1 returned, 10 is expected",
                                   SAPDB_ToString(fields.GetFieldLength(1)));
        return false;
    }
    fieldListLength = fields.GetTotalLength();
    if ( fieldListLength > 17 )
    {
        fields.WriteToTrace("total length of 17 is expected but found was");
        errlist = Fields_Exception(__CONTEXT__,FIELDLIST_TEST_FAILED2,
                                   "total length of 17 is expected but found was",
                                   SAPDB_ToString(fieldListLength));
        return false;
    }
    field.Deassign();
    if ( fields.GetField (2, field, errlist) != SAPDBFields_IFieldSequence::fieldFound )
    {
        fields.WriteToTrace("fields, GetField(2) must return fieldFound");
        field.WriteToTrace("field GetField(2) must return fieldFound");
        errlist = errlist + Fields_Exception(__CONTEXT__,FIELDLIST_TEST_FAILED,
                                            "GetField(2) must return fieldFound");
        return false;
    }
    if ( ! field.IsAssigned() )
    {
        fields.WriteToTrace("field must be assigned");
        field.WriteToTrace("field must be assigned");
        errlist = Fields_Exception(__CONTEXT__,FIELDLIST_TEST_FAILED,
                                   "field must be assigned");
        return false;
    }
    if ( field.GetLength() != 5 )
    {
        fields.WriteToTrace("field must have length 5");
        field.WriteToTrace("field must have length 5");
        errlist = Fields_Exception(__CONTEXT__,FIELDLIST_TEST_FAILED,
                                   "field must have length 5");
        return false;
    }
    fields.Deassign();
    if ( ! fields.Assign (persistentFields,FieldListLength,errlist,fieldListLength) )
    {
        fields.WriteToTrace("fields.Assign failed");
        errlist = Fields_Exception(__CONTEXT__,FIELDLIST_TEST_FAILED,
                                   "fields.Assign failed");
        return false;
    }
    if ( ! SAPDB_MemFillNoCheck(persistentField,0xCC,MaxFieldLength) )
    {
        fields.WriteToTrace("SAPDB_MemFillNoCheck failed");
        errlist = Fields_Exception(__CONTEXT__,FIELDLIST_TEST_FAILED,
                                   "SAPDB_MemFillNoCheck failed");
        return false;
    }
    field.Assign(persistentField,MaxFieldLength);
    if ( ! fields.AppendField (field,errlist) )
    {
        fields.WriteToTrace("fields.AppendField failed");
        field.WriteToTrace("fields.AppendField failed");
        errlist = Fields_Exception(__CONTEXT__,FIELDLIST_TEST_FAILED,
                                   "fields.AppendField failed");
        return false;
    }
	fields.WriteToTrace("fieldlist after append of third field");
    fieldListLength = fields.GetTotalLength();
    if ( fieldListLength > 320 )
    {
        fields.WriteToTrace("total length of 320 is expected but found was");
        errlist = Fields_Exception(__CONTEXT__,FIELDLIST_TEST_FAILED2,
                                   "total length of 17 is expected but found was",
                                   SAPDB_ToString(fieldListLength));
        return false;
    }
    field.Deassign();
    if ( fields.GetField (3, field, errlist) != SAPDBFields_IFieldSequence::fieldFound )
    {
        fields.WriteToTrace("fields, GetField(3) must return fieldFound");
        field.WriteToTrace("field GetField(3) must return fieldFound");
        errlist = errlist + Fields_Exception(__CONTEXT__,FIELDLIST_TEST_FAILED,
                                            "GetField(3) must return fieldFound");
        return false;
    }
    if ( ! field.IsAssigned() )
    {
        fields.WriteToTrace("field must be assigned");
        field.WriteToTrace("field must be assigned");
        errlist = Fields_Exception(__CONTEXT__,FIELDLIST_TEST_FAILED,
                                   "field must be assigned");
        return false;
    }
    if ( field.GetLength() != 300 )
    {
        fields.WriteToTrace("field must have length 300");
        field.WriteToTrace("field must have length 300");
        errlist = Fields_Exception(__CONTEXT__,FIELDLIST_TEST_FAILED,
                                   "field must have length 300");
        return false;
    }
    fields.Deassign();
    if ( ! fields.Assign (persistentFields,FieldListLength,errlist,fieldListLength) )
    {
        fields.WriteToTrace("fields.Assign failed");
        errlist = Fields_Exception(__CONTEXT__,FIELDLIST_TEST_FAILED,
                                   "fields.Assign failed");
        return false;
    }
	fields.WriteToTrace("fieldlist after Assign with 3 fields");
    return true;
}
