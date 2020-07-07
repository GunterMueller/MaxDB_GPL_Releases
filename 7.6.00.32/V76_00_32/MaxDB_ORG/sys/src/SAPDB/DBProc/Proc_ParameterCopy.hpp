/*!*********************************************************************

    @file         Proc_ParameterCopy.hpp

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

#ifndef PROC_PARAMETERCOPY_HPP
#define PROC_PARAMETERCOPY_HPP

#include "SAPDBCommon/Fields/SAPDBFields_AttributedField.hpp"
#include "SAPDBCommon/Fields/SAPDBFields_IFieldSequences.hpp"

class SAPDBMem_IRawAllocator;
class SAPDBFields_FieldMap;
class KSQL_PreparedStatement;

/*!

  @class Proc_ParameterCopy
  @brief copy db proc parameters to prevent overwriting
    them through a nested statement context

 */
class Proc_ParameterCopy
    : public SAPDBFields_IFieldSequence
{
public:
    Proc_ParameterCopy (SAPDBMem_IRawAllocator     * allocator,
                        SAPDBFields_IFieldSequence * parameterMap);
    ~Proc_ParameterCopy ();
    void bindParameter (KSQL_PreparedStatement * stmt, int paramIndex, bool binary = true);

    virtual IndexType GetFieldCount() const;

    virtual GetFieldResult GetField ( IndexType                    fieldno,
                                      SAPDBFields_AttributedField &field,
                                      SAPDBErr_MessageList        &errlist) const;

    virtual SizeType GetFieldLength(IndexType FieldNo) const;

    virtual SizeType GetTotalLength() const;
    /*!
        @brief reset the fields to accommodate output parameters
     */
    void Reset (int newBufferSize);

    /*!
        @brief copy a parameter into the collection
     */
    void StoreParameter (IndexType index, const void * data,
            int len, tsp00_Byte definedByte);

    /*!
        @brief change a parameter slot to point to external memory

        The memory of the new value is not controlled by Proc_ParameterCopy,
        it is up to the caller to deallocate this memory.

        The pointer must include the defined byte.
     */
    void PatchParameter (IndexType index, void * data, int len);

    /*!
        @brief copy cursor name to return to client
     */
    void StoreCursorName (const tsp00_Byte * nameptr, int len);

    /*!
        @brief retrieve cursor name
     */
    bool GetCursorName (const tsp00_Char * & nameptr, int & len);


private:
    SAPDBMem_IRawAllocator      * allocator;
    int                           count;
    SAPDBFields_AttributedField * fields;
    tsp00_Byte                  * raw;
    int                           rawSize;
    tsp00_Byte                  * writePosition;
    int                           cursorNameLen;
    tsp00_KnlIdentifier           cursorName;
};


#endif

