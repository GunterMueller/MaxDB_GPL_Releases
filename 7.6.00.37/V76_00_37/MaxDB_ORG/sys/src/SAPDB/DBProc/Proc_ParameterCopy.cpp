/*!*********************************************************************

    @file         Proc_ParameterCopy.cpp

    @author       DanielD

    @brief

\if EMIT_LICENCE

    ========== licence begin  GPL
    Copyright (c) 2003-2005 SAP AG

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

#include "DBProc/Proc_ParameterCopy.hpp"

#include "SAPDBCommon/MemoryManagement/SAPDBMem_IRawAllocator.hpp"
#include "SAPDBCommon/Fields/SAPDBFields_FieldMap.hpp"
#include "SQLManager/KernelSQL/KSQL_PreparedStatement.hpp"


/*----------------------------------------*/

Proc_ParameterCopy::Proc_ParameterCopy (
    SAPDBMem_IRawAllocator     * allocator_init,
    SAPDBFields_IFieldSequence * parameterMap)
    : cursorNameLen(-1)
{
    /*
     * alloc memory
     */
    this->allocator = allocator_init;
    int fieldCount = parameterMap->GetFieldCount ();
    int dataLength = parameterMap->GetTotalLength ();
    int fieldsSize = fieldCount * sizeof (SAPDBFields_AttributedField);
    int requiredLength = fieldsSize + dataLength;
    this->count = fieldCount;
    this->raw = static_cast<tsp00_Byte*> (this->allocator->Allocate (requiredLength));
    this->rawSize = requiredLength;
    this->fields = reinterpret_cast<SAPDBFields_AttributedField *>(this->raw);
    /*
     * copy fields
     */
    tsp00_Byte * writePos = this->raw + fieldsSize;
    SAPDBFields_AttributedField sourceField;
    SAPDBErr_MessageList        errlist;
    for (int i = 0; i < fieldCount; ++i) {
        if( parameterMap->GetField (i, sourceField, errlist)
            !=
            SAPDBFields_IFieldSequence::fieldFound )
            RTE_Crash(errlist);
        int fieldLen = sourceField.GetLength ();
        memcpy (writePos, sourceField.GetDataPtr (), fieldLen);
        this->fields[i].Assign (writePos, fieldLen);
        writePos += fieldLen;
    }
}

/*----------------------------------------*/

Proc_ParameterCopy::~Proc_ParameterCopy ()
{
    if (this->raw) {
        this->allocator->Deallocate (this->raw);
    }
}

/*----------------------------------------*/

void Proc_ParameterCopy::
bindParameter (
    KSQL_PreparedStatement * stmt,
    int                      paramIndex,
    bool                     binary)
{
    SAPDBFields_AttributedField  field;
    SAPDBErr_MessageList         errlist;
    bool               isNull;
    const SAPDB_Byte * data;
    int                len;
    /*
     * get field info
     */
    if ( this->GetField (paramIndex, field, errlist)
         !=
         SAPDBFields_IFieldSequence::fieldFound )
         RTE_Crash(errlist);
    data = field.GetDataPtr ();
    if (data [0] == 0xff) {
        isNull = true;
        data = NULL;
        len = 0;
    }
    else {
        isNull = false;
        data = data + 1;
        len = field.GetLength () - 1;
    }
    /*
     * bind parameter
     */
    if (binary) {
        stmt->bindInput (paramIndex + 1, data, len);
    }
    else {
        stmt->bindAsciiInput (paramIndex + 1, (const char *) data, len);
    }
}

/*----------------------------------------*/

SAPDBFields_IFieldSequence::IndexType Proc_ParameterCopy::
GetFieldCount() const
{
    return this->count;
};

/*----------------------------------------*/

SAPDBFields_IFieldSequence::GetFieldResult
Proc_ParameterCopy::GetField ( IndexType                    fieldno,
                               SAPDBFields_AttributedField &field,
                               SAPDBErr_MessageList        &errlist) const
{
    field = this->fields [fieldno];
    return SAPDBFields_IFieldSequence::fieldFound;
}

/*----------------------------------------*/

SAPDBFields_AttributedField::SizeType Proc_ParameterCopy::
GetFieldLength(
    IndexType FieldNo) const
{
    return this->fields [FieldNo].GetLength ();
}

/*----------------------------------------*/
SAPDBFields_IFieldSequence::SizeType
Proc_ParameterCopy::GetTotalLength() const
{
    SAPDBFields_IFieldSequence::SizeType length = 0;
    for (int i = 0; i < this->count; ++i) {
        length += this->fields [i].GetLength ();
    }
    return length;
};

/*----------------------------------------*/

void Proc_ParameterCopy::
Reset (
    int newBufferSize)
{
    int sizeOfFields = this->count * sizeof (SAPDBFields_AttributedField);
    int requiredSize = newBufferSize + sizeOfFields;
    if (requiredSize > this->rawSize) {
        this->allocator->Deallocate (this->raw);
        this->raw = (tsp00_Byte *) this->allocator->Allocate (requiredSize);
    }
    this->writePosition = this->raw + sizeOfFields;
    for (int i = 0; i < this->count; ++i) {
        this->fields [i].Deassign ();
    }
    this->cursorNameLen = -1;
}

/*----------------------------------------*/

void Proc_ParameterCopy::
StoreParameter (
    IndexType   index,
    const void* data,
    int         len,
    tsp00_Byte  definedByte)
{
    *this->writePosition = definedByte;
    ++this->writePosition;
    memcpy (this->writePosition, data, len);
    this->fields [index].Assign (this->writePosition, len);
    this->writePosition += len;
}

/*----------------------------------------*/

void Proc_ParameterCopy::
PatchParameter (
    IndexType   index,
    void      * data,
    int         len)
{
    this->fields [index].Assign (data, len);
}

/*----------------------------------------*/

void Proc_ParameterCopy::
StoreCursorName (
    const tsp00_Byte * nameptr,
    int len)
{
    memcpy (this->cursorName, nameptr, len);
    this->cursorNameLen = len;
}

/*----------------------------------------*/

bool Proc_ParameterCopy::
GetCursorName (
    const tsp00_Char * & nameptr,
    int & len)
{
    if (this->cursorNameLen >= 0) {
        nameptr = this->cursorName;
        len = this->cursorNameLen;
        return true;
    }
    else {
        return false;
    }
}
